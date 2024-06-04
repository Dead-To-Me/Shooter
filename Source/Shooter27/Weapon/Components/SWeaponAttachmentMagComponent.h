// Shooter27

#pragma once

#include "CoreMinimal.h"
#include "Components/SWeaponAttachmentComponent.h"
#include "SWeaponAttachmentMagComponent.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER27_API USWeaponAttachmentMagComponent : public USWeaponAttachmentComponent
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(NoResetToDefault, ClampMin=1, UIMin=1))
    int32 MaxAmmoInMagazine = 1;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(NoResetToDefault, ClampMin=1, UIMin=1))
    int32 MaxAmmo = 1;

public:
    int32 GetMaxAmmoInMagazine() const { return MaxAmmoInMagazine; }
    int32 GetMaxAmmo() const { return MaxAmmo; }
};
