// Shooter27


#include "SWeaponManagerComponent.h"

#include "SCharacter.h"
#include "SGrenade.h"
#include "SPlayerState.h"
#include "SWeapon.h"
#include "SWeaponFirearm.h"
#include "Components/SCharacterMovement.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

void USWeaponManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    // TODO: Cannot Swap Weapon After Spawn
    DOREPLIFETIME(USWeaponManagerComponent, CurrentWeapon);
    DOREPLIFETIME_CONDITION(USWeaponManagerComponent, PrimaryWeapon, COND_InitialOnly);
    DOREPLIFETIME_CONDITION(USWeaponManagerComponent, SecondaryWeapon, COND_InitialOnly);
    DOREPLIFETIME_CONDITION(USWeaponManagerComponent, Grenades, COND_InitialOnly);
    DOREPLIFETIME_CONDITION(USWeaponManagerComponent, Gadget, COND_InitialOnly);
    DOREPLIFETIME_CONDITION(USWeaponManagerComponent, Gadget2, COND_InitialOnly);
}

// Sets default values for this component's properties
USWeaponManagerComponent::USWeaponManagerComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;
    SetIsReplicatedByDefault(true);
    bThrowingGrenade = false;
    Grenades = 2;
    MaxGrenades = 2;
    RecoilLerpDuration = 0.3f;
    GrenadeCooldown = 1.5f;
    ChangeWeaponTime = 0.8f;
    BackRecoilSpeed = 2;
    bCanUseWeapon = true;
    WeaponSocket = "RightWeaponSocket";
}

void USWeaponManagerComponent::ScrollWeapon(float Axis)
{
    if (FMath::IsNearlyZero(Axis))
        return;
    if (!CurrentWeapon)
        return;
    if (Character->IsLeftMouseButtonPressed())
        return;

    int32 NewSlot = FMath::TruncToInt32(Axis) + static_cast<int32>(CurrentWeapon->
        InventorySlot);
    if (!UKismetMathLibrary::InRange_IntInt(NewSlot, 0,
                                            static_cast<int32>(EInventorySlot::Gadget)))
    {
        NewSlot = NewSlot < 0
                      ? static_cast<int32>(EInventorySlot::Gadget)
                      : static_cast<int32>(EInventorySlot::Primary);
    }
    Character->PlayChangeWeaponMontage();
    ServerChangeWeapon(static_cast<EInventorySlot>(NewSlot));
}


// Called when the game starts
void USWeaponManagerComponent::BeginPlay()
{
    Super::BeginPlay();
    if (!GetOwner())
        return;
    Character = Cast<ASCharacter>(GetOwner());
}

void USWeaponManagerComponent::ClearGrenadeTimer()
{
    GetWorld()->GetTimerManager().ClearTimer(GrenadeCooldownTimer);
}

void USWeaponManagerComponent::ChangeWeaponFinish()
{
    bCanUseWeapon = true;
    GetWorld()->GetTimerManager().ClearTimer(ChangeWeaponTimer);
}

bool USWeaponManagerComponent::SelectWeapon(ASWeapon* NewSelectedWeapon)
{
    if (!NewSelectedWeapon)
        return false;

    if (CurrentWeapon)
        CurrentWeapon->SetActorHiddenInGame(true);

    CurrentWeapon = NewSelectedWeapon;
    CurrentWeapon->SetActorHiddenInGame(false);
    return true;
}

void USWeaponManagerComponent::StartFire()
{
    ServerStartFire();
    if (Character->GetCurrentMontage())
        return;
    if (!CurrentWeapon)
        return;
    CurrentWeapon->ClientStartShooting();
}

void USWeaponManagerComponent::ServerStartFire_Implementation()
{
    if (!bCanUseWeapon)
        return;
    Character->SetLeftMouseButtonPressed(true);
    if (!CurrentWeapon)
        return;
    CurrentWeapon->StartShooting();
}

