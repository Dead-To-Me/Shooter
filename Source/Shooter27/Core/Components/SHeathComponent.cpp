// Shooter27


#include "SHeathComponent.h"
#include "SCharacter.h"
#include "SGameModeBase.h"
#include "SPlayerController.h"
#include "SPlayerState.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
USHeathComponent::USHeathComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
    MaxRegenHeath = 50;
    MaxHealth = 100;
    CurrentHealth = MaxHealth;
    DelayHealthRegeneration = 5;
    RegenHealthDelta = 1.5f;
    RegenHealthTimeDelta = 0.5f;
}

void USHeathComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(USHeathComponent, CurrentHealth);
    DOREPLIFETIME(USHeathComponent, MaxHealth);
}

// Called when the game starts
void USHeathComponent::BeginPlay()
{
    Super::BeginPlay();
    if (GetOwnerRole() == ENetRole::ROLE_Authority)
    {
        Character = GetOwner<ASCharacter>();
        GameMode = Cast<ASGameModeBase>(GetWorld()->GetAuthGameMode());
    }
}

void USHeathComponent::HealthRegeneration()
{
    CurrentHealth = FMath::Min(CurrentHealth + RegenHealthDelta, MaxRegenHeath);
    if (CurrentHealth == MaxRegenHeath)
        GetWorld()->GetTimerManager().ClearTimer(HealthRegenerationTimer);
}

void USHeathComponent::StartHealthRegeneration()
{
    GetWorld()->GetTimerManager().ClearTimer(HealthRegenDelayTimer);
    GetWorld()->GetTimerManager().SetTimer(HealthRegenerationTimer, this, &USHeathComponent::HealthRegeneration,
                                           RegenHealthTimeDelta, true);
}

void USHeathComponent::DealDamage(const float Damage, AController* Killer, AActor* DamageCauser)
{
    if (!Character)
        return;
    if (!Character->GetPlayerState())
        return;
    if (Character->GetPlayerState<ASPlayerState>()->IsDead())
        return;

    CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);

    GetWorld()->GetTimerManager().ClearTimer(HealthRegenerationTimer);
    GetWorld()->GetTimerManager().ClearTimer(HealthRegenDelayTimer);
    GetWorld()->GetTimerManager().SetTimer(HealthRegenDelayTimer, this, &USHeathComponent::StartHealthRegeneration,
                                           DelayHealthRegeneration);

    if (!Killer)
        return;

    Cast<ASPlayerController>(Killer)->ClientShowHitCrosshair();
    ASPlayerState* KillerPlayerState = Killer->GetPlayerState<ASPlayerState>();
    if (KillerPlayerState != Character->GetPlayerState())
        KillerPlayerState->AddPoints(Damage);

    if (CurrentHealth > 0)
        return;
    GameMode->PlayerDead(Character->GetController(), Killer, DamageCauser);
}
