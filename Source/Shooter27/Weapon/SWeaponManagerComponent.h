// Shooter27

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SWeaponManagerComponent.generated.h"

class ASWeaponFirearm;
class ASWeapon;
class ASCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SHOOTER27_API USWeaponManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    USWeaponManagerComponent();
    void ScrollWeapon(float Axis);

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

private:
    void ClearGrenadeTimer();
    void ChangeWeaponFinish();
    bool SelectWeapon(ASWeapon* NewSelectedWeapon);

    template <class T>
    T* SpawnWeapon(TSubclassOf<ASWeapon> Weapon);

public:
    UFUNCTION()
    void StartFire();

    UFUNCTION(Server, Unreliable)
    void ServerStartFire();

    UFUNCTION()
    void StopFire();

    UFUNCTION(Server, Unreliable)
    void ServerStopFire();

    UFUNCTION(BlueprintAuthorityOnly)
    void SpawnWeapons();

    UFUNCTION(Server, Reliable)
    void ServerReload();

    UFUNCTION(Server, Reliable)
    void ServerChangeWeaponFiremode();

    UFUNCTION(Server, Reliable)
    void ThrowGrenade();

    UFUNCTION(Server, Reliable, BlueprintCallable)
    void ServerChangeWeapon(EInventorySlot Slot);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void SetMaxGrenade();

protected:
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Grenade")
    bool bThrowingGrenade;

    UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category="Grenade")
    int32 Grenades;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grenade")
    int32 MaxGrenades;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    FName WeaponSocket;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="RecoilLerp")
    float RecoilLerpDuration;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Grenade")
    float GrenadeCooldown;

    UPROPERTY(BlueprintReadOnly)
    bool bEnableBackRecoilLerp;

    UPROPERTY(EditDefaultsOnly)
    float BackRecoilSpeed;

private:
    UPROPERTY(VisibleInstanceOnly)
    ASCharacter* Character;

    UPROPERTY(Replicated, VisibleInstanceOnly)
    ASWeapon* CurrentWeapon;

    UPROPERTY(Replicated, VisibleInstanceOnly)
    ASWeaponFirearm* PrimaryWeapon;

    UPROPERTY(Replicated, VisibleInstanceOnly)
    ASWeaponFirearm* SecondaryWeapon;

    UPROPERTY(Replicated, VisibleInstanceOnly)
    ASWeapon* Gadget;

    UPROPERTY(Replicated, VisibleInstanceOnly)
    ASWeapon* Gadget2;

    float BackRecoilPitch;
    FTimerHandle GrenadeCooldownTimer;
    FTimerHandle ChangeWeaponTimer;
    float ChangeWeaponTime;
    bool bCanUseWeapon;

public:
    virtual void BeginDestroy() override;

    UFUNCTION(BlueprintPure)
    float GetSpread() const;

    UFUNCTION(BlueprintPure)
    bool IsWeaponShooting() const;

    UFUNCTION(BlueprintPure)
    const USceneComponent* GetCurrentWeaponScope() const;

    UFUNCTION(BlueprintPure)
    ASWeapon* GetCurrentWeapon() const { return CurrentWeapon; }

    template <class T>
    T* GetCurrentWeapon() const { return Cast<T>(CurrentWeapon); }

    UFUNCTION(BlueprintPure)
    bool IsThrowingGrenade() const { return bThrowingGrenade; }

    UFUNCTION(BlueprintCallable)
    void GetWeapons(UPARAM(ref) TArray<ASWeapon*>& Weapons);

    UFUNCTION(BlueprintCallable)
    void EnableBackRecoilLerp(const float EndPitch);

    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;
};
