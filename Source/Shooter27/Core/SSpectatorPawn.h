// Shooter27

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "SSpectatorPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class SHOOTER27_API ASSpectatorPawn : public ASpectatorPawn
{
    GENERATED_BODY()

    FVector SpawnLocation;
    FRotator SpawnRotation;
    bool bEnableLerp;
    float LerpTimeElapsed;
    FTimerHandle FollowKillerTimer;
    
    UPROPERTY()
    AActor* ToActor;

public:
    // Sets default values for this pawn's properties
    ASSpectatorPawn();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    void LerpToActor();
    void StopFollowKiller();
    void SetLerpActor();

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    UCameraComponent* Camera;

    UPROPERTY(EditDefaultsOnly)
    float LerpDuration = 1.5f;

    UPROPERTY(EditDefaultsOnly)
    float FollowKillerTime = 3.5f;
};
