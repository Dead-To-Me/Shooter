// Shooter27

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SBulletBase.generated.h"

class AFieldSystemActor;
class UBoxComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;
class USoundAttenuation;

UCLASS()
class SHOOTER27_API ASBulletBase : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ASBulletBase();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Bullet")
    FDataTableRowHandle TableRowHandle;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    UBoxComponent* BoxCollision;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Bullet")
    UStaticMeshComponent* BulletMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    UProjectileMovementComponent* ProjectileMovement;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Bullet")
    float CrossSectionalArea;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Bullet")
    float DragCoefficient = 0.5;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Bullet")
    float Drag = 100000;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName DamageableTag = "Damageable";

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName Player = "Player";

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Bullet")
    TSubclassOf<AFieldSystemActor> FieldSystem;

    bool bTookDamage = false;

protected:
    UFUNCTION(BlueprintCosmetic)
    virtual void BulletHitCosmetic(const FHitResult& Hit);

    void TurnOffCollision();

public:
    FDataTableRowHandle& GetTableRowHandle() { return TableRowHandle; }
    virtual void Tick(float DeltaSeconds) override;
    UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
};
