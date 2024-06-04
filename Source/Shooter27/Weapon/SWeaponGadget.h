// Shooter27

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"
#include "SWeaponGadget.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER27_API ASWeaponGadget : public ASWeapon
{
    GENERATED_BODY()

protected:
    UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite)
    int32 LeftUsesAmount = 1;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    int32 MaxUsesAmount = 1;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    FText Description;

public:
    virtual void StartShooting() override;
    virtual bool CanShoot() const override;
    virtual void AddAmmo() override;
    virtual int32 GetAmmoMag() override;
};