void USWeaponManagerComponent::StopFire()
{
    ServerStopFire();
    if (!CurrentWeapon)
        return;
    CurrentWeapon->ClientStopShooting();
}

void USWeaponManagerComponent::ServerStopFire_Implementation()
{
    Character->SetLeftMouseButtonPressed(false);
}

void USWeaponManagerComponent::BeginDestroy()
{
    Super::BeginDestroy();
    // TODO: SetLifeSpan ~30s For PickUp
    if (PrimaryWeapon)
        PrimaryWeapon->Destroy();
    if (SecondaryWeapon)
        SecondaryWeapon->Destroy();
    if (Gadget)
        Gadget->Destroy();
    if (Gadget2)
        Gadget2->Destroy();
}

float USWeaponManagerComponent::GetSpread() const
{
    if (!CurrentWeapon)
        return 3;
    ASWeaponFirearm* Weapon = Cast<ASWeaponFirearm>(CurrentWeapon);
    if (!Weapon)
        return 3;
    return Weapon->GetCurrentSpread();
}

bool USWeaponManagerComponent::IsWeaponShooting() const
{
    return CurrentWeapon && CurrentWeapon->GetIsShooting();
}

const USceneComponent* USWeaponManagerComponent::GetCurrentWeaponScope() const
{
    return CurrentWeapon && CurrentWeapon->IsA(ASWeaponFirearm::StaticClass())
               ? Cast<ASWeaponFirearm>(CurrentWeapon)->GetScopeMesh()
               : nullptr;
}

void USWeaponManagerComponent::GetWeapons(UPARAM(ref) TArray<ASWeapon*>& Weapons)
{
    Weapons.Add(PrimaryWeapon);
    Weapons.Add(SecondaryWeapon);
    Weapons.Add(Gadget);
    Weapons.Add(Gadget2);
}

void USWeaponManagerComponent::EnableBackRecoilLerp(const float EndPitch)
{
    BackRecoilPitch = EndPitch;
    bEnableBackRecoilLerp = true;
}

void USWeaponManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!Character)
        return;
    if (!Character->IsLocallyControlled())
    {
        SetComponentTickEnabled(false);
        return;
    }

    if (!bEnableBackRecoilLerp)
        return;
    if (BackRecoilPitch >= Character->GetBaseAimRotation().Pitch || FMath::IsNearlyEqual(
        BackRecoilPitch, Character->GetBaseAimRotation().Pitch, 0.01) || Character->IsMouseMoving())
    {
        bEnableBackRecoilLerp = false;
        BackRecoilPitch = 0;
        return;
    }
    // TODO: if too close Current Recoil - StartRecoil = BackRecoilSpeed;

    Character->AddControllerPitchInput(DeltaTime * BackRecoilSpeed);
}

void USWeaponManagerComponent::ServerChangeWeaponFiremode_Implementation()
{
    if (!CurrentWeapon)
        return;
    if (!CurrentWeapon->IsA(ASWeaponFirearm::StaticClass()))
        return;
    Cast<ASWeaponFirearm>(CurrentWeapon)->ChangeFiremode();
}

void USWeaponManagerComponent::ServerReload_Implementation()
{
    if (!CurrentWeapon)
        return;
    CurrentWeapon->StartReload();
}

