// Shooter27

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeaponData.h"
#include "SWeapon.generated.h"

class ASPlayerController;
class ASCharacter;

UCLASS()
class SHOOTER27_API ASWeapon : public AActor
{
    GENERATED_BODY()

    UPROPERTY()
    UMaterialInstanceDynamic* OwnerWeaponMaterial;

public:
    // Sets default values for this actor's properties
    ASWeapon();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
    FString WeaponName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
    EInventorySlot InventorySlot;

protected:
    UPROPERTY(BlueprintReadOnly)
    ASCharacter* Character;

    UPROPERTY(BlueprintReadOnly)
    ASPlayerController* PlayerController;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Weapon")
    USkeletalMeshComponent* WeaponMesh;

    UPROPERTY(BlueprintReadWrite, Category="Shot")
    FTimerHandle ShootTimer;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Shot")
    float ShootRate = 1.0f;

    UPROPERTY(ReplicatedUsing=OnRep_Shooting, VisibleDefaultsOnly, BlueprintReadWrite, Category="Shot")
    bool IsShooting = false;

    UPROPERTY(BlueprintReadOnly, Category="Reload")
    FTimerHandle ReloadTimer;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Reload")
    float ReloadTime = 1.0f;

    UPROPERTY(ReplicatedUsing = OnRep_Reloading, BlueprintReadOnly, Category="Reload")
    bool bReloading = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Material")
    FName WeaponClippingParamName = "UseWeaponClipping";

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Material")
    FName WeaponFOVParamName = "FOV";

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
    UTexture2D* WeaponTexture;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnFOVChangedHandle(float NewFOV);

public:
    virtual void SetOwner(AActor* NewOwner) override;
    virtual void ClientStartShooting();
    virtual void ClientStopShooting();
    virtual void BeginDestroy() override;

    UFUNCTION()
    virtual void OnRep_Shooting();

    UFUNCTION(BlueprintAuthorityOnly, BlueprintNativeEvent)
    void MakeShoot();

    UFUNCTION(BlueprintAuthorityOnly)
    virtual void StartShooting();

    UFUNCTION(Server, Reliable, BlueprintAuthorityOnly)
    virtual void StopShooting();

    UFUNCTION(BlueprintPure)
    virtual bool CanShoot() const;

    UFUNCTION(BlueprintPure)
    virtual bool CanReload() const;

    UFUNCTION(BlueprintAuthorityOnly)
    virtual void Reload();

    UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable)
    virtual void StartReload();

    UFUNCTION(BlueprintAuthorityOnly)
    void StopReload();

    UFUNCTION()
    virtual void OnRep_Reloading();

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    virtual void AddAmmo();

    UFUNCTION(BlueprintPure)
    virtual int32 GetCurrentAmmo();

    UFUNCTION(BlueprintPure)
    virtual int32 GetAmmoMag();

    UFUNCTION(BlueprintCallable, BlueprintCosmetic)
    virtual void SetFOVWeaponMaterial(const float NewFOV);

    bool GetIsShooting() const { return IsShooting; }
    USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
};
