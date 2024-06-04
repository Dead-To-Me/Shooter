// Shooter27

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SGameStateBase.generated.h"

class ASPlayerState;
struct FDataTableRowHandle;

UENUM(BlueprintType)
enum class EGameMode : uint8
{
    Deathmatch UMETA(DisplayName="DM"),
    TeamDeathmatch UMETA(DisplayName="TDM"),
    Conquest UMETA(DisplayName="Conquest"),
    Lobby UMETA(DisplayName="Lobby"),
    MAX
};

UENUM(BlueprintType)
enum class EMatchState : uint8
{
    Preparation,
    Playing,
    End
};

USTRUCT(BlueprintType)
struct FGameMaps : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString LevelName;
};

/**
 * 
 */
UCLASS()
class SHOOTER27_API ASGameStateBase : public AGameStateBase
{
    GENERATED_BODY()

public:
    ASGameStateBase();

private:
    UPROPERTY()
    ASGameModeBase* GameModeBase;

    FTimerHandle EndMatchTimer;
    FString NextGameMap;
    EGameMode NextGameMode;
    TArray<FName> MapsTable;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GameModeSettings")
    EGameMode CurrentGameMode;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GameModeSettings")
    int32 GameGoal;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GameModeSettings")
    float MatchTime;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GameModeSettings")
    int32 MinNumPlayersToStartMatch;

    UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite)
    int32 CurrentGoal;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GameModeSettings")
    float RespawnTime;

    UPROPERTY(EditDefaultsOnly, Category="GameModeSettings")
    float DelayAfterEndMatch;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GameModeSettings")
    int32 BonusPointAfterKill;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GameModeSettings")
    int32 PenaltySuicide;

    UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly)
    FTimespan CurrentMatchTime;

    UPROPERTY(ReplicatedUsing=OnRep_MatchState, VisibleInstanceOnly, BlueprintReadWrite)
    EMatchState MatchState;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    FTimerHandle MatchTimeTimer;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TMap<ASPlayerState*, EGameMode> VotedPlayers;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    TMap<ASPlayerState*, int32> VotedMaps;

    UPROPERTY(EditDefaultsOnly)
    FDataTableRowHandle AvailableMaps;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool bCanVote = true;

    UPROPERTY(BlueprintReadOnly)
    FTimerHandle AcceptGameSettingsTimer;

public:
    float GetRespawnTime() const { return RespawnTime; }

    UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly, BlueprintCallable)
    void StartMatch();

    UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly, BlueprintCallable)
    void EndMatch();

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnRep_MatchState();

    UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly, BlueprintCallable)
    void AddCurrentGoal();

    UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly, BlueprintCallable)
    void AfterMatchEnd();

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void SelectGameMode();

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void SelectGameMap();

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void ResetGame();

    UFUNCTION(NetMulticast, Reliable)
    void MulticastPlayerDead(ASPlayerState* DeadPlayer, ASPlayerState* Killer, AActor* DamageCauser);

    UFUNCTION(NetMulticast, Reliable)
    void MapVoteCountUpdate(int32 MapIndex, int32 VoteCount);

    UFUNCTION(BlueprintImplementableEvent)
    void MulticastMapVoteCountUpdateBlueprint(int32 MapIndex, int32 VoteCount);

    UFUNCTION()
    void OnPlayerDead(AController* DeadPlayer, AController* Killer, AActor* DamageCauser);

    void VoteForGameMode(ASPlayerState* Player, EGameMode SelectedGameMode);
    void VoteForGameMap(ASPlayerState* Player, int32 SelectedGameMapIndex);
    int32 CountMapVotes(int32 MapIndex) const;
    void SelectRandomMap();
    EGameMode GetCurrentGameMode() const { return CurrentGameMode; }

protected:
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintAuthorityOnly)
    void AddPlayTime();

    UFUNCTION(BlueprintNativeEvent)
    void CheckCanStartMatch();

public:
    virtual void AddPlayerState(APlayerState* PlayerState) override;
    virtual void RemovePlayerState(APlayerState* PlayerState) override;
    virtual bool HasMatchStarted() const override { return MatchState == EMatchState::Playing; };
};
