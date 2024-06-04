// Shooter27
#include "SWeaponFirearm.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "SBulletBase.h"
#include "SCharacter.h"
#include "SWeaponManagerComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SWeaponAttachmentLaser.h"
#include "Components/SWeaponAttachmentMagComponent.h"
#include "Components/SWeaponAttachmentScope.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundCue.h"


void ASWeaponFirearm::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ASWeaponFirearm, Firemode);
    DOREPLIFETIME(ASWeaponFirearm, Scope);
    DOREPLIFETIME(ASWeaponFirearm, Mag);
    DOREPLIFETIME(ASWeaponFirearm, bBipod);
    DOREPLIFETIME_CONDITION(ASWeaponFirearm, CurrentAmmo, COND_OwnerOnly);
    DOREPLIFETIME_CONDITION(ASWeaponFirearm, CurrentAmmoMagazine, COND_OwnerOnly);
}

void ASWeaponFirearm::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
    Super::PreReplication(ChangedPropertyTracker);
    //DOREPLIFETIME_ACTIVE_OVERRIDE_FAST(ASWeaponFirearm, bBipod, bHaveBipod);
}


ASWeaponFirearm::ASWeaponFirearm()
{
    SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>("SceneCapture");
    SceneCapture->SetupAttachment(WeaponMesh);
    bHaveBipod = false;
    bBipod = false;
    Firemode = EFiremode::Automatic;
    bCanChangeFiremode = true;
    CurrentBulletSpread = 0;
    bFirstShot = true;
    bCanAiming = true;
    CurrentAmmo = 0;
    CurrentAmmoMagazine = 0;
    AimingSpreadMultiplier = 0.1;
    CrouchSpreadMultiplier = 0.75;
    FallingSpreadMultiplier = 2.5;
    StandSpreadInc = 0;
    StandSpreadMax = 0;
    MoveSpreadInc = 0;
    MoveSpreadMax = 0;
    BulletOffset = 40;
    ReloadLeftThreshold = 0.5;
}

void ASWeaponFirearm::StartShooting()
{
    if (IsShooting)
        return;

    IsShooting = true;

    MakeShoot();

    switch (Firemode)
    {
    case EFiremode::SemiAutomatic:
        GetWorldTimerManager().SetTimer(ShootTimer, this, &ASWeaponFirearm::StopShooting, ShootRate, false);
        break;
    case EFiremode::Automatic:
        GetWorldTimerManager().SetTimer(ShootTimer, this, &ASWeaponFirearm::MakeShoot, ShootRate, true);
        break;
    }
}

void ASWeaponFirearm::MakeShoot_Implementation()
{
    if (!CanShoot())
    {
        StopShooting();
        return;
    }

    CurrentBulletSpread = Character->GetVelocity().Length() > 0
                              ? FMath::Clamp(CurrentBulletSpread + MoveSpreadInc, 0, MoveSpreadMax)
                              : FMath::Clamp(CurrentBulletSpread + StandSpreadInc, 0, StandSpreadMax);
    CurrentBulletSpread *= Character->IsAiming() ? AimingSpreadMultiplier : 1;
    CurrentBulletSpread *= Character->bIsCrouched ? CrouchSpreadMultiplier : 1;
    CurrentBulletSpread *= Character->GetMovementComponent()->IsFalling() ? FallingSpreadMultiplier : 1;
    const float HalfRad = FMath::DegreesToRadians(CurrentBulletSpread);
    const FVector Spread = FMath::VRandCone(Character->GetViewRotation().Vector(), HalfRad);
    // FTransform SpawnTransform = WeaponMesh->GetSocketTransform(MuzzleSocket);
    FTransform SpawnTransform = Character->GetCamera()->GetComponentTransform();
    SpawnTransform.SetLocation(SpawnTransform.GetLocation() + SpawnTransform.GetRotation().Vector() * BulletOffset);
    SpawnTransform.SetRotation(FQuat::FindBetweenVectors(FVector::ForwardVector, Spread));

    ASBulletBase* Bullet = GetWorld()->SpawnActor<ASBulletBase>(Attachments.BulletClass, SpawnTransform);
    Bullet->SetInstigator(Character);
    Bullet->SetOwner(this);

    CurrentAmmoMagazine -= 1;
    bFirstShot = false;
}

const USceneComponent* ASWeaponFirearm::GetScopeMesh() const
{
    if (!bCanAiming)
        return nullptr;

    if (Scope)
        return Scope;

    return WeaponMesh;
}

const USceneComponent* ASWeaponFirearm::GetMagMesh() const
{
    return Mag;
}

