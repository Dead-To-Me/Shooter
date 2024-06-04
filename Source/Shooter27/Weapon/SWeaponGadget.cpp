// Shooter27


#include "SWeaponGadget.h"

#include "Net/UnrealNetwork.h"


void ASWeaponGadget::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME_CONDITION(ASWeaponGadget, LeftUsesAmount, COND_OwnerOnly);
}

void ASWeaponGadget::StartShooting()
{
    if (!CanShoot())
    {
        StopShooting();
        return;
    }

    MakeShoot();
}

bool ASWeaponGadget::CanShoot() const
{
    return Super::CanShoot() && !ShootTimer.IsValid() && LeftUsesAmount > 0;
}

void ASWeaponGadget::AddAmmo()
{
    LeftUsesAmount = MaxUsesAmount;
}

int32 ASWeaponGadget::GetAmmoMag()
{
    return LeftUsesAmount;
}
