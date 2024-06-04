// Shooter27

#pragma once

#include "CoreMinimal.h"
#include "Components/SWeaponAttachmentComponent.h"
#include "SWeaponAttachmentScope.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER27_API USWeaponAttachmentScope : public USWeaponAttachmentComponent
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* ScopeOverlay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 ScopeMatIndex = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ScopeFOV = 5.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 WHRenderTarget = 256;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName AttachSocket = "Aim";

protected:
    virtual void BeginPlay() override;
};