void ASWeaponFirearm::GetCalculatedRecoil(FVector2D& CalculatedRecoil)
{
    if (!Character)
        return;
    // TODO: Normal Movement State (Maybe Set/Array/Map With States Like Crouch = 0.5, Walking = 1.0, Prone = 0.25)
    Recoil.PitchMultiplier = Character->bIsCrouched ? 0.65f : 1.0f;
    Recoil.YawMultiplier = Character->bIsCrouched ? 0.65f : 1.0f;
    Recoil.PitchMultiplier *= Character->IsAiming() ? 0.85f : 1.0f;
    Recoil.YawMultiplier *= Character->IsAiming() ? 0.85f : 1.0f;
    Recoil.PitchMultiplier *= bBipod ? 0.15f : 1.0f;
    Recoil.YawMultiplier *= bBipod ? 0.15f : 1.0f;

    CalculatedRecoil.X = -(FMath::FRandRange(Recoil.PitchMin, Recoil.PitchMax) * Recoil.PitchMultiplier);
    CalculatedRecoil.Y = (FMath::RandBool() ? 1 : -1) * FMath::FRandRange(Recoil.YawMin, Recoil.YawMax) * Recoil.
        YawMultiplier;
}

void ASWeaponFirearm::SetupWeaponAttachments(const FWeaponAttachments* WeaponAttachments)
{
    Attachments = WeaponAttachments ? *WeaponAttachments : Attachments;

    if (Attachments.ScopeClass->IsValidLowLevel())
    {
        Scope = Cast<USWeaponAttachmentScope>(
            AddComponentByClass(Attachments.ScopeClass, true, FTransform::Identity,
                                false));
        Scope->AttachToComponent(WeaponMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, ScopeSocket);
    }

    if (Attachments.MagClass->IsValidLowLevel())
    {
        Mag = Cast<USWeaponAttachmentMagComponent>(
            AddComponentByClass(Attachments.MagClass, true, WeaponMesh->GetSocketTransform(MagazineSocket),
                                false));
        Mag->AttachToComponent(WeaponMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, MagazineSocket);
        CurrentAmmoMagazine = Mag->GetMaxAmmoInMagazine();
        CurrentAmmo = Mag->GetMaxAmmo();
    }
    // TODO: Muzzle Component
    if (Attachments.Laser->IsValidLowLevel())
    {
        USWeaponAttachmentLaser* Laser = Cast<USWeaponAttachmentLaser>(
            AddComponentByClass(Attachments.Laser, true, FTransform::Identity, false));
        Laser->AttachToComponent(WeaponMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, LaserSocket);
    }
}

void ASWeaponFirearm::PlayShotSound()
{
    // TODO: Optimize
    if (Character->IsLocallyControlled())
    {
        UGameplayStatics::PlaySound2D(this, ShotSound);
        return;
    }
    UGameplayStatics::PlaySoundAtLocation(this, ShotSound, GetActorLocation());
}

bool ASWeaponFirearm::CanReload() const
{
    return Super::CanReload() && Mag && CurrentAmmo > 0 && CurrentAmmoMagazine != Mag->GetMaxAmmoInMagazine();
}

bool ASWeaponFirearm::CanShoot() const
{
    return Super::CanShoot() && CurrentAmmoMagazine > 0;
}

void ASWeaponFirearm::OnRep_Reloading()
{
    Super::OnRep_Reloading();
    if (HasAuthority())
        return;

    if (!bReloading)
    {
        if (Character->IsLocallyControlled())
        {
            UGameplayStatics::PlaySound2D(this, ReloadEndSound);
            return;
        }
        UGameplayStatics::PlaySoundAtLocation(this, ReloadEndSound, GetActorLocation());
    }
}

void ASWeaponFirearm::ChangeFiremode()
{
    if (!bCanChangeFiremode)
        return;

    if (IsShooting)
        return;

    switch (Firemode)
    {
    case EFiremode::SemiAutomatic:
        Firemode = EFiremode::Automatic;
        break;
    case EFiremode::Automatic:
        Firemode = EFiremode::SemiAutomatic;
        break;
    }
}

void ASWeaponFirearm::OnRep_Firemode()
{
    if (HasAuthority())
        return;
    if (Character->IsLocallyControlled())
    {
        UGameplayStatics::PlaySound2D(this, ChangeFireModeSound);
        return;
    }
    UGameplayStatics::PlaySoundAtLocation(this, ChangeFireModeSound, GetActorLocation());
}

void ASWeaponFirearm::OnRep_Shooting()
{
    Super::OnRep_Shooting();
    if (HasAuthority())
        return;
    if (Character->IsLocallyControlled())
        return;


    if (IsShooting)
    {
        if (WeaponMesh->GetAnimInstance() && !WeaponMesh->GetAnimInstance()->Montage_IsPlaying(ShotMontage))
            WeaponMesh->GetAnimInstance()->Montage_Play(ShotMontage);
        PlayShotSound();
        if (Firemode == EFiremode::Automatic)
            GetWorldTimerManager().SetTimer(ShotSoundTimer, this, &ASWeaponFirearm::PlayShotSound, ShootRate, true);
    }
    else
    {
        GetWorldTimerManager().ClearTimer(ShotSoundTimer);
    }

    if (MuzzleFlesh)
        MuzzleFlesh->SetActive(IsShooting);
    else
    {
        MuzzleFlesh = UNiagaraFunctionLibrary::SpawnSystemAttached(ShotEffect, WeaponMesh, FName("Muzzle"),
                                                                   FVector::ZeroVector,
                                                                   FRotator::ZeroRotator,
                                                                   EAttachLocation::SnapToTarget,
                                                                   false);
    }
}

