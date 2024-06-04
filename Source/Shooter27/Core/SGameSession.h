// Shooter27

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "SGameSession.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER27_API ASGameSession : public AGameSession
{
    GENERATED_BODY()

    virtual void RegisterServer() override;
};
