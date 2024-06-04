// Genderfield

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SAnimInstance.generated.h"

class UCurveVector;
class ASCharacter;
/**
 * 
 */
UCLASS()
class SHOOTER27_API USAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

    void SetAimTransform();
    void SetRelativeHandTransform();
    void SetLeftHandTransform();
    
    void FootPlacement();
    bool FootDisplacement(const FName& Socket, float& Distance);
    void SetFootDisplacement(FName& Socket, FRotator& NewRot, float& NewFootIK);
    float GetFoot(FName& Soket, FVector& Normal) const;

    bool SetFeetPlacement(FName& Bone, float& ZLoc);

    void InterpAim();
    void InterpAimWalk();
    FRotator LastUpdateRotation;

public:
    USAnimInstance();

    virtual void NativeBeginPlay() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    ASCharacter* Character;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Hands")
    FTransform RelativeHandTransform;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Hands")
    FTransform AimTransform;

    UPROPERTY(BlueprintReadWrite, Category="Hands")
    int32 AimLenght;

    UPROPERTY(BlueprintReadOnly, Category="Hands")
    FTransform LeftHandTransform;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SAnim|Foot")
    FName RHandSocket;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SAnim|Foot")
    FName LFootName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SAnim|Foot")
    FName RFootName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SAnim|Foot")
    FName RootBoneName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SAnim|Foot")
    float FootDelta;

    UPROPERTY(BlueprintReadOnly)
    bool bFalling;

    UPROPERTY(BlueprintReadOnly)
    float Displacment;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float FootInterpSpeed;

    UPROPERTY(BlueprintReadOnly)
    FRotator RightFootRotation;

    UPROPERTY(BlueprintReadOnly)
    FRotator LeftFootRotation;

    UPROPERTY(BlueprintReadOnly)
    float RightFootIK;

    UPROPERTY(BlueprintReadOnly)
    float LeftFootIK;

    UPROPERTY(BlueprintReadOnly)
    float Speed;

    UPROPERTY(BlueprintReadOnly)
    float MovementDirection;

    UPROPERTY(BlueprintReadOnly)
    bool bAiming;

    UPROPERTY(BlueprintReadOnly)
    float AimingAlpha;

    UPROPERTY(BlueprintReadOnly)
    bool bInterpAim;

    UPROPERTY(BlueprintReadOnly)
    bool bCrouch;

    UPROPERTY(BlueprintReadOnly)
    bool bLocal;

    UPROPERTY(BlueprintReadOnly)
    FVector LookAtPos;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FVector LocalHandModifier;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FVector ServerHandModifier;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float AimInterpSpeed;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float MaxWeaponSway;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float WeaponSwayInterpSpeed;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UCurveVector* AimWalkCurve;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float WalkInterpDuration;

    UPROPERTY(BlueprintReadOnly)
    float WalkInterpElapsed;

    UPROPERTY(BlueprintReadOnly)
    FVector LocalHandSway;

    UPROPERTY(EditDefaultsOnly)
    FName MoveRightName;

    UPROPERTY(EditDefaultsOnly)
    float MoveRightAngle;

    UPROPERTY(BlueprintReadOnly)
    float LeftFootZ;

    UPROPERTY(BlueprintReadOnly)
    float RightFootZ;

    UPROPERTY(BlueprintReadOnly)
    bool HitLeftFoot;

    UPROPERTY(BlueprintReadOnly)
    bool HitRightFoot;

    UPROPERTY(BlueprintReadOnly)
    float HipsZ;

    UPROPERTY(EditDefaultsOnly)
    float InterpSpeedHips;
};
