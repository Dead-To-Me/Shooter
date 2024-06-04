// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "SGameModeBase.h"
#include "SGrenade.h"
#include "Components/SCharacterMovement.h"
#include "SWeaponManagerComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SHeathComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "SPlayerController.h"
#include "SWeaponData.h"
#include "SWeaponFirearm.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Shooter27/Anim/SAnimInstance.h"
#include "Shooter27/Widgets/SPlayerTagWidget.h"
#include "PaperSpriteComponent.h"
#include "SPlayerState.h"
#include "Kismet/KismetMathLibrary.h"


void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ASCharacter, bLeftMouseButtonPressed);
    DOREPLIFETIME(ASCharacter, bAiming);
    DOREPLIFETIME(ASCharacter, bDetected);
    DOREPLIFETIME(ASCharacter, bSprinting);
}

ASCharacter::ASCharacter(const FObjectInitializer& ObjectInitializer) : Super(
    ObjectInitializer.SetDefaultSubobjectClass<USCharacterMovement>(CharacterMovementComponentName))
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    GetCharacterMovement()->SetIsReplicated(true);

    bLeftMouseButtonPressed = false;
    bAiming = false;
    LifeSpanAfterDead = 30.0f;
    FieldOfViewScope = 55.0f;
    FieldOfView = 110.0f;
    ViewCheckRate = 0.2f;
    ViewCheckRadius = 15.0f;
    ViewDistanceCheck = 50000.0f;
    InteractDistance = 300.0f;
    GrenadeCheckRadius = 15.f;
    bDetected = false;
    DetectedFadeOutTime = 0.75f;
    LandVelocity = FVector2D(1300, 4000);
    FallDamageMultiplier = 100;
    HeadBoneName = "Head";
    HeadStabilizationSpeed = 3;

    GetMesh()->SetOwnerNoSee(true);

    OwnerMesh = CreateDefaultSubobject<USkeletalMeshComponent>("OwnerMesh");
    OwnerMesh->SetupAttachment(GetMesh());
    OwnerMesh->SetOnlyOwnerSee(true);

    HeathComponent = CreateDefaultSubobject<USHeathComponent>("HeathComponent");

    Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
    Camera->SetupAttachment(OwnerMesh, HeadBoneName);
    Camera->bUsePawnControlRotation = true;

    WeaponManager = CreateDefaultSubobject<USWeaponManagerComponent>("WeaponManager");

    PlayerBarWidget = CreateDefaultSubobject<UWidgetComponent>("PlayerBarWidget");
    PlayerBarWidget->SetupAttachment(GetMesh(), HeadBoneName);
    PlayerBarWidget->SetOwnerNoSee(true);

    InteractSphere = CreateDefaultSubobject<USphereComponent>("InteractSphere");
    InteractSphere->SetupAttachment(GetRootComponent());

    MapCapture = CreateDefaultSubobject<USceneCaptureComponent2D>("MapCapture");
    MapCapture->SetupAttachment(GetRootComponent());
    MapCapture->ProjectionType = ECameraProjectionMode::Orthographic;
    MapCapture->OrthoWidth = 8192;

    PlayerIcon = CreateDefaultSubobject<UPaperSpriteComponent>("PlayerIcon");
    PlayerIcon->SetupAttachment(GetRootComponent());
    PlayerIcon->bVisibleInSceneCaptureOnly = true;
    PlayerIcon->SetOwnerNoSee(true);
    PlayerIcon->SetReceivesDecals(false);
}


// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
    Super::BeginPlay();
    OnTakePointDamage.AddDynamic(this, &ASCharacter::OnTakePointDamageHandle);
    OnTakeRadialDamage.AddDynamic(this, &ASCharacter::OnTakeRadialDamageHandle);

    AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
    SCharacterMovement = GetCharacterMovement<USCharacterMovement>();
    SPlayerController = GetController<ASPlayerController>();

    if (HasAuthority())
        GetWorldTimerManager().SetTimer(ViewCheckTimer, this, &ASCharacter::CheckIfViewTarget, ViewCheckRate, true);

    if (!IsLocallyControlled())
        return;

    GetWorldTimerManager().SetTimer(ViewCheckTimer, this, &ASCharacter::CheckIfViewTarget, ViewCheckRate, true);
    InteractSphere->OnComponentBeginOverlap.AddDynamic(this, &ASCharacter::OnBeginOverlapSphere);
    InteractSphere->OnComponentEndOverlap.AddDynamic(this, &ASCharacter::OnEndOverlapSphere);
}

