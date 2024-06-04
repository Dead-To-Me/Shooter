// Genderfield


#include "Components/SWeaponAttachmentLaser.h"

#include "NiagaraComponent.h"
#include "SCharacter.h"
#include "GameFramework/Character.h"

void USWeaponAttachmentLaser::CheckDistance()
{
    FHitResult Hit;
    FVector End = GetComponentLocation() + GetRightVector() * 500;
    FCollisionQueryParams Params = FCollisionQueryParams::DefaultQueryParam;
    TArray<AActor*> Ignore;
    Ignore.SetNum(2);
    Ignore.Add(GetOwner());
    Ignore.Add(GetOwner()->GetOwner());
    Params.AddIgnoredActors(Ignore);
    GetWorld()->LineTraceSingleByChannel(Hit, GetComponentLocation(), End, ECC_Visibility, Params);
    float Length = Hit.bBlockingHit ? Hit.Distance : 500;
    LaserSystem->SetVectorParameter("Beam End", FVector::RightVector * Length);
}

void USWeaponAttachmentLaser::BeginPlay()
{
    Super::BeginPlay();
    if (GetOwnerRole() == ROLE_Authority)
        return;
    ASCharacter* Character = GetOwner()->GetOwner<ASCharacter>();
    if (!Character)
        return;

    LaserSystem = Cast<UNiagaraComponent>(
        GetOwner()->AddComponentByClass(UNiagaraComponent::StaticClass(), true, FTransform::Identity, false));
    LaserSystem->SetAsset(NiagaraSystem);
    LaserSystem->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale, LaserSocket);
    LaserSystem->SetVectorParameter("Beam End", FVector::RightVector * 500);
    if (Character->IsLocallyControlled())
        return;
    GetWorld()->GetTimerManager().SetTimer(DistanceCheckTimer, this, &USWeaponAttachmentLaser::CheckDistance,
                                           LaserCheckDistanceRate, true);
}
