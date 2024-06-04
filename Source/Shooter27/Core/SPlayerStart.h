// Genderfield

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "SPlayerStart.generated.h"

enum class ETeams : uint8;
class UBoxComponent;
/**
 * 
 */
UCLASS()
class SHOOTER27_API ASPlayerStart : public APlayerStart
{
    GENERATED_BODY()

public:
    ASPlayerStart(const FObjectInitializer& ObjectInitializer);


protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    UBoxComponent* Box;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ETeams Team;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    bool bLocked;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float LockedDuration;

    UPROPERTY(BlueprintReadWrite)
    FTimerHandle LockedTimer;

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    bool CanRespawnPlayer();

    UFUNCTION(BlueprintCallable)
    void SetLockedTimer();

private:
    void ResetLocked();
};