void ASCharacter::MoveForward(float Value)
{
    AddMovementInput(GetActorForwardVector(), Value);
}

void ASCharacter::MoveRight(float Value)
{
    AddMovementInput(GetActorRightVector(), Value);
}

void ASCharacter::LookUp(float Value)
{
    AddControllerPitchInput(Value);
}

void ASCharacter::TurnAround(float Value)
{
    AddControllerYawInput(Value);
}

void ASCharacter::CheckIfViewTarget() const
{
    FHitResult Hit;
    FVector StartPoint = Camera->GetComponentLocation() + Camera->GetForwardVector();
    FVector EndPoint = Camera->GetComponentLocation() + Camera->GetForwardVector() * ViewDistanceCheck;
    FCollisionQueryParams Params;
    GetIgnoreCollisionParams(Params);
    if (!GetWorld()->SweepSingleByChannel(Hit, StartPoint, EndPoint, FQuat::Identity,
                                          ECC_Visibility, FCollisionShape::MakeSphere(ViewCheckRadius), Params))
        return;
    if (!(Hit.GetActor() && Hit.GetActor()->Implements<USInteractInterface>()))
        return;
    Execute_BecomeViewTargetInterface(Hit.GetActor());

    if (!IsLocallyControlled())
        return;
    if (InteractDistance < FVector::Dist(Hit.GetActor()->GetActorLocation(), StartPoint))
        return;
    Execute_ShowInteractWidget(Hit.GetActor());
}

void ASCharacter::BecomeViewTargetInterface_Implementation()
{
    if (!HasAuthority())
    {
        Cast<USPlayerTagWidget>(PlayerBarWidget->GetWidget())->ShowPlayerTag();
        return;
    }
    bDetected = true;
    GetWorldTimerManager().ClearTimer(DetectedFadeOutTimer);
    GetWorldTimerManager().SetTimer(DetectedFadeOutTimer, this, &ASCharacter::DetectedFadeOut, DetectedFadeOutTime);
}

void ASCharacter::Landed(const FHitResult& Hit)
{
    Super::Landed(Hit);
    if (IsLocallyControlled())
    {
        // TODO: Fall Sound
        return;
    }
    const float FallDamage = abs(UKismetMathLibrary::NormalizeToRange(GetVelocity().Z, LandVelocity.X, LandVelocity.Y))
        * FallDamageMultiplier;
    UGameplayStatics::ApplyDamage(this, FallDamage, GetController(), this, UDamageType::StaticClass());
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);
    PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::LookUp);
    PlayerInputComponent->BindAxis("TurnAround", this, &ASCharacter::TurnAround);
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ASCharacter::StopJumping);
    PlayerInputComponent->BindAction("Fire", IE_Pressed, WeaponManager, &USWeaponManagerComponent::StartFire);
    PlayerInputComponent->BindAction("Fire", IE_Released, WeaponManager, &USWeaponManagerComponent::StopFire);
    PlayerInputComponent->BindAction("Aiming", IE_Pressed, this, &ASCharacter::BeginAiming);
    PlayerInputComponent->BindAction("Aiming", IE_Released, this, &ASCharacter::StopAiming);
    PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::WantToCrouch);
    PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASCharacter::EndCrouch);
    PlayerInputComponent->BindAction("Reload", IE_Pressed, WeaponManager,
                                     &USWeaponManagerComponent::ServerReload);
    PlayerInputComponent->BindAction("ChangeFiremode", IE_Pressed, WeaponManager,
                                     &USWeaponManagerComponent::ServerChangeWeaponFiremode);
    PlayerInputComponent->BindAction("Grenade", IE_Released, WeaponManager, &USWeaponManagerComponent::ThrowGrenade);
    PlayerInputComponent->BindAction("PrimaryWeapon", IE_Pressed, this, &ASCharacter::SelectPrimaryWeapon);
    PlayerInputComponent->BindAction("SecondaryWeapon", IE_Pressed, this, &ASCharacter::SelectSecondaryWeapon);
    PlayerInputComponent->BindAction("Gadget", IE_Pressed, this, &ASCharacter::SelectGadget);
    PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ASCharacter::ClientCheckInteract);
    PlayerInputComponent->BindAxis("WeaponScroll", WeaponManager, &USWeaponManagerComponent::ScrollWeapon);
    PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASCharacter::BeginSprinting);
    PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASCharacter::EndSprinting);
    PlayerInputComponent->BindAction("GrenadeThrowBack", IE_Pressed, this, &ASCharacter::GrenadeThrowBack);
    PlayerInputComponent->BindAction("CrouchToggle", IE_Pressed, this, &ASCharacter::CrouchToggle);
    PlayerInputComponent->BindAction("Gadget2", IE_Pressed, this, &ASCharacter::SelectSecondGadget);
}