void ASWeaponFirearm::AddAmmo()
{
    if (!Mag)
        return;
    if (Mag->GetMaxAmmo() == CurrentAmmo)
        return;
    CurrentAmmo = Mag->GetMaxAmmo() + Mag->GetMaxAmmoInMagazine() - CurrentAmmoMagazine;
}

void ASWeaponFirearm::ClientStartShooting()
{
    Recoil.StartRecoilPosPitch = FRotator::NormalizeAxis(Character->GetControlRotation().Pitch);
    ClientMakeShot();

    switch (Firemode)
    {
    case EFiremode::SemiAutomatic:
        GetWorldTimerManager().SetTimer(ShootTimer, this, &ASWeaponFirearm::ClientStopShooting, ShootRate, false);
        break;
    case EFiremode::Automatic:
        GetWorldTimerManager().SetTimer(ShootTimer, this, &ASWeaponFirearm::ClientMakeShot, ShootRate, true);
        break;
    }
}

void ASWeaponFirearm::ClientStopShooting()
{
    Character->GetWeaponManager()->EnableBackRecoilLerp(Recoil.StartRecoilPosPitch);
    GetWorldTimerManager().ClearTimer(ShootTimer);
    if (MuzzleFlesh)
        MuzzleFlesh->SetActive(false);
}

void ASWeaponFirearm::StartReload()
{
    if (!CanReload())
        return;
    bReloading = true;
    float TimeToReload = CurrentAmmoMagazine / Mag->GetMaxAmmoInMagazine() > ReloadLeftThreshold
                             ? ReloadTimeLeft
                             : ReloadTime;
    GetWorldTimerManager().SetTimer(ReloadTimer, this, &ASWeapon::Reload, TimeToReload);
}

void ASWeaponFirearm::ClientMakeShot()
{
    if (!Character->GetController() || bReloading || Character->IsSprinting() || CurrentAmmoMagazine <= 0)
    {
        ClientStopShooting();
        return;
    }
    if (WeaponMesh->GetAnimInstance() && !WeaponMesh->GetAnimInstance()->Montage_IsPlaying(ShotMontage))
        WeaponMesh->GetAnimInstance()->Montage_Play(ShotMontage);

    StartRecoil();
    PlayShotSound();
    if (MuzzleFlesh)
        MuzzleFlesh->SetActive(true);
    else
    {
        MuzzleFlesh = UNiagaraFunctionLibrary::SpawnSystemAttached(ShotEffect, WeaponMesh, FName("Muzzle"),
                                                                   FVector::ZeroVector,
                                                                   FRotator::ZeroRotator,
                                                                   EAttachLocation::SnapToTarget,
                                                                   false);
    }
}

void ASWeaponFirearm::StartRecoil()
{
    AddRecoil();
}

void ASWeaponFirearm::AddRecoil()
{
    if (!Character)
        return;
    if (!Character->IsLocallyControlled())
        return;
    FVector2D LRecoil;
    GetCalculatedRecoil(LRecoil);
    Character->AddRecoil(LRecoil);
}

void ASWeaponFirearm::Reload()
{
    Super::Reload();

    if (!CanReload())
        return;

    if (!HasAuthority())
        return;

    const int32 DeltaAmmo = Mag->GetMaxAmmoInMagazine() - CurrentAmmoMagazine;
    if (CurrentAmmo >= DeltaAmmo)
    {
        CurrentAmmoMagazine += DeltaAmmo;
        CurrentAmmo -= DeltaAmmo;
    }
    else
    {
        CurrentAmmoMagazine += CurrentAmmo;
        CurrentAmmo = 0;
    }
}


void ASWeaponFirearm::StopShooting_Implementation()
{
    Super::StopShooting_Implementation();
    bFirstShot = true;
    if (CurrentAmmoMagazine == 0)
        StartReload();
}

float ASWeaponFirearm::GetCurrentSpread() const
{
    float Spread = Character->GetVelocity().Length() > 0 ? MoveSpreadMax : StandSpreadMax;
    Spread *= Character->IsAiming() ? AimingSpreadMultiplier : 1;
    Spread *= Character->bIsCrouched ? CrouchSpreadMultiplier : 1;
    Spread *= Character->GetMovementComponent()->IsFalling() ? FallingSpreadMultiplier : 1;
    return Spread;
}
