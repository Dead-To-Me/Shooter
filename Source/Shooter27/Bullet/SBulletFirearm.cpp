// Shooter27


#include "SBulletFirearm.h"
#include "SWeaponFirearm.h"
#include "Components/BoxComponent.h"
#include "Field/FieldSystemActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Shooter27/SBFL.h"


ASBulletFirearm::ASBulletFirearm()
{
    BoxCollision->bReturnMaterialOnMove = true;
    HeadDamageMode = 2;
    BodyDamageMode = 1;
}

void ASBulletFirearm::BulletHitCosmetic(const FHitResult& Hit)
{
    TableRowHandle.RowName = UEnum::GetValueAsName(UGameplayStatics::GetSurfaceType(Hit));
    FImpactData* ImpactData = TableRowHandle.GetRow<FImpactData>("GetRow");

    if (!ImpactData)
    {
        UE_LOG(LogTemp, Error, TEXT("Phys Mat is Invalid"));
        return;
    }

    USBFL::BulletHitEffect(this, Hit, *ImpactData);
}

void ASBulletFirearm::BeginPlay()
{
    Super::BeginPlay();
    BoxCollision->OnComponentHit.AddDynamic(this, &ASBulletFirearm::OnComponentHit);
    SpawnLocation = GetActorLocation();
}


void ASBulletFirearm::OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                     FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor == GetInstigator())
        return;

    if (bTookDamage)
        return;

    TurnOffCollision();

    GetWorld()->SpawnActor<AFieldSystemActor>(FieldSystem, GetActorLocation(), FRotator::ZeroRotator);

    if (!HasAuthority())
        BulletHitCosmetic(Hit);

    if (!OtherActor->ActorHasTag(DamageableTag))
        return;

    float Damage = Cast<ASWeaponFirearm>(GetOwner())->GetDamageCurve()->GetFloatValue(
        ProjectileMovement->Velocity.Length() / 100); // TODO: In Base Class

    // TODO: To Array?
    Damage *= Hit.BoneName == "neck_01" || Hit.BoneName == "head" ? HeadDamageMode : 1;
    Damage *= Hit.BoneName == "spine_01" || Hit.BoneName == "spine_02" || Hit.BoneName == "spine_03"
                  ? BodyDamageMode
                  : 1;

    UGameplayStatics::ApplyPointDamage(OtherActor, Damage, (SpawnLocation - GetActorLocation()).GetSafeNormal(),
                                       Hit,
                                       GetInstigatorController(),
                                       GetInstigator(),
                                       UDamageType::StaticClass());
}
