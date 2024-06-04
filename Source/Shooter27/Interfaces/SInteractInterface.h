// Shooter27

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SInteractInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USInteractInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 * 
 */
class SHOOTER27_API ISInteractInterface
{
    GENERATED_BODY()

    // Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void Interact(ASCharacter* Character);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void BecomeViewTargetInterface();

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void ShowInteractWidget();
};
