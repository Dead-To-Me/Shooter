// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"

#include "SGameModeBase.h"
#include "SGameStateBase.h"
#include "SPlayerController.h"
#include "SWeaponFirearm.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ASPlayerState, bDead);
    DOREPLIFETIME_CONDITION(ASPlayerState, bCanRespawn, COND_OwnerOnly);
    DOREPLIFETIME(ASPlayerState, Kills);
    DOREPLIFETIME(ASPlayerState, Deaths);
    DOREPLIFETIME(ASPlayerState, Points);
    DOREPLIFETIME(ASPlayerState, Team);
}

void ASPlayerState::ChangeTeam()
{
    if (!bDead)
        return;
    ServerChangeTeam();
}

void ASPlayerState::ServerChangeTeam_Implementation()
{
    if (!bDead)
        return;
    switch (GameState->GetCurrentGameMode())
    {
    case EGameMode::TeamDeathmatch: //passthrough
    case EGameMode::Conquest:
        Team = static_cast<ETeams>(abs(static_cast<int32>(Team) - 1));
        break;
    }
}

ASPlayerState::ASPlayerState()
{
    Kills = 0;
    Deaths = 0;
    Points = 0;
    bDead = true;
    bCanRespawn = false;
}

void ASPlayerState::SetInventorySlots_Implementation(const FInventorySlots& NewInventorySlots)
{
    InventorySlots = NewInventorySlots;
}

void ASPlayerState::SetWeaponAttachments_Implementation(TSubclassOf<ASWeaponFirearm> WeaponClass,
                                                        const FWeaponAttachments& NewWeaponAttachments)
{
    FWeaponAttachments* WeaponAttachment = WeaponAttachments.Find(WeaponClass);
    if (WeaponAttachment)
    {
        WeaponAttachment->ScopeClass = NewWeaponAttachments.ScopeClass;
        WeaponAttachment->MagClass = NewWeaponAttachments.MagClass;
        WeaponAttachment->BulletClass = NewWeaponAttachments.BulletClass;
        WeaponAttachment->Laser = NewWeaponAttachments.Laser;
        return;
    }
    WeaponAttachments.Add(WeaponClass, NewWeaponAttachments);
}

void ASPlayerState::ServerSetPlayerName_Implementation(const FString& Name)
{
    if (Name.IsEmpty())
        return;
    SetPlayerName(Name);
}

void ASPlayerState::SetPlayerNameBlueprint(const FString& Name)
{
    if (Name.IsEmpty())
        return;
    SetPlayerName(Name);
}

void ASPlayerState::ServerVoteForMap_Implementation(int32 MapIndex)
{
    GameState->VoteForGameMap(this, MapIndex);
}

void ASPlayerState::AddPoints(const float Value)
{
    Points += Value;
}

ERelation ASPlayerState::GetRelation(ASPlayerState* ToPlayer) const
{
    if (!ToPlayer)
        return ERelation::Enemy;
    if (this == ToPlayer)
        return ERelation::Ally;
    if (GameState && GameState->GetCurrentGameMode() == EGameMode::Deathmatch)
        return ERelation::Enemy;
    if (ToPlayer->GetTeam() == Team)
        return ERelation::Ally;
    return ERelation::Enemy;
}

void ASPlayerState::OnRepChangeTeam_Implementation()
{
}

void ASPlayerState::ServerVote_Implementation(EGameMode SelectGameMode)
{
    GameState->VoteForGameMode(this, SelectGameMode);
}

void ASPlayerState::SetupDefaultWeaponAttachment()
{
    if (InventorySlots.PrimaryWeapon->IsValidLowLevel())
        WeaponAttachments.Add(InventorySlots.PrimaryWeapon,
                              InventorySlots.PrimaryWeapon.GetDefaultObject()->GetAttachments());
    if (InventorySlots.SecondaryWeapon->IsValidLowLevel())
        WeaponAttachments.Add(InventorySlots.SecondaryWeapon,
                              InventorySlots.SecondaryWeapon.GetDefaultObject()->GetAttachments());
}

void ASPlayerState::OnRep_Dead()
{
    if (!GetWorld())
        return;
    if (HasAuthority())
        return;
    if (!SPlayerController)
        return;

    SPlayerController->OnPlayerDead.Broadcast(bDead);

    if (!bDead)
        return;
    if (!GameState)
        GameState = Cast<ASGameStateBase>(UGameplayStatics::GetGameState(this));

    GetWorldTimerManager().SetTimer(RespawnTimer, this, &ASPlayerState::RespawnTimerFinish,
                                    GameState->GetRespawnTime());
}

void ASPlayerState::RespawnTimerFinish()
{
    bCanRespawn = true;
    GetWorldTimerManager().ClearTimer(RespawnTimer);
}

void ASPlayerState::SetRespawnTimer()
{
    if (!GameState)
        return;
    if (!GameState->HasMatchStarted())
        return;

    GetWorldTimerManager().SetTimer(RespawnTimer, this, &ASPlayerState::RespawnTimerFinish,
                                    GameState->GetRespawnTime());
}

void ASPlayerState::BeginPlay()
{
    Super::BeginPlay();
    if (GetWorld()->GetNetMode() == NM_Standalone)
        return;

    GameState = Cast<ASGameStateBase>(UGameplayStatics::GetGameState(this));
    SPlayerController = Cast<ASPlayerController>(GetPlayerController());
    SetRespawnTimer();
    if (!HasAuthority())
        return;

    GameMode = Cast<ASGameModeBase>(UGameplayStatics::GetGameMode(this));
    SetupDefaultWeaponAttachment();
}

void ASPlayerState::SetDead(const bool NewDead)
{
    bDead = NewDead;

    if (!bDead)
        return;
    DisableInput(GetPlayerController());
    SetRespawnTimer();
}
