// Shooter27

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"
#include "SWeaponFirearm.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class USWeaponAttachmentMagComponent;
class USWeaponAttachmentScope;
class USoundCue;
class ASBulletBase;
class USceneCaptureComponent2D;


USTRUCT(BlueprintType)
struct FRecoil
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float PitchMax = 0;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float PitchMin = 0;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    float PitchMultiplier = 1;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float YawMax = 0;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float YawMin = 0;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    float YawMultiplier = 1;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    float StartRecoilPosPitch = 0;
};

UENUM(BlueprintType)
enum class EFiremode : uint8 // TODO: Change Name in Widget Blueprint
{
    SemiAutomatic UMETA(DisplayName="SEMI"),
    Automatic UMETA(DisplayName="AUTO")
};


/**
 * 
 */
UCLASS()
class SHOOTER27_API ASWeaponFirearm : public ASWeapon
{
    GENERATED_BODY()

protected:
    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Attachment")
    USWeaponAttachmentScope* Scope;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Attachment")
    UStaticMeshComponent* MuzzleMesh;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Attachment")
    USWeaponAttachmentMagComponent* Mag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USceneCaptureComponent2D* SceneCapture;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Sockets")
    FName MuzzleSocket = "Muzzle";

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Sockets")
    FName ScopeSocket = "Scope";

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Sockets")
    FName MagazineSocket = "Mag";

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Sockets")
    FName LaserSocket = "laser";

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
    EWeaponType WeaponType;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Attachment")
    TArray<EWeaponAttachmentSlot> AvailableAttachments;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Attachment")
    TArray<TSubclassOf<USWeaponAttachmentScope>> AvailableScopes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attachment")
    TArray<TSubclassOf<USWeaponAttachmentMagComponent>> AvailableMags;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attachment")
    TArray<TSubclassOf<USWeaponAttachmentLaser>> AvailableLasers;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attachment")
    FWeaponAttachments Attachments;

    // UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attachment")
    // AvailableMuzzles; TODO:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attachment")
    bool bHaveBipod;

    UPROPERTY(ReplicatedUsing=OnRep_Bipod, BlueprintReadWrite, Category="Attachment")
    bool bBipod;

    UPROPERTY(ReplicatedUsing=OnRep_Firemode, EditDefaultsOnly, BlueprintReadOnly, Category="Shot")
    EFiremode Firemode;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Shot")
    bool bCanChangeFiremode;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Shot")
    UCurveFloat* DamageCurve;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Spread")
    float CurrentBulletSpread;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spread")
    float AimingSpreadMultiplier;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spread")
    float CrouchSpreadMultiplier;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spread")
    float FallingSpreadMultiplier;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spread")
    float StandSpreadInc;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spread")
    float StandSpreadMax;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spread")
    float MoveSpreadInc;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spread")
    float MoveSpreadMax;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Shot")
    bool bFirstShot;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Shot")
    int32 BulletOffset;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Shot")
    UNiagaraSystem* ShotEffect;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Shot")
    FRecoil Recoil;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sound")
    USoundCue* ShotSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sound")
    USoundCue* DryFireSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sound")
    USoundCue* ReloadEndSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sound")
    USoundCue* ChangeFireModeSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aim")
    bool bCanAiming;

    UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category="Ammo")
    int32 CurrentAmmoMagazine;

    UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category="Ammo")
    int32 CurrentAmmo;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Anim")
    UAnimMontage* ShotMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Anim")
    UAnimMontage* ReloadMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Reload")
    float ReloadTimeLeft;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Reload")
    float ReloadLeftThreshold;

    FTimerHandle RecoilTimer;

private:
    UFUNCTION()
    void OnRep_Firemode();

protected:
    UFUNCTION(BlueprintImplementableEvent)
    void OnRep_Bipod();

public:
    ASWeaponFirearm();
    virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;
    virtual void StartShooting() override;
    virtual void MakeShoot_Implementation() override;
    virtual bool CanReload() const override;
    virtual bool CanShoot() const override;
    virtual void Reload() override;
    virtual void StopShooting_Implementation() override;
    virtual void OnRep_Reloading() override;
    virtual void OnRep_Shooting() override;
    virtual void AddAmmo() override;
    virtual void ClientStartShooting() override;
    virtual void ClientStopShooting() override;
    virtual void StartReload() override;
    void ClientMakeShot();
    void StartRecoil();
    void StopRecoil();
    void AddRecoil();
    void SetupWeaponAttachments(const FWeaponAttachments* WeaponAttachments);
    void ChangeFiremode();
    void PlayShotSound();


    UFUNCTION(BlueprintPure)
    UCurveFloat* GetDamageCurve() const { return DamageCurve; }

    UFUNCTION(BlueprintPure)
    const USceneComponent* GetScopeMesh() const;

    UFUNCTION(BlueprintPure)
    UStaticMeshComponent* GetMuzzleMesh() const { return MuzzleMesh; }

    UFUNCTION(BlueprintPure)
    const USceneComponent* GetMagMesh() const;

    UFUNCTION(BlueprintPure)
    void GetCalculatedRecoil(FVector2D& CalculatedRecoil);

    const FWeaponAttachments& GetAttachments() const { return Attachments; }
    virtual int32 GetCurrentAmmo() override { return CurrentAmmo; }
    virtual int32 GetAmmoMag() override { return CurrentAmmoMagazine; }
    bool CanAim() const { return bCanAiming; }
    USceneCaptureComponent2D* GetSceneCapture() const { return SceneCapture; }
    float GetCurrentSpread() const;

private:
    UPROPERTY()
    UNiagaraComponent* MuzzleFlesh;

    UPROPERTY()
    FTimerHandle ShotSoundTimer;
};

