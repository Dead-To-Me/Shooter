// Shooter27

#pragma once

#include "CoreMinimal.h"
#include "SBulletBase.h"
#include "SBulletFirearm.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER27_API ASBulletFirearm : public ASBulletBase
{
    GENERATED_BODY()

public:
    ASBulletFirearm();

private:
    FVector SpawnLocation;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Bullet")
    float HeadDamageMode;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Bullet")
    float BodyDamageMode;

private:
    UFUNCTION()
    void OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                FVector NormalImpulse, const FHitResult& Hit);

protected:
    virtual void BulletHitCosmetic(const FHitResult& Hit) override;
    virtual void BeginPlay() override;
};
