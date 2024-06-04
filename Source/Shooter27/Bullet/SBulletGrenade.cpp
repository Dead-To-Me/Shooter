// Shooter27


#include "SBulletGrenade.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/BoxComponent.h"
#include "Field/FieldSystemActor.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

void ASBulletGrenade::BulletHitCosmetic(const FHitResult& Hit)
{
    Super::BulletHitCosmetic(Hit);
    const FRotator Rotator = Hit.Normal.Rotation();
    UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, Explosion, GetActorLocation(), Rotator);
    UGameplayStatics::SpawnSoundAtLocation(this, ExplosionSound, GetActorLocation());
    // TODO: Spawn Decal
}

void ASBulletGrenade::BeginPlay()
{
    Super::BeginPlay();
    BoxCollision->OnComponentHit.AddDynamic(this, &ASBulletGrenade::OnComponentHit);
}

void ASBulletGrenade::OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                     FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor == GetInstigator())
        return;

    if (bTookDamage)
        return;

    GetWorld()->SpawnActor<AFieldSystemActor>(FieldSystem, GetActorLocation(), FRotator::ZeroRotator);

    TurnOffCollision();

    if (!HasAuthority())
        BulletHitCosmetic(Hit);

    FVector Origin = GetActorLocation() + Hit.ImpactNormal * 15;
    const TArray<AActor*> IgnoredActors;
    UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), MaxDamage, MinDamage, Origin,
                                                   DamageInnerRadius, DamageOuterRadius, DamageFalloff,
                                                   UDamageType::StaticClass(), IgnoredActors, this,
                                                   GetInstigatorController());
}
