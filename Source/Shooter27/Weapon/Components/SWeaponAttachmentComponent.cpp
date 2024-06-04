// Shooter27


#include "Components/SWeaponAttachmentComponent.h"
#include "SCharacter.h"
#include "SPlayerController.h"
#include "Camera/CameraComponent.h"

void USWeaponAttachmentComponent::CreateDynamicMaterials()
{
    TArray<UMaterialInterface*> Materials = GetMaterials();
    for (int i = 0; i < Materials.Num(); ++i)
    {
        OwnerWeaponAttachmentMats.AddUnique(CreateDynamicMaterialInstance(i));
    }
}

void USWeaponAttachmentComponent::OnFOVChangedHandle(float NewFOV)
{
    if (OwnerWeaponAttachmentMats.IsEmpty())
        CreateDynamicMaterials();
    for (auto Mat : OwnerWeaponAttachmentMats)
    {
        Mat->SetScalarParameterValue(FOVParamName, NewFOV);
    }
}

// Sets default values for this component's properties
USWeaponAttachmentComponent::USWeaponAttachmentComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}


// Called when the game starts
void USWeaponAttachmentComponent::BeginPlay()
{
    Super::BeginPlay();
    SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SetGenerateOverlapEvents(false);
    return;
    if (!GetOwner())
        return;
    if (GetOwnerRole() == ROLE_Authority)
        return;
    
    // Weapon->Character->Controller
    ASPlayerController* Controller = Cast<ASPlayerController>(GetOwner()->GetOwner()->GetOwner());
    if (!Controller)
        return;
    if (Controller->IsLocalController())
        return;
    Controller->OnFOVChanged.AddDynamic(this, &USWeaponAttachmentComponent::OnFOVChangedHandle);
    OnFOVChangedHandle(Controller->GetPawn<ASCharacter>()->GetCamera()->FieldOfView);
}
