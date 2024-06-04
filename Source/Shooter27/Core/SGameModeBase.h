// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameModeBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPlayerDead, AController*, DeadPlayer, AController*, Killer, AActor*, DamageCauser);

class ASGameStateBase;
/**
 * 
 */
UCLASS()
class SHOOTER27_API ASGameModeBase : public AGameModeBase
{
    GENERATED_BODY()

public:
    ASGameModeBase();

    UPROPERTY(BlueprintAssignable)
    FOnPlayerDead OnPlayerDead;

protected:
    virtual void OnPostLogin(AController* NewPlayer) override;
    // TODO: virtual void FailedToRestartPlayer(AController* NewPlayer) override;
    virtual void BeginPlay() override;

public:
    virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName) override;
    virtual void Logout(AController* Exiting) override;
    void PlayerDead(AController* DeadPlayer, AController* Killer, AActor* DamageCauser);
    void Redeployment(ASCharacter* Character);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, BlueprintNativeEvent)
    void RespawnPlayer(AController* NewPlayer);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void RespawnAsSpectator(AController* Controller);
};
