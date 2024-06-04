// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeaponData.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"


class ASPlayerController;
enum class EGameMode : uint8;
class ASGameModeBase;
class ASGameStateBase;

UENUM(BlueprintType)
enum class ETeams : uint8
{
    TeamA,
    TeamB,
    None
};


UENUM(BlueprintType)
enum class ERelation : uint8
{
    Enemy,
    Ally,
    Friend
};

/**
 * 
 */
UCLASS()
class SHOOTER27_API ASPlayerState : public APlayerState
{
    GENERATED_BODY()

    UPROPERTY()
    ASPlayerController* SPlayerController;

    UPROPERTY()
    ASGameStateBase* GameState;

    UPROPERTY()
    ASGameModeBase* GameMode;

protected:
    UPROPERTY(ReplicatedUsing=OnRep_Dead, VisibleAnywhere, BlueprintReadWrite)
    bool bDead;

    UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly)
    bool bCanRespawn;

    UPROPERTY(BlueprintReadOnly)
    FTimerHandle RespawnTimer;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
    int32 Kills;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
    int32 Deaths;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
    int32 Points;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FInventorySlots InventorySlots;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<TSubclassOf<ASWeaponFirearm>, FWeaponAttachments> WeaponAttachments;

    UPROPERTY(ReplicatedUsing=OnRepChangeTeam, EditDefaultsOnly, BlueprintReadWrite)
    ETeams Team;

public:
    UFUNCTION(BlueprintPure)
    const FInventorySlots& GetInventorySlots() const { return InventorySlots; }

    UFUNCTION(Server, Reliable, BlueprintCallable)
    void SetInventorySlots(const FInventorySlots& NewInventorySlots);

    UFUNCTION(Server, Reliable, BlueprintCallable)
    void SetWeaponAttachments(TSubclassOf<ASWeaponFirearm> WeaponClass, const FWeaponAttachments& NewWeaponAttachments);

    UFUNCTION(Server, Reliable, BlueprintCallable)
    void ServerSetPlayerName(const FString& Name);

    UFUNCTION(BlueprintCallable)
    void SetPlayerNameBlueprint(const FString& Name);

    UFUNCTION(Server, Reliable, BlueprintCallable)
    void ServerVote(EGameMode SelectGameMode);

    UFUNCTION(Server, Reliable, BlueprintCallable)
    void ServerVoteForMap(int32 MapIndex);

    UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable)
    void AddPoints(const float Value);

    UFUNCTION(BlueprintCallable)
    ERelation GetRelation(ASPlayerState* ToPlayer) const;

    UFUNCTION(BlueprintNativeEvent)
    void OnRepChangeTeam();

    UFUNCTION(BlueprintCallable)
    void ChangeTeam();

    UFUNCTION(Server, Reliable)
    void ServerChangeTeam();

    ASPlayerState();
    ETeams GetTeam() const { return Team; };
    int32 GetPoints() const { return Points; }
    void SetCanRespawn(bool State) { bCanRespawn = State; }
    bool CanRespawn() const { return bCanRespawn; }
    bool IsDead() const { return bDead; }
    void SetDead(const bool NewDead);

    const TMap<TSubclassOf<ASWeaponFirearm>, FWeaponAttachments>& GetWeaponAttachments() const
    {
        return WeaponAttachments;
    }

protected:
    UFUNCTION()
    void SetupDefaultWeaponAttachment();

    UFUNCTION()
    void OnRep_Dead();

    void RespawnTimerFinish();
    void SetRespawnTimer();
    virtual void BeginPlay() override;
};
