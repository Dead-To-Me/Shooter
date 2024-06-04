// Shooter27

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SBaseWidget.generated.h"

class UWidgetComponent;
/**
 * 
 */
UCLASS()
class SHOOTER27_API USBaseWidget : public UUserWidget
{
    GENERATED_BODY()

    UPROPERTY()
    UWidgetComponent* OwnerWidget;

    void UpdateRenderScale();

protected:
    virtual bool CheckIfVisible();
    virtual void Check();
    virtual void NativeConstruct() override;

public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnViewed();

protected:
    FTimerHandle CheckTimer;

    UPROPERTY(BlueprintReadWrite, meta=(ExposeOnSpawn=true))
    AActor* Instigator;

    UPROPERTY(EditDefaultsOnly)
    float CheckRate = 0.2f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    float Distance = 0;

    UPROPERTY(EditDefaultsOnly)
    FVector2D TranslationScale = FVector2D(0.7, 0.8);

    UPROPERTY(EditDefaultsOnly)
    FVector2D TranslationOffset = FVector2D::Zero();
};
