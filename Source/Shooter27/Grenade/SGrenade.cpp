// Shooter27


#include "SGrenade.h"

#include "Components/WidgetComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASGrenade::ASGrenade()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
    bExploded = false;

    GrenadeMesh = CreateDefaultSubobject<UStaticMeshComponent>("Grenade");
    SetRootComponent(GrenadeMesh);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
    ProjectileMovement->bShouldBounce = true;
    ProjectileMovement->Bounciness = 0.3f;
    ProjectileMovement->Friction = 0.4f;
    ProjectileMovement->InitialSpeed = 2400.f;

    GrenadeWidget = CreateDefaultSubobject<UWidgetComponent>("GrenadeWidget");
    GrenadeWidget->SetupAttachment(GetRootComponent());
}

void ASGrenade::ExplodeGrenade_Implementation()
{
    bExploded = true;
    SetLifeSpan(1);
    GrenadeMesh->SetVisibility(false);
}

void ASGrenade::AnyDamageHandle(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                AController* InstigatedBy, AActor* DamageCauser)
{
    if (bExploded)
    {
        OnTakeAnyDamage.Clear();
        return;
    }
    ExplodeGrenade();
}

// Called when the game starts or when spawned
void ASGrenade::BeginPlay()
{
    Super::BeginPlay();
    OnTakeAnyDamage.AddDynamic(this, &ASGrenade::AnyDamageHandle);
}
