// Shooter27

#pragma once

#include "CoreMinimal.h"
#include "SWeaponAttachmentComponent.generated.h"


enum class EWeaponAttachmentSlot : uint8;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class SHOOTER27_API USWeaponAttachmentComponent : public UStaticMeshComponent
{
    GENERATED_BODY()

    void CreateDynamicMaterials();

    UFUNCTION()
    void OnFOVChangedHandle(float NewFOV);

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    EWeaponAttachmentSlot AttachmentSlot;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    FString AttachmentName;

    UPROPERTY()
    TArray<UMaterialInstanceDynamic*> OwnerWeaponAttachmentMats;

    UPROPERTY(EditDefaultsOnly)
    FName FOVParamName = "FOV";

public:
    // Sets default values for this component's properties
    USWeaponAttachmentComponent();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;
};
