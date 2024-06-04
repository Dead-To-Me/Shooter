// Genderfield


#include "SGrenadeExplode.h"


ASGrenadeExplode::ASGrenadeExplode()
{
    MaxDamage = 110;
    MinDamage = 10;
    DamageInnerRadius = 150;
    DamageOuterRadius = 350;
    DamageFalloff = 1;
    TimeToExplode = 3;
}

void ASGrenadeExplode::SetExplodeGrenadeTimer()
{
    GetWorldTimerManager().SetTimer(ExplodeGrenadeTimer, this, &ASGrenade::ExplodeGrenade, TimeToExplode);
}

void ASGrenadeExplode::BeginPlay()
{
    Super::BeginPlay();
    if (!HasAuthority())
        return;
    SetExplodeGrenadeTimer();
}
