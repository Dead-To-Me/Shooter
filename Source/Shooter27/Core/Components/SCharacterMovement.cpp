// Genderfield


#include "Components/SCharacterMovement.h"
#include "SCharacter.h"
#include "SWeaponManagerComponent.h"
#include "Components/CapsuleComponent.h"

USCharacterMovement::FSavedMove_SCharacter::FSavedMove_SCharacter()
{
    Saved_bPrevWantsToCrouch = 0;
}

void USCharacterMovement::FSavedMove_SCharacter::Clear()
{
    FSavedMove_Character::Clear();
    Saved_bPrevWantsToCrouch = 0;
}

void USCharacterMovement::FSavedMove_SCharacter::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel,
                                                            FNetworkPredictionData_Client_Character& ClientData)
{
    FSavedMove_Character::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);
    return;
    const USCharacterMovement* CharacterMovement = Cast<USCharacterMovement>(C->GetCharacterMovement());
    Saved_bPrevWantsToCrouch = CharacterMovement->Safe_bPrevWantsToCrouch;
}

void USCharacterMovement::FSavedMove_SCharacter::PrepMoveFor(ACharacter* C)
{
    FSavedMove_Character::PrepMoveFor(C);
    return;
    USCharacterMovement* CharacterMovement = Cast<USCharacterMovement>(C->GetCharacterMovement());

    CharacterMovement->Safe_bPrevWantsToCrouch = Saved_bPrevWantsToCrouch;
}

USCharacterMovement::USCharacterMovement()
{
    MaxWalkSpeed = 350.f;
    MaxWalkSpeedCrouched = 200.f;
    MaxSprintSpeed = 625.f;
    MaxCrouchSprintSpeed = 500.f;
}

float USCharacterMovement::CalculateDirection() const
{
    if (!Velocity.IsNearlyZero())
    {
        const FMatrix RotMatrix = FRotationMatrix(CharacterOwner->GetBaseAimRotation());
        const FVector ForwardVector = RotMatrix.GetScaledAxis(EAxis::X);
        const FVector RightVector = RotMatrix.GetScaledAxis(EAxis::Y);
        const FVector NormalizedVel = Velocity.GetSafeNormal2D();

        // get a cos(alpha) of forward vector vs velocity
        const float ForwardCosAngle = static_cast<float>(FVector::DotProduct(ForwardVector, NormalizedVel));
        // now get the alpha and convert to degree
        float ForwardDeltaDegree = FMath::RadiansToDegrees(FMath::Acos(ForwardCosAngle));

        // depending on where right vector is, flip it
        const float RightCosAngle = static_cast<float>(FVector::DotProduct(RightVector, NormalizedVel));
        if (RightCosAngle < 0.f)
        {
            ForwardDeltaDegree *= -1.f;
        }

        return ForwardDeltaDegree;
    }

    return 0.f;
}


float USCharacterMovement::GetMaxSpeed() const
{
    switch (MovementMode)
    {
    case MOVE_Walking: // Passthrough 
    case MOVE_NavWalking:
    case MOVE_Falling:
        return bWantToSprint && CanSprint()
                   ? IsCrouching()
                         ? MaxCrouchSprintSpeed
                         : MaxSprintSpeed
                   : IsCrouching()
                   ? MaxWalkSpeedCrouched
                   : MaxWalkSpeed;
    case MOVE_Custom:
        return SlideMaxSpeed;
    default:
        return 0.f;
    }
}

bool USCharacterMovement::CanSprint() const
{
    float Direction = CalculateDirection();
    return SCharacterOwner && !FMath::IsNearlyZero(CharacterOwner->GetVelocity().Length()) && -60 < Direction &&
        Direction < 60 && !SCharacterOwner->IsAiming() && MovementMode != MOVE_Custom &&
        !SCharacterOwner->IsLeftMouseButtonPressed() && SCharacterOwner->GetWeaponManager() &&
        !SCharacterOwner->GetWeaponManager()->IsWeaponShooting();
}

bool USCharacterMovement::IsCustomMovementMove(ECustomMovementMode InCustomMovementMode) const
{
    return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}

void USCharacterMovement::InitializeComponent()
{
    Super::InitializeComponent();
    SCharacterOwner = Cast<ASCharacter>(CharacterOwner);
}

void USCharacterMovement::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
    Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
    Safe_bPrevWantsToCrouch = bWantsToCrouch;
}

void USCharacterMovement::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
    Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
    SCharacterOwner->SetSprinting(CanSprint() && bWantToSprint);
}

void USCharacterMovement::UpdateCharacterStateAfterMovement(float DeltaSeconds)
{
    Super::UpdateCharacterStateAfterMovement(DeltaSeconds);
}

void USCharacterMovement::PhysCustom(float deltaTime, int32 Iterations)
{
    Super::PhysCustom(deltaTime, Iterations);
    return;
    switch (CMOVE_Slide)
    {
    case CMOVE_Slide:
        PhysSlide(deltaTime, Iterations);
        break;
    default:
        UE_LOG(LogTemp, Error, TEXT("No MovementState"));
        break;
    }
}

void USCharacterMovement::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
    Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
    return;
    if (PreviousCustomMode == MOVE_Custom && PreviousCustomMode == CMOVE_Slide)
        EndSlide();
    if (IsCustomMovementMove(CMOVE_Slide))
        EnterSlide();
}

bool USCharacterMovement::CanSlide() const
{
    return false;
    FVector Start = UpdatedComponent->GetComponentLocation();
    FVector End = Start + CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.5f *
        FVector::DownVector;
    FName ProfileName = TEXT("BlockAll");
    FCollisionQueryParams Params;
    bool bValidSurface = GetWorld()->LineTraceTestByProfile(Start, End, ProfileName,
                                                            SCharacterOwner->GetIgnoreCollisionParams(Params));
    bool bEnoughSpeed = Velocity.SizeSquared() > pow(SlideMinSpeed, 2);
    return bValidSurface && bEnoughSpeed;
}

bool USCharacterMovement::IsMovingOnGround() const
{
    return Super::IsMovingOnGround() || IsCustomMovementMove(CMOVE_Slide);
}

bool USCharacterMovement::CanCrouchInCurrentState() const
{
    return Super::CanCrouchInCurrentState() && IsMovingOnGround();
}

void USCharacterMovement::EnterSlide()
{
    bWantsToCrouch = true;
    SCharacterOwner->SetSprinting(false);
    Velocity += Velocity.GetSafeNormal2D() * SlideImpulse;
    SetMovementMode(MOVE_Custom, CMOVE_Slide);
    FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, true,NULL);
}

void USCharacterMovement::EndSlide()
{
    bWantsToCrouch = false;
    //FQuat NewRotation = FRotationMatrix::MakeFromXZ(UpdatedComponent->GetForwardVector().GetSafeNormal2D(),
    //                                                FVector::UpVector).ToQuat();
    //FHitResult Hit;
    //SafeMoveUpdatedComponent(FVector::ZeroVector, NewRotation, true, Hit);
    //SetMovementMode(MOVE_Walking);

    if (bWantToSprint)
        SCharacterOwner->SetSprinting(CanSprint());
}

void USCharacterMovement::PhysSlide(float DeltaTime, int32 Iterations)
{
    SetMovementMode(MOVE_Walking);
    StartNewPhysics(DeltaTime, Iterations);
}
