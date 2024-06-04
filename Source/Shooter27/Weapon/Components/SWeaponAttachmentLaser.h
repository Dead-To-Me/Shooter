// Genderfield

#pragma once

#include "CoreMinimal.h"
#include "Components/SWeaponAttachmentComponent.h"
#include "SWeaponAttachmentLaser.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class SHOOTER27_API USWeaponAttachmentLaser : public USWeaponAttachmentComponent
{
    GENERATED_BODY()
    FTimerHandle DistanceCheckTimer;

    void CheckDistance();
protected:
    UPROPERTY(BlueprintReadOnly)
    UNiagaraComponent* LaserSystem;

    UPROPERTY(EditDefaultsOnly)
    UNiagaraSystem* NiagaraSystem;

    UPROPERTY(EditDefaultsOnly)
    FName LaserSocket = "laser";

    UPROPERTY(EditDefaultsOnly)
    float LaserCheckDistanceRate = 0.2;

protected:
    virtual void BeginPlay() override;
};