void USWeaponManagerComponent::SpawnWeapons()
{
    const FWeaponAttachments* WeaponAttachments;

    const ASPlayerState* PlayerState = Character->GetPlayerState<ASPlayerState>();
    const auto& Inventory = PlayerState->GetInventorySlots();

    if (Inventory.PrimaryWeapon.GetDefaultObject())
    {
        PrimaryWeapon = SpawnWeapon<ASWeaponFirearm>(Inventory.PrimaryWeapon);
        WeaponAttachments = PlayerState->GetWeaponAttachments().Find(Inventory.PrimaryWeapon);
        PrimaryWeapon->SetupWeaponAttachments(WeaponAttachments);
    }

    if (Inventory.SecondaryWeapon.GetDefaultObject())
    {
        SecondaryWeapon = SpawnWeapon<ASWeaponFirearm>(Inventory.SecondaryWeapon);
        WeaponAttachments = PlayerState->GetWeaponAttachments().Find(Inventory.SecondaryWeapon);
        SecondaryWeapon->SetupWeaponAttachments(WeaponAttachments);
    }

    // TODO: SpawnWeapon(Inventory.Melee);

    Gadget = SpawnWeapon<ASWeapon>(Inventory.Gadget);
    if (Gadget && Gadget->IsA(ASWeaponFirearm::StaticClass()))
    {
        WeaponAttachments = PlayerState->GetWeaponAttachments().Find(Gadget->GetClass());
        Cast<ASWeaponFirearm>(Gadget)->SetupWeaponAttachments(WeaponAttachments);
    }
    Gadget2 = SpawnWeapon<ASWeapon>(Inventory.Gadget2);

    if (CurrentWeapon)
        CurrentWeapon->SetActorHiddenInGame(false);
}

void USWeaponManagerComponent::SetMaxGrenade()
{
    Grenades = MaxGrenades;
}

void USWeaponManagerComponent::ServerChangeWeapon_Implementation(EInventorySlot Slot)
{
    if (CurrentWeapon)
    {
        if (CurrentWeapon->InventorySlot == Slot)
            return;
        CurrentWeapon->StopReload();
    }

    switch (Slot)
    {
    case EInventorySlot::Primary:
        SelectWeapon(PrimaryWeapon);
        break;
    case EInventorySlot::Secondary:
        SelectWeapon(SecondaryWeapon);
        break;
    case EInventorySlot::Gadget:
        SelectWeapon(Gadget);
        break;
    case EInventorySlot::Gadget2:
        SelectWeapon(Gadget2);
        break;
    }
    Character->PlayChangeWeaponMontage();
    bCanUseWeapon = false;
    GetWorld()->GetTimerManager().SetTimer(ChangeWeaponTimer, this, &USWeaponManagerComponent::ChangeWeaponFinish,
                                           ChangeWeaponTime);
}


void USWeaponManagerComponent::ThrowGrenade_Implementation()
{
    if (GrenadeCooldownTimer.IsValid())
        return;

    if (Grenades < 1)
        return;

    auto PlayerState = Character->GetPlayerState<ASPlayerState>();
    if (!PlayerState->GetInventorySlots().Grenade->IsValidLowLevel())
        return;

    FVector SpawnLocation = Character->GetMesh()->GetSocketLocation("head") + Character->GetViewRotation().Vector() *
        80;
    ASGrenade* Grenade = GetWorld()->SpawnActor<ASGrenade>(PlayerState->GetInventorySlots().Grenade, SpawnLocation,
                                                           Character->GetViewRotation());
    Grenade->SetInstigator(Character);
    Grenades--;
    GetWorld()->GetTimerManager().SetTimer(GrenadeCooldownTimer, this, &USWeaponManagerComponent::ClearGrenadeTimer,
                                           GrenadeCooldown, false);
}

template <class T>
T* USWeaponManagerComponent::SpawnWeapon(TSubclassOf<ASWeapon> Weapon)
{
    if (!Weapon.GetDefaultObject())
        return nullptr;
    ASWeapon* SpawnedWeapon = GetWorld()->SpawnActor<ASWeapon>(Weapon);
    SpawnedWeapon->SetOwner(Character);
    SpawnedWeapon->AttachToComponent(Character->GetMeshTrue(),
                                     FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
                                     WeaponSocket);
    SpawnedWeapon->SetActorHiddenInGame(true);
    if (!CurrentWeapon)
        CurrentWeapon = SpawnedWeapon;
    return Cast<T>(SpawnedWeapon);
}