void ASCharacter::BeginAiming()
{
    ServerBeginAiming();
    if (IsLocallyControlled())
    {
        SPlayerController->SetMouseSensitivity(SPlayerController->GetAimSens());
        Camera->SetFieldOfView(FieldOfViewScope);
        GetController<ASPlayerController>()->OnFOVChanged.Broadcast(FieldOfViewScope);
    }
}

void ASCharacter::StopAiming()
{
    ServerStopAiming();
    if (IsLocallyControlled())
    {
        SPlayerController->SetMouseSensitivity(SPlayerController->GetDefaultSens());
        Camera->SetFieldOfView(FieldOfView);
        GetController<ASPlayerController>()->OnFOVChanged.Broadcast(FieldOfView);
    }
}

void ASCharacter::ServerStopAiming_Implementation()
{
    bAiming = false;
}

void ASCharacter::ServerBeginAiming_Implementation()
{
    bAiming = true;
}

bool ASCharacter::CanCrouch() const
{
    return Super::CanCrouch() and !GetCharacterMovement()->IsFalling();
}

void ASCharacter::WantToCrouch()
{
    Crouch();
}

void ASCharacter::EndCrouch()
{
    UnCrouch();
}

void ASCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    WeaponManager->SpawnWeapons();
}

void ASCharacter::UnPossessed()
{
    Super::UnPossessed();
    if (WeaponManager)
        WeaponManager->GetCurrentWeapon()->StopShooting();
    SetCharacterRagdoll();
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SetLifeSpan(LifeSpanAfterDead);
    GetMesh()->bPauseAnims = true;
    OwnerMesh->bPauseAnims = true;
}

void ASCharacter::SetCharacterRagdoll_Implementation()
{
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetMesh()->SetSimulatePhysics(true);
    OwnerMesh->SetSimulatePhysics(true);
    OwnerMesh->bPauseAnims = true;
    GetMesh()->bPauseAnims = true;
    PlayerIcon->SetSprite(DeathSprite);
}


bool ASCharacter::IsMouseMoving() const
{
    return abs(GetInputAxisValue("LookUp")) > 0.1f || abs(GetInputAxisValue("TurnAround")) > 0.1f;
}

void ASCharacter::SetPitch(const float NewPitch) const
{
    // if (SPlayerController->RotationInput.Pitch > 0.1f || SPlayerController->RotationInput.Yaw > 0.1f)
    FRotator NewRot = GetControlRotation();
    NewRot.Pitch = NewPitch;
    SPlayerController->SetControlRotation(NewRot);
}

void ASCharacter::SetYaw(const float NewYaw) const
{
    SPlayerController->RotationInput.Yaw = NewYaw;
}

void ASCharacter::SetSprinting(bool NewState)
{
    bSprinting = NewState;
}

bool ASCharacter::IsWantToSprint() const
{
    return SCharacterMovement->GetWantToSprint();
}

void ASCharacter::PlayChangeWeaponMontage()
{
    OwnerMesh->GetAnimInstance()->Montage_Play(ChangeWeaponMontage);
    GetMesh()->GetAnimInstance()->Montage_Play(ChangeWeaponMontage);
}

void ASCharacter::OutsideWorldBounds()
{
    Super::OutsideWorldBounds();
    // TODO: 
}

