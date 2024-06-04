// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"
#include "SCharacter.h"
#include "SGameSession.h"
#include "SGameStateBase.h"
#include "SPlayerController.h"
#include "SPlayerState.h"
#include "SSpectatorPawn.h"
#include "Camera/CameraActor.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

ASGameModeBase::ASGameModeBase()
{
    bUseSeamlessTravel = true;
    bStartPlayersAsSpectators = true;
    GameSessionClass = ASGameSession::StaticClass();
    GameStateClass = ASGameStateBase::StaticClass();
    PlayerControllerClass = ASPlayerController::StaticClass();
    DefaultPawnClass = ASCharacter::StaticClass();
    SpectatorClass = ASSpectatorPawn::StaticClass();
}

void ASGameModeBase::OnPostLogin(AController* NewPlayer)
{
    Super::OnPostLogin(NewPlayer);
}

void ASGameModeBase::BeginPlay()
{
    Super::BeginPlay();
}

AActor* ASGameModeBase::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
    TArray<AActor*> PlayerStarts;
    UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
    return PlayerStarts[FMath::RandRange(0, PlayerStarts.Num() - 1)];
}

void ASGameModeBase::Logout(AController* Exiting)
{
    Super::Logout(Exiting);
    if (Exiting->GetPawn())
        Exiting->GetPawn()->Destroy();
}

void ASGameModeBase::PlayerDead(AController* DeadPlayer, AController* Killer, AActor* DamageCauser)
{
    if (!DeadPlayer || DeadPlayer->IsPendingKillPending())
        return;
    DeadPlayer->GetPlayerState<ASPlayerState>()->SetDead(true);
    if (!Killer || Killer->IsPendingKillPending())
    {
        RespawnAsSpectator(DeadPlayer);
        return;
    }

    ASpectatorPawn* SpectatorPawn = GetWorld()->SpawnActor<ASpectatorPawn>(SpectatorClass,
                                                                           Killer->GetPawn()->GetActorLocation(),
                                                                           FRotator::ZeroRotator);
    SpectatorPawn->SetInstigator(Killer->GetPawn());
    DeadPlayer->Possess(SpectatorPawn);
    OnPlayerDead.Broadcast(DeadPlayer, Killer, DamageCauser);
}

void ASGameModeBase::Redeployment(ASCharacter* Character)
{
    ASPlayerController* Player = Character->GetController<ASPlayerController>();
    if (!Player || Player->IsPendingKillPending())
        return;

    Character->GetPlayerState<ASPlayerState>()->SetDead(true);
    ASpectatorPawn* SpectatorPawn = GetWorld()->SpawnActor<ASpectatorPawn>(SpectatorClass,
                                                                           FindPlayerStart(Player)->GetActorLocation(),
                                                                           FRotator::ZeroRotator);
    Player->Possess(SpectatorPawn);
    OnPlayerDead.Broadcast(Player, Player, nullptr);
}

void ASGameModeBase::RespawnAsSpectator(AController* Controller)
{
    if (!Controller || Controller->IsPendingKillPending())
        return;
    AActor* StartSpot = UGameplayStatics::GetActorOfClass(this, ACameraActor::StaticClass());
    if (!StartSpot)
    {
        UE_LOG(LogGameMode, Fatal, TEXT("Can't Find Camera On Spectator Spawn"));
        return;
    }
    ASpectatorPawn* SpectatorPawn = GetWorld()->SpawnActor<ASpectatorPawn>(SpectatorClass,
                                                                           StartSpot->GetActorLocation(),
                                                                           FRotator::ZeroRotator);
    Controller->Possess(SpectatorPawn);
}

void ASGameModeBase::RespawnPlayer_Implementation(AController* NewPlayer)
{
    if (!NewPlayer || NewPlayer->IsPendingKillPending())
        return;
    if (!GameState->HasMatchStarted())
        return;

    AActor* StartSpot = FindPlayerStart(NewPlayer);
    RestartPlayerAtPlayerStart(NewPlayer, StartSpot);
}
