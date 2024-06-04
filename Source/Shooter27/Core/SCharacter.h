// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SInteractInterface.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"


class ASPlayerController;
class USCharacterMovement;
class UPaperSprite;
class ASGrenade;
class USphereComponent;
class USAnimInstance;
class USpringArmComponent;
class USHeathComponent;
class USWeaponManagerComponent;
class UCameraComponent;
class ASWeapon;
class UWidgetComponent;
class UPaperSpriteComponent;

UCLASS()
class SHOOTER27_API ASCharacter : public ACharacter, public ISInteractInterface
{
    GENERATED_BODY()

    UPROPERTY()
    ASPlayerController* SPlayerController;

public:
    // Sets default values for this character's properties
    ASCharacter(const FObjectInitializer& ObjectInitializer);

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<ASWeapon> WeaponClass;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    USHeathComponent* HeathComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    UCameraComponent* Camera;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    USWeaponManagerComponent* WeaponManager;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USkeletalMeshComponent* OwnerMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UWidgetComponent* PlayerBarWidget;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Interact")
    USphereComponent* InteractSphere;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Map")
    USceneCaptureComponent2D* MapCapture;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Map")
    UPaperSpriteComponent* PlayerIcon;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USAnimInstance* AnimInstance;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
    bool bLeftMouseButtonPressed;

    UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, Category="Aiming")
    bool bAiming;

    UPROPERTY(EditDefaultsOnly)
    float LifeSpanAfterDead;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Aiming")
    float FieldOfViewScope;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Aiming")
    float FieldOfView;

    UPROPERTY(EditDefaultsOnly)
    float ViewCheckRate;

    UPROPERTY(EditDefaultsOnly)
    float ViewCheckRadius;

    UPROPERTY(EditDefaultsOnly)
    float ViewDistanceCheck;

    UPROPERTY(EditDefaultsOnly, Category="Interact")
    float InteractDistance;

    UPROPERTY(EditDefaultsOnly, Category="Grenade")
    float GrenadeCheckRadius;

    UPROPERTY(EditDefaultsOnly, Category="Map")
    UPaperSprite* DeathSprite;

    UPROPERTY(ReplicatedUsing=OnRepDetected, EditDefaultsOnly, BlueprintReadOnly, Category="Map")
    bool bDetected;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Map")
    float DetectedFadeOutTime;

    UPROPERTY(ReplicatedUsing=OnRep_IsSprinting, BlueprintReadOnly, VisibleInstanceOnly)
    bool bSprinting;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FVector2D LandVelocity;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float FallDamageMultiplier;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UAnimMontage* ChangeWeaponMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName HeadBoneName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float HeadStabilizationSpeed;

private:
    FTimerHandle ViewCheckTimer;
    FTimerHandle DetectedFadeOutTimer;
    float CurrentRelativeCameraZ;
    float CurrentWorldCameraZ;

    UPROPERTY()
    USCharacterMovement* SCharacterMovement;

private:
    void MoveForward(float Value);
    void MoveRight(float Value);
    void LookUp(float Value);
    void TurnAround(float Value);
    void CheckIfViewTarget() const;
    void SelectPrimaryWeapon();
    void SelectSecondaryWeapon();
    void SelectGadget();
    void SelectSecondGadget();
    void CheckInteract();
    void ClientCheckInteract();
    void BeginSprinting();
    void EndSprinting();
    void GrenadeThrowBack();
    void DetectedFadeOut();
    void CrouchToggle();

private:
    UFUNCTION(Reliable, Server)
    void ServerGrenadeThrowBack(ASGrenade* Grenade);

    UFUNCTION(Reliable, Server, BlueprintCallable)
    void ServerRedeployment();

    UFUNCTION()
    void OnBeginOverlapSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool
                              bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnEndOverlapSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnRep_IsSprinting();

    UFUNCTION(BlueprintNativeEvent)
    void OnRepDetected();

public:
    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void BecomeViewTargetInterface_Implementation() override;
    virtual void Landed(const FHitResult& Hit) override;
    void BeginAiming();
    void StopAiming();
    virtual bool CanCrouch() const override;
    void WantToCrouch();
    void EndCrouch();
    virtual void PossessedBy(AController* NewController) override;
    virtual void UnPossessed() override;
    UCameraComponent* GetCamera() const { return Camera; }
    bool IsMouseMoving() const;
    bool IsSprinting() const { return bSprinting; }
    void SetPitch(const float NewPitch) const;
    void SetYaw(const float NewYaw) const;
    void SetSprinting(bool NewState);
    bool IsWantToSprint() const;
    USCharacterMovement* GetSCharacterMovement() const { return SCharacterMovement; }
    void PlayChangeWeaponMontage();
    virtual void OutsideWorldBounds() override;
    virtual void OnRep_Owner() override;
    FCollisionQueryParams& GetIgnoreCollisionParams(FCollisionQueryParams& Params) const;
    virtual void Jump() override;
    virtual void StopJumping() override;
    USkeletalMeshComponent* GetMeshTrue()const;

    UFUNCTION(BlueprintNativeEvent)
    void AddRecoil(const FVector2D& Recoil);

    UFUNCTION(BlueprintPure)
    USWeaponManagerComponent* GetWeaponManager() const { return WeaponManager; }

    UFUNCTION(BlueprintPure)
    bool IsLeftMouseButtonPressed() const { return bLeftMouseButtonPressed; }

    UFUNCTION(BlueprintCallable)
    bool SetLeftMouseButtonPressed(const bool NewState) { return bLeftMouseButtonPressed = NewState; }

    UFUNCTION(BlueprintPure)
    bool IsAiming() const { return bAiming; }

    UFUNCTION(Client, Unreliable, BlueprintCallable)
    void OnPlayerDamageHandle(const FVector& Direction);

    UFUNCTION(Server, Reliable)
    void ServerCheckInteract();

private:
    UFUNCTION(Server, Unreliable)
    void ServerBeginAiming();

    UFUNCTION(Server, Unreliable)
    void ServerStopAiming();

    UFUNCTION(NetMulticast, Unreliable)
    void SetCharacterRagdoll();

    UFUNCTION(Server, Reliable)
    void ServerBeginSprint();

    UFUNCTION(Server, Reliable)
    void ServerEndSprint();

    UFUNCTION()
    void OnTakePointDamageHandle(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation,
                                 UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection,
                                 const UDamageType* DamageType, AActor* DamageCauser);

    UFUNCTION()
    void OnTakeRadialDamageHandle(AActor* DamagedActor, float Damage, const UDamageType*
                                  DamageType, FVector Origin, const FHitResult& HitInfo, AController*
                                  InstigatedBy, AActor* DamageCauser);
};
