// Genderfield

#pragma once

#include "CoreMinimal.h"
#include "SGrenade.h"
#include "SGrenadeExplode.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER27_API ASGrenadeExplode : public ASGrenade
{
    GENERATED_BODY()

private:
    FTimerHandle ExplodeGrenadeTimer;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GrenadeExplode")
    float MaxDamage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GrenadeExplode")
    float MinDamage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GrenadeExplode")
    float DamageInnerRadius;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GrenadeExplode")
    float DamageOuterRadius;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GrenadeExplode")
    float DamageFalloff;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GrenadeExplode")
    float TimeToExplode;

public:
    ASGrenadeExplode();

    UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable)
    void SetExplodeGrenadeTimer();

protected:
    virtual void BeginPlay() override;
};
