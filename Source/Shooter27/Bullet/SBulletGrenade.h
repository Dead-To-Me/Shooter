// Shooter27

#pragma once

#include "CoreMinimal.h"
#include "SBulletBase.h"
#include "SBulletGrenade.generated.h"

class USoundCue;
class UNiagaraSystem;

/**
 * 
 */
UCLASS()
class SHOOTER27_API ASBulletGrenade : public ASBulletBase
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Bullet")
    float MaxDamage = 1;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Bullet")
    float MinDamage = 0;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Bullet")
    float DamageInnerRadius = 200;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Bullet")
    float DamageOuterRadius = 100;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Bullet")
    float DamageFalloff = 1;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Bullet")
    UNiagaraSystem* Explosion;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Bullet")
    USoundCue* ExplosionSound;

protected:
    virtual void BulletHitCosmetic(const FHitResult& Hit) override;
    virtual void BeginPlay() override;

private:
    UFUNCTION()
    void OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                FVector NormalImpulse, const FHitResult& Hit);
};
