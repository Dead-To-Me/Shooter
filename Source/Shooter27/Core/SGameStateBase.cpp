// Shooter27


#include "SGameStateBase.h"

#include "SGameModeBase.h"
#include "SHUD.h"
#include "SPlayerController.h"
#include "SPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void ASGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ASGameStateBase, CurrentMatchTime);
    DOREPLIFETIME(ASGameStateBase, MatchState);
    DOREPLIFETIME(ASGameStateBase, CurrentGoal);
}

ASGameStateBase::ASGameStateBase()
{
    PrimaryActorTick.bCanEverTick = false;
    CurrentGameMode = EGameMode::Deathmatch;
    GameGoal = 31;
    MatchTime = 900;
    MinNumPlayersToStartMatch = 1;
    RespawnTime = 5;
    DelayAfterEndMatch = 15;
    BonusPointAfterKill = 10;
    PenaltySuicide = 100;
    MatchState = EMatchState::Preparation;
}

void ASGameStateBase::StartMatch_Implementation()
{
    MatchState = EMatchState::Playing;
    GetWorldTimerManager().SetTimer(MatchTimeTimer, this, &ASGameStateBase::AddPlayTime, 1.0f, true);
}

void ASGameStateBase::EndMatch_Implementation()
{
    if (MatchState != EMatchState::Playing)
        return;
    MatchState = EMatchState::End;
    GetWorldTimerManager().ClearTimer(MatchTimeTimer);
    for (auto PlayerState : PlayerArray)
    {
        Cast<ASPlayerController>(PlayerState->GetPlayerController())->RespawnAsSpectator();
    }
    GetWorldTimerManager().SetTimer(EndMatchTimer, this, &ASGameStateBase::AfterMatchEnd, DelayAfterEndMatch);
}

void ASGameStateBase::AddPlayTime()
{
    if (CurrentMatchTime.GetSeconds() >= MatchTime)
        EndMatch();

    CurrentMatchTime += FTimespan::FromSeconds(1.0);
}

void ASGameStateBase::CheckCanStartMatch_Implementation()
{
    if (MatchState > EMatchState::Preparation)
        return;

    if (MinNumPlayersToStartMatch >= PlayerArray.Num())
        StartMatch();
}

void ASGameStateBase::AddCurrentGoal_Implementation()
{
    CurrentGoal++;
    if (CurrentGoal >= GameGoal)
        EndMatch();
}

void ASGameStateBase::AddPlayerState(APlayerState* PlayerState)
{
    Super::AddPlayerState(PlayerState);
    CheckCanStartMatch();
}

void ASGameStateBase::RemovePlayerState(APlayerState* PlayerState)
{
    Super::RemovePlayerState(PlayerState);
    if (PlayerArray.Num() == 0)
        ResetGame();
}

void ASGameStateBase::MapVoteCountUpdate_Implementation(int32 MapIndex, int32 VoteCount)
{
    if (HasAuthority())
        return;
    MulticastMapVoteCountUpdateBlueprint(MapIndex, VoteCount);
}

void ASGameStateBase::OnPlayerDead(AController* DeadPlayer, AController* Killer, AActor* DamageCauser)
{
    if (!DeadPlayer)
        return;
    if (!Killer)
        return;
    MulticastPlayerDead(DeadPlayer->GetPlayerState<ASPlayerState>(), Killer->GetPlayerState<ASPlayerState>(),
                        DamageCauser);
}

void ASGameStateBase::VoteForGameMode(ASPlayerState* Player, const EGameMode SelectedGameMode)
{
    if (MatchState != EMatchState::End)
        return;
    EGameMode& VotedGameMode = VotedPlayers.FindOrAdd(Player, SelectedGameMode);
    VotedGameMode = SelectedGameMode;
}

void ASGameStateBase::VoteForGameMap(ASPlayerState* Player, int32 SelectedGameMapIndex)
{
    if (MatchState != EMatchState::End)
        return;
    if (!MapsTable.IsValidIndex(SelectedGameMapIndex))
        return;

    int32& VotedGameMap = VotedMaps.FindOrAdd(Player, SelectedGameMapIndex);
    VotedGameMap = VotedGameMap == SelectedGameMapIndex ? VotedGameMap : SelectedGameMapIndex;
    MapVoteCountUpdate(SelectedGameMapIndex, CountMapVotes(SelectedGameMapIndex));
}

