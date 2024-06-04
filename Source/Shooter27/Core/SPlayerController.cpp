// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"
#include "SGameModeBase.h"
#include "SGameStateBase.h"
#include "SHUD.h"
#include "GameFramework/PlayerInput.h"
#include "Kismet/GameplayStatics.h"


void ASPlayerController::ServerRespawn_Implementation()
{
    if (!(SPlayerState->CanRespawn() && SPlayerState->IsDead() && GameState->HasMatchStarted()))
        return;

    SPlayerState->SetDead(false);
    SPlayerState->SetCanRespawn(false);
    SetPawn(nullptr);
    GameMode->RespawnPlayer(this);
}


ASPlayerController::ASPlayerController()
{
    DefaultSens = 0.02f;
    AimSens = 0.02f;
}

void ASPlayerController::RespawnAsSpectator()
{
    SPlayerState->SetDead(true);
    SPlayerState->SetCanRespawn(false);
    SetPawn(nullptr);
    GameMode->RespawnAsSpectator(this);
}

void ASPlayerController::OnPlayerDamageCaller_Implementation(const FVector& Direction)
{
    OnPlayerDamage.Broadcast(Direction);
}

void ASPlayerController::ClientShowHitCrosshair_Implementation()
{
    GetHUD<ASHUD>()->ShowHitCrosshair();
}

void ASPlayerController::SetMouseSensitivity(float NewSens)
{
    PlayerInput->SetMouseSensitivity(NewSens);
}

void ASPlayerController::SetMouseInverted(bool NewState)
{
    PlayerInput->InvertAxis("MouseY");
}


void ASPlayerController::BeginPlay()
{
    Super::BeginPlay();
    GameState = Cast<ASGameStateBase>(UGameplayStatics::GetGameState(this));
    SPlayerState = GetPlayerState<ASPlayerState>();

    if (!HasAuthority())
        return;

    GameMode = Cast<ASGameModeBase>(UGameplayStatics::GetGameMode(this));
}
