// Genderfield


#include "SAnimInstance.h"

#include "SCharacter.h"
#include "SWeapon.h"
#include "SWeaponManagerComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Curves/CurveVector.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

USAnimInstance::USAnimInstance()
{
    AimLenght = 25;
    bFalling = false;
    FootInterpSpeed = 15;
    RightFootRotation = FRotator::ZeroRotator;
    LeftFootRotation = FRotator::ZeroRotator;
    bAiming = false;
    AimingAlpha = 0;
    bInterpAim = false;
    AimInterpSpeed = 10;
    MaxWeaponSway = 7;
    WeaponSwayInterpSpeed = 10;
    bCrouch = false;
    bLocal = false;
    LookAtPos = FVector::ZeroVector;
    LocalHandModifier = FVector(25, 10, 25);
    ServerHandModifier = FVector(0, 0, 15);
    MoveRightName = "MoveRight";
    MoveRightAngle = 5;
    RHandSocket = "RightHand";
    LFootName = "LeftFoot";
    RFootName = "RightFoot";
    RootBoneName = "Hips";
    InterpSpeedHips = 5;
}

void USAnimInstance::SetAimTransform()
{
    if (!Character)
        return;
    const FTransform& CamTransform = Character->GetCamera()->GetComponentTransform();
    const FTransform& MeshTransform = Character->GetMesh()->GetComponentTransform();

    FTransform NewAimTransform = UKismetMathLibrary::MakeRelativeTransform(CamTransform, MeshTransform);
    NewAimTransform.SetLocation(
        NewAimTransform.GetLocation() + NewAimTransform.GetRotation().GetForwardVector() * AimLenght);
    if (Character->IsLocallyControlled())
    {
        float TurnAround = Character->GetInputAxisValue("TurnAround") * MaxWeaponSway;
        FRotator FinalRot = FRotator(TurnAround, Character->GetInputAxisValue("LookUp") * MaxWeaponSway, TurnAround);
        FRotator TargetRot = FRotator(NewAimTransform.Rotator().Pitch - FinalRot.Pitch,
                                      NewAimTransform.Rotator().Yaw + FinalRot.Yaw,
                                      NewAimTransform.Rotator().Roll + FinalRot.Roll);
        NewAimTransform.SetRotation(
            FMath::RInterpTo(NewAimTransform.Rotator(), TargetRot, GetDeltaSeconds(), WeaponSwayInterpSpeed).
            Quaternion());
    }

    AimTransform = NewAimTransform;
}

void USAnimInstance::SetRelativeHandTransform()
{
    if (!Character)
        return;
    if (!Character->GetWeaponManager())
        return;
    if (!Character->GetWeaponManager()->GetCurrentWeaponScope())
        return;
    FTransform LAimTransform = Character->GetWeaponManager()->GetCurrentWeaponScope()->GetSocketTransform("Aim");
    FTransform RHandTransform = Character->GetMesh()->GetSocketTransform(RHandSocket);
    RelativeHandTransform = UKismetMathLibrary::MakeRelativeTransform(LAimTransform, RHandTransform);
    float DeltaRoll = -Character->GetInputAxisValue(MoveRightName) * MoveRightAngle;
    RelativeHandTransform.ConcatenateRotation(FRotator(0, 0, DeltaRoll).Quaternion());
}

void USAnimInstance::SetLeftHandTransform()
{
    if (!Character)
        return;
    if (!Character->GetWeaponManager()->GetCurrentWeapon())
        return;
    FTransform GripTransform = Character->GetWeaponManager()->GetCurrentWeapon()->GetWeaponMesh()->
                                          GetSocketTransform("Grip");
    FTransform RHandTransform = Character->GetMesh()->GetSocketTransform(RHandSocket);
    LeftHandTransform = UKismetMathLibrary::MakeRelativeTransform(GripTransform, RHandTransform);
}

void USAnimInstance::InterpAim()
{
    if (FMath::IsNearlyEqual(AimingAlpha, bAiming))
    {
        bInterpAim = false;
        return;
    }
    AimingAlpha = UKismetMathLibrary::FInterpTo(AimingAlpha, bAiming, GetWorld()->GetDeltaSeconds(),
                                                AimInterpSpeed);
}

void USAnimInstance::InterpAimWalk()
{
    if (!bAiming)
    {
        LocalHandSway = FVector::ZeroVector;
        WalkInterpElapsed = 0;
        return;
    }
    LocalHandSway = AimWalkCurve && Character->GetVelocity().Length() > 0
                        ? AimWalkCurve->GetVectorValue(WalkInterpElapsed)
                        : FVector::ZeroVector; //TODO: Lerp To Zero
    WalkInterpElapsed = WalkInterpElapsed > WalkInterpDuration
                            ? 0
                            : WalkInterpElapsed += GetDeltaSeconds();
}

