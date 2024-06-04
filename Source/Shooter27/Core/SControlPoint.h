// Genderfield

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SControlPoint.generated.h"

class ASGameModeBase;
class UBoxComponent;
enum class ETeams : uint8;

UENUM(Blueprintable)
enum class EControlPointState : uint8
{
    UnCaptured,
    Capturing,
    Captured,
    Resisting
};

UCLASS()
class SHOOTER27_API ASControlPoint : public AActor
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UBoxComponent* BoxControlZone;

    UPROPERTY(ReplicatedUsing=OnRep_ChangedControlTeam, EditAnywhere, BlueprintReadWrite)
    ETeams CurrentCaptureTeam;

    UPROPERTY(ReplicatedUsing=OnRep_ChangedControlPointState, VisibleInstanceOnly, BlueprintReadWrite)
    EControlPointState ControlPointState;

    UPROPERTY(BlueprintReadWrite)
    ASGameModeBase* GameMode;

    UPROPERTY(BlueprintReadWrite)
    FTimerHandle CapturingTimer;

    UPROPERTY(BlueprintReadWrite)
    FTimerHandle CaptureTimer;

    UPROPERTY(EditDefaultsOnly)
    float AddScoreDelta;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float CapturingDelta;

    UPROPERTY(Replicated, BlueprintReadWrite)
    int32 TeamAPlayers;

    UPROPERTY(Replicated, BlueprintReadWrite)
    int32 TeamBPlayers;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float CaptureTime;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float CaptureDelta;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float CurrentCaptureTime;

public:
    // Sets default values for this actor's properties
    ASControlPoint();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    void CapturingPoint();

    UFUNCTION()
    void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool
                        bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    UFUNCTION()
    void OnRep_ChangedControlTeam();

    UFUNCTION()
    void OnRep_ChangedControlPointState();

    UFUNCTION(BlueprintImplementableEvent)
    void AddTeamScore();

    UFUNCTION(BlueprintImplementableEvent)
    void ClientsOnChangedControlTeam();
};
