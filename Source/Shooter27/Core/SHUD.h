// Shooter27

#pragma once

#include "CoreMinimal.h"
#include "SPlayerState.h"
#include "GameFramework/HUD.h"
#include "SHUD.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER27_API ASHUD : public AHUD
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void ShowHitCrosshair();

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void UpdateKillFeed(ASPlayerState* DeadPlayer, ASPlayerState* Killer, AActor* DamageCauser);

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void TogglePauseMenu();
};