void USAnimInstance::FootPlacement()
{
    if (bFalling)
        return;
    FRotator NewLastUpdateRotation = Character->GetCharacterMovement()->GetLastUpdateRotation();
    bool IsTooFast = Character->GetVelocity().Length() > 20;
    bool NotRotating = UKismetMathLibrary::NormalizedDeltaRotator(NewLastUpdateRotation,
                                                                  LastUpdateRotation) != FRotator::ZeroRotator;
    if (!(IsTooFast || NotRotating))
        return;
    LastUpdateRotation = NewLastUpdateRotation;
    float RFootDistance = 0;
    float LFootDistance = 0;
    bool RFootHit = SetFeetPlacement(RFootName, RFootDistance);
    bool LFootHit = SetFeetPlacement(LFootName, LFootDistance); //FootDisplacement(LFootName, LFootDistance);
    if (!(RFootHit || LFootHit))
        return;
    float FootDistance = FMath::Max(RFootDistance, LFootDistance);
    /*
    FootDistance = FootDistance - Character->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
    FootDistance *= -1;
    */
    Displacment = UKismetMathLibrary::FInterpTo(Displacment, FootDistance, GetWorld()->GetDeltaSeconds(),
                                                FootInterpSpeed);
}

bool USAnimInstance::FootDisplacement(const FName& Socket, float& Distance)
{
    float StartZ = Character->GetMesh()->GetComponentLocation().Z + Character->GetCapsuleComponent()->
                                                                               GetUnscaledCapsuleHalfHeight();
    FVector StartLocation = Character->GetMesh()->GetSocketLocation(Socket);
    StartLocation.Z = StartZ;
    FVector EndLocation = StartLocation;
    EndLocation.Z -= 60.0f;
    FCollisionQueryParams Params;
    Character->GetIgnoreCollisionParams(Params);
    Params.bTraceComplex = false;
    FHitResult Hit;
    Hit.bStartPenetrating = true;
    GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECC_Visibility, Params);
    Distance = Hit.Distance;
    return Hit.bBlockingHit;
}

void USAnimInstance::SetFootDisplacement(FName& Socket, FRotator& NewRot, float& NewFootIK)
{
    FVector Normal;
    float Target = GetFoot(Socket, Normal);
    FRotator TargetRotator = FRotator(UKismetMathLibrary::DegAtan2(Normal.X, Normal.Z) * -1,
                                      0, UKismetMathLibrary::DegAtan2(Normal.Y, Normal.Z));
    NewRot = UKismetMathLibrary::RInterpTo(NewRot, TargetRotator, GetWorld()->GetDeltaSeconds(),
                                           FootInterpSpeed);
    NewFootIK = UKismetMathLibrary::FInterpTo(NewFootIK, Target, GetWorld()->GetDeltaSeconds(),
                                              FootInterpSpeed);
}


float USAnimInstance::GetFoot(FName& Soket, FVector& Normal) const
{
    FVector Start = Character->GetMesh()->GetSocketLocation(Soket);
    Start.Z = Character->GetMesh()->GetSocketLocation(RootBoneName).Z;
    FVector End = Start;
    Start.Z += 100;
    End.Z -= 100;
    FCollisionQueryParams Params;
    Character->GetIgnoreCollisionParams(Params);
    Params.bTraceComplex = false;
    FHitResult Hit;
    GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
    Normal = Hit.Normal;
    return Hit.bBlockingHit ? Hit.Distance : 999;
}

bool USAnimInstance::SetFeetPlacement(FName& Bone, float& ZLoc)
{
    FVector BoneLoc = Character->GetMesh()->GetSocketLocation(Bone);
    FVector StartLoc = FVector(BoneLoc.X, BoneLoc.Y, BoneLoc.Z + 50);
    FVector EndLoc = FVector(BoneLoc.X, BoneLoc.Y, BoneLoc.Z - 60);
    FCollisionQueryParams Params;
    Character->GetIgnoreCollisionParams(Params);
    Params.bTraceComplex = false;
    FHitResult Hit;
    GetWorld()->SweepSingleByChannel(Hit, StartLoc, EndLoc, FQuat::Identity, ECC_Visibility,
                                     FCollisionShape::MakeSphere(2), Params); // TODO: To Var
    //DrawDebugPoint(GetWorld(), Hit.Location, 12, FColor::Blue, false, 1);
    ZLoc = Hit.Distance - 50;
    return Hit.bBlockingHit;
}

void USAnimInstance::NativeBeginPlay()
{
    Super::NativeBeginPlay();
    Character = Cast<ASCharacter>(TryGetPawnOwner());
}

void USAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);
    if (!Character)
        return;
    bFalling = Character->GetMovementComponent() ? Character->GetMovementComponent()->IsFalling() : false;
    Speed = Character->GetVelocity().Length();
    // ReSharper disable once CppDeprecatedEntity
    MovementDirection = CalculateDirection(Character->GetVelocity(), Character->GetActorRotation());
    bLocal = Character->IsLocallyControlled();
    bCrouch = Character->GetCharacterMovement()->IsCrouching();

    LookAtPos = Character->GetActorLocation() + Character->GetBaseAimRotation().Vector() * 1000;
    if (bAiming != Character->IsAiming())
        bInterpAim = true;
    bAiming = Character->IsAiming();
    if (bInterpAim)
        InterpAim();
    if (bLocal)
        InterpAimWalk();
    SetAimTransform();
    SetRelativeHandTransform();
    SetLeftHandTransform();
    FootPlacement();
    //HitLeftFoot = SetFeetPlacement(LFootName, LeftFootZ);
    //HitRightFoot = SetFeetPlacement(RFootName, RightFootZ);
    //HipsZ = FMath::FInterpTo(HipsZ, FMath::Min(LeftFootZ, RightFootZ), GetDeltaSeconds(), InterpSpeedHips);
}
