// Shooter27

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SHeathComponent.generated.h"


class ASCharacter;
class ASPlayerController;
class ASGameModeBase;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SHOOTER27_API USHeathComponent : public UActorComponent
{
    GENERATED_BODY()

    UPROPERTY()
    ASCharacter* Character;

    FTimerHandle HealthRegenerationTimer;
    FTimerHandle HealthRegenDelayTimer;

public:
    // Sets default values for this component's properties
    USHeathComponent();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

private:
    void HealthRegeneration();
    void StartHealthRegeneration();

public:
    UFUNCTION(BlueprintAuthorityOnly)
    void DealDamage(float Damage, AController* Killer, AActor* DamageCauser);

protected:
    UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite)
    float MaxHealth;

    UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite)
    float CurrentHealth;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float MaxRegenHeath;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float DelayHealthRegeneration;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float RegenHealthDelta;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float RegenHealthTimeDelta;

private:
    UPROPERTY()
    ASGameModeBase* GameMode;

public:
    float GetHeath() const { return CurrentHealth; }
};
