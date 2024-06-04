// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"

class ASPlayerState;
class ASGameStateBase;
class ASGameModeBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerDamage, FVector, Direction);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDead, bool, bIsDead);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFOVChanged, float, NewFOV);

/**
 * 
 */
UCLASS()
class SHOOTER27_API ASPlayerController : public APlayerController
{
    GENERATED_BODY()

    UPROPERTY()
    ASPlayerState* SPlayerState;

    UPROPERTY()
    ASGameStateBase* GameState;

    UPROPERTY()
    ASGameModeBase* GameMode;

protected:
    UPROPERTY(BlueprintReadWrite)
    float AimSens;

    UPROPERTY(BlueprintReadWrite)
    float DefaultSens;

public:
    UPROPERTY(BlueprintAssignable)
    FOnPlayerDamage OnPlayerDamage;

    UPROPERTY(BlueprintAssignable, BlueprintCallable)
    FOnDead OnPlayerDead;

    UPROPERTY(BlueprintAssignable, BlueprintCallable)
    FOnFOVChanged OnFOVChanged;

public:
    ASPlayerController();
    void RespawnAsSpectator();
    float GetAimSens() const { return AimSens; };
    float GetDefaultSens() const { return DefaultSens; }

    UFUNCTION(Client, Unreliable)
    void OnPlayerDamageCaller(const FVector& Direction);

    UFUNCTION(Server, Reliable, BlueprintCallable)
    void ServerRespawn();

    UFUNCTION(Client, Reliable)
    void ClientShowHitCrosshair();

    UFUNCTION(BlueprintCallable)
    void SetMouseSensitivity(float NewSens);

    UFUNCTION(BlueprintCallable)
    void SetMouseInverted(bool NewState);

    UFUNCTION(BlueprintCallable)
    void SetAimSens(float NewSens) { AimSens = NewSens; };

protected:
    virtual void BeginPlay() override;
};
