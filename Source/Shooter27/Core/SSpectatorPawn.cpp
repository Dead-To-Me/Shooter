// Shooter27


#include "SSpectatorPawn.h"

#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASSpectatorPawn::ASSpectatorPawn()
{
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    GetCollisionComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
    SpringArm->SetupAttachment(GetRootComponent());

    Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
    Camera->SetupAttachment(SpringArm);
    Camera->bUsePawnControlRotation = false;
}

// Called when the game starts or when spawned
void ASSpectatorPawn::BeginPlay()
{
    Super::BeginPlay();
    if (HasAuthority() || IsLocallyControlled())
    {
        if (GetInstigator() == this || !GetInstigator())
        {
            SetLerpActor();
            return;
        }
        GetWorldTimerManager().SetTimer(FollowKillerTimer, this, &ASSpectatorPawn::StopFollowKiller, FollowKillerTime);
    }
}

void ASSpectatorPawn::LerpToActor()
{
    if (!ToActor || !bEnableLerp)
        return;
    if (LerpTimeElapsed > LerpDuration)
    {
        bEnableLerp = false;
        return;
    }

    SetActorLocation(FMath::Lerp(SpawnLocation, ToActor->GetActorLocation(), LerpTimeElapsed / LerpDuration));
    Camera->SetWorldRotation(
        FMath::Lerp(SpawnRotation, ToActor->GetActorRotation(), LerpTimeElapsed / LerpDuration));
    LerpTimeElapsed += GetWorld()->GetDeltaSeconds();
}

void ASSpectatorPawn::StopFollowKiller()
{
    SetInstigator(nullptr);
    SetLerpActor();
    if (GetController())
        GetController<APlayerController>()->bShowMouseCursor = true;
}

void ASSpectatorPawn::SetLerpActor()
{
    SpringArm->TargetArmLength = 0;
    SpringArm->bUsePawnControlRotation = false;
    SpawnLocation = GetActorLocation();
    SpawnRotation = GetActorRotation();
    TArray<AActor*> OutActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraActor::StaticClass(), OutActors);
    if (!OutActors.IsValidIndex(0))
        return;
    if (!OutActors[0])
        return;
    ToActor = OutActors[0];
    bEnableLerp = true;
}

// Called every frame
void ASSpectatorPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (GetInstigator() == this || !GetInstigator())
    {
        LerpToActor();
        return;
    }
    SetActorLocation(GetInstigator()->GetActorLocation());
}

// Called to bind functionality to input
void ASSpectatorPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}
