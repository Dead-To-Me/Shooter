// Shooter27

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SGrenade.generated.h"

class UNiagaraSystem;
class UWidgetComponent;
class USoundCue;
class AFieldSystemActor;
class UProjectileMovementComponent;

UCLASS()
class SHOOTER27_API ASGrenade : public AActor
{
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadOnly)
    bool bExploded;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    UStaticMeshComponent* GrenadeMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    UProjectileMovementComponent* ProjectileMovement;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UWidgetComponent* GrenadeWidget;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grenade")
    FString GrenadeName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Grenade")
    AFieldSystemActor* ExplosionField;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Grenade")
    USoundCue* ExplosionSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Grenade")
    UNiagaraSystem* ExplosionSystem;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Grenade")
    FText Description;

public:
    // Sets default values for this actor's properties
    ASGrenade();

    UFUNCTION(BlueprintNativeEvent)
    void ExplodeGrenade();

    UFUNCTION()
    void AnyDamageHandle(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy,
                         AActor* DamageCauser);

    UProjectileMovementComponent* GetGrenadeProjectile() const { return ProjectileMovement; }
    UStaticMeshComponent* GetGrenadeMesh() const { return GrenadeMesh; }

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
};
