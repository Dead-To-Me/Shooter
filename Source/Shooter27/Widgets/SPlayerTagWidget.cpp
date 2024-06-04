// Shooter27


#include "SPlayerTagWidget.h"
#include "SCharacter.h"
#include "SPlayerController.h"
#include "Kismet/KismetMathLibrary.h"


void USPlayerTagWidget::VisibilityDelayCompleted()
{
    PlayerNameVisibility = ESlateVisibility::Hidden;
    SetVisibility(ESlateVisibility::Hidden);
    SetPlayerNameFont(2);
}

void USPlayerTagWidget::OnPlayerDead(bool bDead)
{
    if (!bDead)
        return;
    SetVisibility(ESlateVisibility::Hidden);
    GetWorld()->GetTimerManager().ClearTimer(CheckDistanceTimer);
}

bool USPlayerTagWidget::CheckIfVisible() const
{
    FHitResult Hit;
    FCollisionQueryParams Params = FCollisionQueryParams::DefaultQueryParam;
    // TArray<AActor*> IgnoreActors = {Player, GetOwningPlayerPawn()};
    Params.AddIgnoredActor(Player);
    return !GetWorld()->LineTraceSingleByChannel(Hit, GetOwningLocalPlayer()->LastViewLocation,
                                                 Player->GetActorLocation(),
                                                 ECC_Visibility, Params);
}

void USPlayerTagWidget::UpdateRenderScale()
{
    float NormDistance = UKismetMathLibrary::NormalizeToRange(Distance, 0, 3000);
    NormDistance = FMath::GetMappedRangeValueClamped(FVector2D(0, 1), FVector2D(0.7, 0.8), NormDistance);
    SetRenderScale(FVector2D(NormDistance));
    float Location = FMath::GetMappedRangeValueClamped(FVector2D(0, 1), TranslationOffset, NormDistance);
    SetRenderTranslation(FVector2D(0, Location));
}

void USPlayerTagWidget::ShowPlayerTag()
{
    SetPlayerNameFont(20);
    PlayerNameVisibility = ESlateVisibility::Visible;
    SetVisibility(ESlateVisibility::Visible);
    GetWorld()->GetTimerManager().ClearTimer(SetVisibilityDelay);
    GetWorld()->GetTimerManager().SetTimer(SetVisibilityDelay, this, &USPlayerTagWidget::VisibilityDelayCompleted,
                                           HidePlayerNameDelay);
}

void USPlayerTagWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (!GetWorld())
        return;
    GetWorld()->GetTimerManager().SetTimer(CheckDistanceTimer, this, &USPlayerTagWidget::CheckDistance, CheckRate,
                                           true);
    if (!GetOwningPlayer())
        return;
    ASPlayerController* Controller = GetOwningPlayer<ASPlayerController>();
    checkf(Controller, TEXT("PlayerTag: Controller Invalid"));
    Controller->OnPlayerDead.AddDynamic(this, &USPlayerTagWidget::OnPlayerDead);
}

void USPlayerTagWidget::CheckDistance_Implementation()
{
    if (!Player)
        return;
    if (!Player->WasRecentlyRendered())
        return;
    Distance = FVector::Dist(GetOwningLocalPlayer()->LastViewLocation, Player->GetActorLocation());
    if (PlayerNameVisibility == ESlateVisibility::Visible)
        UpdateRenderScale();
    /*
    if (Distance > 5000)
    {
        SetVisibility(ESlateVisibility::Hidden);
        return;
    }
    ESlateVisibility NewVis = CheckIfVisible() ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
    SetVisibility(NewVis);
    */
}
