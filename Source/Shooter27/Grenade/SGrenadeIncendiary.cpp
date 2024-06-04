// Genderfield


#include "SGrenadeIncendiary.h"
#include "GameFramework/ProjectileMovementComponent.h"

void ASGrenadeIncendiary::OnProjectileStop(const FHitResult& Hit)
{
    if (bExploded)
    {
        ProjectileMovement->OnProjectileStop.Clear();
        return;
    }
    ExplodeGrenade();
}

void ASGrenadeIncendiary::BeginPlay()
{
    Super::BeginPlay();
    ProjectileMovement->OnProjectileStop.AddDynamic(this, &ASGrenadeIncendiary::OnProjectileStop);
}
