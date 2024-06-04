// Shooter27


#include "SWeapon.h"
#include "SCharacter.h"
#include "SPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/SCharacterMovement.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASWeapon::ASWeapon()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
    SetRootComponent(WeaponMesh);
}

void ASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ASWeapon, IsShooting);
    DOREPLIFETIME(ASWeapon, bReloading);
}


// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
    Super::BeginPlay();
}

void ASWeapon::OnFOVChangedHandle(float NewFOV)
{
    if (!OwnerWeaponMaterial)
        return;
    OwnerWeaponMaterial->SetScalarParameterValue(WeaponFOVParamName, NewFOV);
}

void ASWeapon::OnRep_Shooting()
{
}

void ASWeapon::MakeShoot_Implementation()
{
}

void ASWeapon::StopShooting_Implementation()
{
    IsShooting = false;
    GetWorldTimerManager().ClearTimer(ShootTimer);
}

bool ASWeapon::CanShoot() const
{
    return !bReloading && Character->IsLeftMouseButtonPressed() && Character->GetController();
}

bool ASWeapon::CanReload() const
{
    return !IsShooting && !bReloading;
}

void ASWeapon::Reload()
{
    bReloading = false;
    GetWorldTimerManager().ClearTimer(ReloadTimer);
}

void ASWeapon::StartReload()
{
    if (!CanReload())
        return;
    bReloading = true;
    GetWorldTimerManager().SetTimer(ReloadTimer, this, &ASWeapon::Reload, ReloadTime);
}

void ASWeapon::StopReload()
{
    if (!bReloading)
        return;

    bReloading = false;
    if (!GetWorld())
        return;
    GetWorldTimerManager().ClearTimer(ReloadTimer);
}

void ASWeapon::OnRep_Reloading()
{
    if (!bReloading)
        return;
    GetWorldTimerManager().SetTimer(ReloadTimer, this, &ASWeapon::Reload, ReloadTime);
}

void ASWeapon::AddAmmo()
{
}

int32 ASWeapon::GetCurrentAmmo()
{
    return 0;
}

void ASWeapon::SetOwner(AActor* NewOwner)
{
    Character = Cast<ASCharacter>(NewOwner);
    Super::SetOwner(NewOwner);
    return;
    if (!Character)
        return;
    if (!Character->IsLocallyControlled())
        return;
    Character->GetController<ASPlayerController>()->OnFOVChanged.AddDynamic(this, &ASWeapon::OnFOVChangedHandle);
    OwnerWeaponMaterial = WeaponMesh->CreateDynamicMaterialInstance(0, WeaponMesh->GetMaterial(0));
    OwnerWeaponMaterial->SetScalarParameterValue(WeaponClippingParamName, 1.0f);
    OnFOVChangedHandle(Character->GetCamera()->FieldOfView);
}

void ASWeapon::ClientStartShooting()
{
}

void ASWeapon::ClientStopShooting()
{
}

void ASWeapon::BeginDestroy()
{
    Super::BeginDestroy();
    StopShooting();
    StopReload();
}

int32 ASWeapon::GetAmmoMag()
{
    return 0;
}

void ASWeapon::SetFOVWeaponMaterial(const float NewFOV)
{
    if (!Character->IsLocallyControlled())
        return;
    if (!OwnerWeaponMaterial)
        return;
    OwnerWeaponMaterial->SetScalarParameterValue(WeaponFOVParamName, NewFOV);
}

void ASWeapon::StartShooting()
{
    if (!CanShoot())
        return;

    MakeShoot();
}