int32 ASGameStateBase::CountMapVotes(int32 MapIndex) const
{
    int32 Votes = 0;
    TArray<int32> VotedGameMaps;
    VotedMaps.GenerateValueArray(VotedGameMaps);
    for (auto Map : VotedGameMaps)
    {
        Votes += (Map == MapIndex) * 1;
    }
    return Votes;
}

void ASGameStateBase::SelectRandomMap()
{
    AvailableMaps.RowName = MapsTable[FMath::RandRange(0, MapsTable.Num() - 1)];
    NextGameMap = AvailableMaps.GetRow<FGameMaps>("GetRow")->LevelName;
    UE_LOG(LogTemp, Warning, TEXT("Random Map: %s"), *NextGameMap);
}

void ASGameStateBase::OnRep_MatchState_Implementation()
{
}

void ASGameStateBase::AfterMatchEnd_Implementation()
{
    //ResetGame();
    // DeathMatch
    //FText Command = FText::Format(LOCTEXT("servertravel /Shooter27/Levels/{0}?game={1}"), LevelName, GameModeName);

    SelectGameMap();
    //FString LevelName = UGameplayStatics::GetCurrentLevelName(this);
    //FString GameModeName = "DeathMatch";
    //FString Command = FString::Printf(
    //    TEXT("servertravel /Game/Shooter27/Lobby"));

    //UE_LOG(LogTemp, Display, TEXT("Trevel"));
    //UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(),TEXT("servertravel /Game/Shooter27/Lobby"));
    GetWorld()->ServerTravel(NextGameMap);
    // GetWorld()->Exec(GetWorld(), TEXT("servertravel /Game/Shooter27/Lobby"));
}

void ASGameStateBase::SelectGameMode()
{
    if (VotedPlayers.IsEmpty())
    {
        NextGameMode = static_cast<EGameMode>(FMath::RandRange(0, static_cast<int32>(EGameMode::MAX) - 1));
        return;
    }

    TMap<EGameMode, int32> Votes;
    TArray<EGameMode> GameModes;
    VotedPlayers.GenerateValueArray(GameModes);
    for (auto GameMode : GameModes)
    {
        int32& Vote = Votes.FindOrAdd(GameMode, 0);
        Vote++;
    }

    Votes.ValueSort([](int32 A, int32 B)
    {
        return A > B;
    });
    GameModes.Empty();
    Votes.GenerateKeyArray(GameModes);
    NextGameMode = GameModes[0];
}

void ASGameStateBase::SelectGameMap()
{
    if (VotedMaps.IsEmpty())
    {
        SelectRandomMap();
        return;
    }

    TMap<FName, int32> Votes;
    TArray<int32> VotedGameMaps;
    VotedMaps.GenerateValueArray(VotedGameMaps);
    for (auto Map : VotedGameMaps)
    {
        int32& VoteMap = Votes.FindOrAdd(MapsTable[Map], 0);
        VoteMap++;
    }

    Votes.ValueSort([](int32 A, int32 B)
    {
        return A > B;
    });

    TArray<FName> VotesArray;
    Votes.GenerateKeyArray(VotesArray);
    if (!VotesArray.IsValidIndex(0))
    {
        SelectRandomMap();
        return;
    }
    NextGameMap = VotesArray[0].ToString();
    UE_LOG(LogTemp, Warning, TEXT("Selected Map: %s, Votes: %d"), *NextGameMap, *Votes.Find(VotesArray[0]));
    VotedGameMaps.Empty();
    VotesArray.Empty();
}


void ASGameStateBase::ResetGame()
{
    MatchState = EMatchState::Preparation;
    GetWorldTimerManager().ClearTimer(MatchTimeTimer);
    CurrentGoal = 0;
    CheckCanStartMatch();
}

void ASGameStateBase::MulticastPlayerDead_Implementation(ASPlayerState* DeadPlayer, ASPlayerState* Killer,
                                                         AActor* DamageCauser)
{
    if (HasAuthority())
        return;
    UGameplayStatics::GetPlayerController(this, 0)->GetHUD<ASHUD>()->UpdateKillFeed(DeadPlayer, Killer, DamageCauser);
}

void ASGameStateBase::BeginPlay()
{
    Super::BeginPlay();
    if (!HasAuthority())
        return;
    GameModeBase = Cast<ASGameModeBase>(UGameplayStatics::GetGameMode(this));
    GameModeBase->OnPlayerDead.AddDynamic(this, &ASGameStateBase::OnPlayerDead);
    if (!AvailableMaps.DataTable)
        return;
    MapsTable = AvailableMaps.DataTable->GetRowNames();
}
