// Genderfield

#pragma once

#include "CoreMinimal.h"
#include "SGrenade.h"
#include "SGrenadeIncendiary.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER27_API ASGrenadeIncendiary : public ASGrenade
{
    GENERATED_BODY()

    UFUNCTION()
    void OnProjectileStop(const FHitResult& Hit);

protected:
    virtual void BeginPlay() override;
};
