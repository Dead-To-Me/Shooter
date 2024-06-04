// Shooter27


#include "SBulletBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASBulletBase::ASBulletBase()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    bReplicates = true;
    SetReplicatingMovement(true);
    
    BoxCollision = CreateDefaultSubobject<UBoxComponent>("BoxCollision");
    SetRootComponent(BoxCollision);
    BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BoxCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
    BoxCollision->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
    BoxCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);

    BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>("BulletMesh");
    BulletMesh->SetupAttachment(BoxCollision);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
    ProjectileMovement->bRotationFollowsVelocity = true;
}

// Called when the game starts or when spawned
void ASBulletBase::BeginPlay()
{
    Super::BeginPlay();
    CrossSectionalArea = FMath::Pow(CrossSectionalArea / 1000, 2) * UE_DOUBLE_PI;
    SetLifeSpan(30.0f);
}

void ASBulletBase::TurnOffCollision()
{
    bTookDamage = true;
    BoxCollision->OnComponentHit.Clear();
    BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    BulletMesh->SetVisibility(false);
    SetLifeSpan(1);
}

void ASBulletBase::BulletHitCosmetic(const FHitResult& Hit)
{
}

void ASBulletBase::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (ProjectileMovement->Velocity.Length() <= 0)
        return;
    FVector AirForce = -GetActorForwardVector();
    AirForce *= DragCoefficient * 1.29 * FMath::Pow(ProjectileMovement->Velocity.Length() / 100, 2) * CrossSectionalArea
        / 2;
    AirForce *= Drag;

    ProjectileMovement->AddForce(AirForce * GetWorld()->GetDeltaSeconds());
    //UE_LOG(LogTemp, Display, TEXT("%f"), ProjectileMovement->Velocity.Length() / 100);
}
