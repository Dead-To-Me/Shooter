// Genderfield


#include "SPlayerStart.h"

#include "SCharacter.h"
#include "SPlayerState.h"
#include "Components/BoxComponent.h"


ASPlayerStart::ASPlayerStart(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    Box = CreateDefaultSubobject<UBoxComponent>("Box");
    Box->SetupAttachment(GetRootComponent());
}

bool ASPlayerStart::CanRespawnPlayer_Implementation()
{
    if (bLocked)
        return false;
    TArray<AActor*> Characters;
    Box->GetOverlappingActors(Characters, ASCharacter::StaticClass());
    bLocked = Characters.Num() != 0;
    if (bLocked)
        SetLockedTimer();
    return !bLocked;
}


void ASPlayerStart::SetLockedTimer()
{
    GetWorldTimerManager().ClearTimer(LockedTimer);
    GetWorldTimerManager().SetTimer(LockedTimer, this, &ASPlayerStart::ResetLocked, LockedDuration);
}

void ASPlayerStart::ResetLocked()
{
    bLocked = false;
}