void ASCharacter::OnRep_Owner()
{
    Super::OnRep_Owner();
    if (GetOwner())
        return;
    GetWorldTimerManager().ClearTimer(ViewCheckTimer);
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASCharacter::AddRecoil_Implementation(const FVector2D& Recoil)
{
    AddControllerPitchInput(Recoil.X);
    AddControllerYawInput(Recoil.Y);
}

void ASCharacter::ServerCheckInteract_Implementation()
{
    CheckInteract();
}

void ASCharacter::BeginSprinting()
{
    ServerBeginSprint();
    SCharacterMovement->SetWantToSprint(true);
    bSprinting = SCharacterMovement->CanSprint();
}

void ASCharacter::EndSprinting()
{
    ServerEndSprint();
    SCharacterMovement->SetWantToSprint(false);
    bSprinting = false;
}

void ASCharacter::ServerBeginSprint_Implementation()
{
    SCharacterMovement->SetWantToSprint(true);
    bSprinting = SCharacterMovement->CanSprint();
}

void ASCharacter::ServerEndSprint_Implementation()
{
    SCharacterMovement->SetWantToSprint(false);
    bSprinting = false;
}

void ASCharacter::OnRep_IsSprinting()
{
}

void ASCharacter::CrouchToggle()
{
    if (bIsCrouched)
        UnCrouch();
    else
        Crouch();
}

void ASCharacter::Jump()
{
    if (bIsCrouched)
        UnCrouch();
    Super::Jump();
}

void ASCharacter::StopJumping()
{
    Super::StopJumping();
    // TODO: Check if Want To Crouch
}

USkeletalMeshComponent* ASCharacter::GetMeshTrue() const
{
    return IsLocallyControlled() ? OwnerMesh : GetMesh();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ASCharacter::OnTakePointDamageHandle(AActor* DamagedActor, float Damage, AController* InstigatedBy,
                                          FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName,
                                          FVector ShotFromDirection, const UDamageType* DamageType,
                                          AActor* DamageCauser)
{
    if (!InstigatedBy || !GetPlayerState())
        return;
    if (GetPlayerState<ASPlayerState>()->GetRelation(InstigatedBy->GetPlayerState<ASPlayerState>()) != ERelation::Enemy)
        return;
    ASWeapon* Weapon = Cast<ASWeapon>(DamageCauser);
    DamageCauser = Weapon ? Weapon : DamageCauser;
    HeathComponent->DealDamage(Damage, InstigatedBy, DamageCauser);
    if (!GetController())
        return;
    GetController<ASPlayerController>()->OnPlayerDamageCaller(ShotFromDirection);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ASCharacter::OnTakeRadialDamageHandle(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                           FVector Origin, const FHitResult& HitInfo, AController* InstigatedBy,
                                           AActor* DamageCauser)
{
    if (!GetPlayerState() || !InstigatedBy)
        return;
    if (GetPlayerState<ASPlayerState>()->GetRelation(InstigatedBy->GetPlayerState<ASPlayerState>()) != ERelation::Enemy)
        return;
    HeathComponent->DealDamage(Damage, InstigatedBy, DamageCauser);
}


void ASCharacter::SelectPrimaryWeapon()
{
    if (WeaponManager->GetCurrentWeapon())
    {
        if (WeaponManager->GetCurrentWeapon()->InventorySlot == EInventorySlot::Primary)
            return;
    }
    WeaponManager->ServerChangeWeapon(EInventorySlot::Primary);
    PlayChangeWeaponMontage();
}

void ASCharacter::SelectSecondaryWeapon()
{
    if (WeaponManager->GetCurrentWeapon())
    {
        if (WeaponManager->GetCurrentWeapon()->InventorySlot == EInventorySlot::Secondary)
            return;
    }
    WeaponManager->ServerChangeWeapon(EInventorySlot::Secondary);
    PlayChangeWeaponMontage();
}

void ASCharacter::SelectGadget()
{
    if (WeaponManager->GetCurrentWeapon())
    {
        if (WeaponManager->GetCurrentWeapon()->InventorySlot == EInventorySlot::Gadget)
            return;
    }
    WeaponManager->ServerChangeWeapon(EInventorySlot::Gadget);
    PlayChangeWeaponMontage();
}

void ASCharacter::SelectSecondGadget()
{
    if (WeaponManager->GetCurrentWeapon())
    {
        if (WeaponManager->GetCurrentWeapon()->InventorySlot == EInventorySlot::Gadget2)
            return;
    }
    WeaponManager->ServerChangeWeapon(EInventorySlot::Gadget2);
    PlayChangeWeaponMontage();
}

void ASCharacter::CheckInteract()
{
    FHitResult Hit;
    FVector EndTraceLocation = Camera->GetComponentLocation() + Camera->GetForwardVector() * InteractDistance;
    FCollisionQueryParams Params = FCollisionQueryParams::DefaultQueryParam;
    Params.AddIgnoredActor(this);
    if (!GetWorld()->LineTraceSingleByChannel(Hit, Camera->GetComponentLocation(), EndTraceLocation, ECC_Visibility,
                                              Params))
        return;
    if (!Hit.GetActor())
        return;
    if (!Hit.GetActor()->Implements<USInteractInterface>())
        return;
    Execute_Interact(Hit.GetActor(), this);
}

void ASCharacter::ClientCheckInteract()
{
    ServerCheckInteract();
    CheckInteract();
}


void ASCharacter::GrenadeThrowBack()
{
    FHitResult Hit;
    FVector StartPoint = Camera->GetComponentLocation() + Camera->GetForwardVector();
    FVector EndPoint = Camera->GetComponentLocation() + Camera->GetForwardVector() * InteractDistance;
    FCollisionQueryParams Params;
    if (!GetWorld()->SweepSingleByChannel(Hit, StartPoint, EndPoint, FQuat::Identity, ECC_Visibility,
                                          FCollisionShape::MakeSphere(GrenadeCheckRadius),
                                          GetIgnoreCollisionParams(Params)))
        return;
    ASGrenade* Grenade = Cast<ASGrenade>(Hit.GetActor());
    if (!Grenade)
        return;
    ServerGrenadeThrowBack(Grenade);
    FVector NewVel = GetActorForwardVector() * 1800 + GetActorUpVector() * 700; // TODO: To Var
    Grenade->GetGrenadeProjectile()->SetUpdatedComponent(Grenade->GetGrenadeMesh());
    Grenade->GetGrenadeProjectile()->Velocity = NewVel;
}

void ASCharacter::DetectedFadeOut()
{
    bDetected = false;
}


FCollisionQueryParams& ASCharacter::GetIgnoreCollisionParams(FCollisionQueryParams& Params) const
{
    Params.AddIgnoredActor(this);
    TArray<AActor*> CChildren;
    GetAllChildActors(CChildren);
    Params.AddIgnoredActors(CChildren);
    return Params;
}


void ASCharacter::ServerGrenadeThrowBack_Implementation(ASGrenade* Grenade)
{
    Grenade->SetInstigator(this);
    if (FVector::Dist(Grenade->GetActorLocation(), GetActorLocation()) > InteractDistance + 20)
        return;
    const FVector NewVel = GetActorForwardVector() * 1800 + GetActorUpVector() * 700;
    Grenade->GetGrenadeProjectile()->SetUpdatedComponent(Grenade->GetGrenadeMesh());
    Grenade->GetGrenadeProjectile()->Velocity = NewVel;
}

void ASCharacter::ServerRedeployment_Implementation()
{
    ASGameModeBase* GameMode = Cast<ASGameModeBase>(UGameplayStatics::GetGameMode(this));
    if (!GameMode)
        return;
    GameMode->Redeployment(this);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void ASCharacter::OnBeginOverlapSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                       const FHitResult& SweepResult)
{
    if (!OtherActor->Implements<USInteractInterface>())
        return;
    Execute_ShowInteractWidget(OtherActor);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void ASCharacter::OnEndOverlapSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (!OtherActor->Implements<USInteractInterface>())
        return;
    Execute_ShowInteractWidget(OtherActor);
}

void ASCharacter::OnRepDetected_Implementation()
{
}

void ASCharacter::OnPlayerDamageHandle_Implementation(const FVector& Direction)
{
}
