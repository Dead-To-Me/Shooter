// Shooter27


#include "SBaseWidget.h"

#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"

void USBaseWidget::UpdateRenderScale()
{
    float NormDistance = UKismetMathLibrary::NormalizeToRange(Distance, 0, 3000);
    NormDistance = FMath::GetMappedRangeValueClamped(FVector2D(0, 1), TranslationScale, NormDistance);
    SetRenderScale(FVector2D(NormDistance));
    float Location = FMath::GetMappedRangeValueClamped(FVector2D(0, 1), TranslationOffset, NormDistance);
    SetRenderTranslation(FVector2D(0, Location));
}

bool USBaseWidget::CheckIfVisible()
{
    FHitResult Hit;
    FCollisionQueryParams Params = FCollisionQueryParams::DefaultQueryParam;
    TArray<AActor*> IgnoredActors = {Instigator, GetOwningPlayerPawn()};
    Params.AddIgnoredActors(IgnoredActors);
    return !GetWorld()->LineTraceSingleByChannel(Hit, GetOwningLocalPlayer()->LastViewLocation,
                                                 OwnerWidget->GetComponentLocation(), ECC_Visibility, Params);
}

void USBaseWidget::Check()
{
    if (!CheckIfVisible())
    {
        SetVisibility(ESlateVisibility::Hidden);
        return;
    }

    Distance = FVector::Dist(GetOwningLocalPlayer()->LastViewLocation, Instigator->GetActorLocation());

    if (!IsVisible())
        SetVisibility(ESlateVisibility::Visible);

    UpdateRenderScale();
}

void USBaseWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (!Instigator)
        return;
    OwnerWidget = Instigator->GetComponentByClass<UWidgetComponent>();
    if (!OwnerWidget)
        return;

    GetWorld()->GetTimerManager().SetTimer(CheckTimer, this, &USBaseWidget::Check, CheckRate,
                                           true);
}

void USBaseWidget::OnViewed_Implementation()
{
}
