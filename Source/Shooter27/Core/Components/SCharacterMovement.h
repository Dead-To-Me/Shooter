// Genderfield

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SCharacterMovement.generated.h"

UENUM()
enum ECustomMovementMode
{
    CMOVE_None,
    CMOVE_Slide,
    CMOVE_MAX
};

/**
 * 
 */
UCLASS()
class SHOOTER27_API USCharacterMovement : public UCharacterMovementComponent
{
    GENERATED_BODY()

    UPROPERTY(Transient) // TODO:
    ASCharacter* SCharacterOwner;

    class FSavedMove_SCharacter : public FSavedMove_Character
    {
        uint8 Saved_bPrevWantsToCrouch:1;

    public:
        FSavedMove_SCharacter();

        virtual void Clear() override;
        virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel,
                                FNetworkPredictionData_Client_Character& ClientData) override;
        virtual void PrepMoveFor(ACharacter* C) override;
    };

public:
    bool Safe_bPrevWantsToCrouch;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character Movement: Walking")
    float MaxSprintSpeed;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character Movement: Walking")
    float MaxCrouchSprintSpeed;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Character Movement: Walking")
    bool bWantToSprint;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character Movement: Sliding")
    float SlideMinSpeed = 350;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character Movement: Sliding")
    float SlideMaxSpeed = 1000;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character Movement: Sliding")
    float SlideImpulse = 500;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character Movement: Sliding")
    float SlideGravityForce = 5000;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character Movement: Sliding")
    float SlideFriction = 1.3f;

public:
    USCharacterMovement();
    float CalculateDirection() const;
    virtual float GetMaxSpeed() const override;
    bool GetWantToSprint() const { return bWantToSprint; }
    void SetWantToSprint(bool NewState) { bWantToSprint = NewState; }
    virtual bool IsMovingOnGround() const override;
    virtual bool CanCrouchInCurrentState() const override;

    UFUNCTION(BlueprintPure)
    bool CanSprint() const;

    UFUNCTION(BlueprintPure)
    bool IsCustomMovementMove(ECustomMovementMode InCustomMovementMode) const;

protected:
    virtual void InitializeComponent() override;
    virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
    virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
    virtual void UpdateCharacterStateAfterMovement(float DeltaSeconds) override;
    virtual void PhysCustom(float deltaTime, int32 Iterations) override;
    virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

private:
    bool CanSlide() const;
    void EnterSlide();
    void EndSlide();
    void PhysSlide(float DeltaTime, int32 Iterations);
};
