// Genderfield


#include "SControlPoint.h"

#include "SCharacter.h"
#include "SGameModeBase.h"
#include "SPlayerState.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


void ASControlPoint::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ASControlPoint, CurrentCaptureTeam);
    DOREPLIFETIME(ASControlPoint, ControlPointState);
    DOREPLIFETIME(ASControlPoint, TeamAPlayers);
    DOREPLIFETIME(ASControlPoint, TeamBPlayers);
}

// Sets default values
ASControlPoint::ASControlPoint()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
    CurrentCaptureTeam = ETeams::None;
    CapturingDelta = 0.25f;
    CaptureTime = 75;
    CaptureDelta = 0.2f;
    CurrentCaptureTime = 0;
    AddScoreDelta = 1;

    BoxControlZone = CreateDefaultSubobject<UBoxComponent>("BoxControlZone");
    BoxControlZone->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void ASControlPoint::BeginPlay()
{
    Super::BeginPlay();
    if (!HasAuthority())
        return;
    GameMode = Cast<ASGameModeBase>(UGameplayStatics::GetGameMode(this));
    BoxControlZone->OnComponentBeginOverlap.AddDynamic(this, &ASControlPoint::OnBeginOverlap);
    BoxControlZone->OnComponentEndOverlap.AddDynamic(this, &ASControlPoint::OnEndOverlap);
}

void ASControlPoint::CapturingPoint()
{
    if (TeamAPlayers == TeamBPlayers)
    {
        ControlPointState = EControlPointState::Resisting;
        CurrentCaptureTime = 0;
        return;
    }
    if (CurrentCaptureTime >= CaptureTime)
    {
        ControlPointState = EControlPointState::Captured;
        GetWorldTimerManager().ClearTimer(CapturingTimer);
        CurrentCaptureTime = 0;
        CurrentCaptureTeam = TeamAPlayers > TeamBPlayers ? ETeams::TeamA : ETeams::TeamB;
        GetWorldTimerManager().SetTimer(CaptureTimer, this, &ASControlPoint::AddTeamScore, AddScoreDelta, true);
        return;
    }
    ControlPointState = EControlPointState::Capturing;
    CurrentCaptureTime += CaptureDelta * TeamAPlayers > TeamBPlayers ? TeamAPlayers : TeamBPlayers;
}

void ASControlPoint::OnRep_ChangedControlPointState()
{
    if (!HasAuthority())
        return;
    switch (ControlPointState)
    {
    case EControlPointState::UnCaptured: // passthrough
    case EControlPointState::Capturing:
        GetWorldTimerManager().ClearTimer(CaptureTimer);
        break;
    case EControlPointState::Captured:
        
        break;
    case EControlPointState::Resisting:
        CurrentCaptureTime = 0;
        break;
    }
}


void ASControlPoint::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                    const FHitResult& SweepResult)
{
    if (!HasAuthority())
        return;
    ASCharacter* Character = Cast<ASCharacter>(OtherActor);
    if (!Character)
        return;
    ASPlayerState* PlayerState = Character->GetPlayerState<ASPlayerState>();
    if (!PlayerState)
        return;
    switch (PlayerState->GetTeam())
    {
    case ETeams::TeamA:
        TeamAPlayers++;
        break;
    case ETeams::TeamB:
        TeamBPlayers++;
        break;
    }
    if (CapturingTimer.IsValid())
        return;
    GetWorldTimerManager().SetTimer(CapturingTimer, this, &ASControlPoint::CapturingPoint, CapturingDelta, true);
}

void ASControlPoint::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (!HasAuthority())
        return;
    ASCharacter* Character = Cast<ASCharacter>(OtherActor);
    if (!Character)
        return;
    ASPlayerState* PlayerState = Character->GetPlayerState<ASPlayerState>();
    if (!PlayerState)
        return;
    switch (PlayerState->GetTeam())
    {
    case ETeams::TeamA:
        TeamAPlayers--;
        break;
    case ETeams::TeamB:
        TeamBPlayers--;
        break;
    }
    if (CurrentCaptureTeam == ETeams::None && TeamAPlayers == 0 && TeamBPlayers == 0)
    {
        CurrentCaptureTime = 0;
        GetWorldTimerManager().ClearTimer(CaptureTimer);
    }
}

void ASControlPoint::OnRep_ChangedControlTeam()
{
    if (!HasAuthority())
    {
        ClientsOnChangedControlTeam();
        return;
    }
    if (CurrentCaptureTeam == ETeams::None)
        GetWorldTimerManager().ClearTimer(CapturingTimer);
}
