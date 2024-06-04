// Shooter27

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SPlayerTagWidget.generated.h"

class ASCharacter;
/**
 * 
 */
UCLASS()
class SHOOTER27_API USPlayerTagWidget : public UUserWidget
{
    GENERATED_BODY()

    void VisibilityDelayCompleted();
    bool CheckIfVisible() const;
    void UpdateRenderScale();

protected:
    FTimerHandle CheckDistanceTimer;
    FTimerHandle SetVisibilityDelay;
    float HideTextFont = 2.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    ESlateVisibility PlayerNameVisibility = ESlateVisibility::Hidden;

    UPROPERTY(EditDefaultsOnly)
    float CheckRate = 0.1f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    ASCharacter* Player;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    float Distance = 0;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float HidePlayerNameDelay = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FVector2D TranslationOffset;

public:
    void ShowPlayerTag();

protected:
    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintNativeEvent)
    void CheckDistance();

    UFUNCTION(BlueprintImplementableEvent)
    void SetPlayerNameFont(const float FontSize);

private:
    UFUNCTION()
    void OnPlayerDead(bool bDead);
};
