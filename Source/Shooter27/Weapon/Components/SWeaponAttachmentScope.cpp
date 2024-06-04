// Shooter27


#include "Components/SWeaponAttachmentScope.h"
#include "SWeaponFirearm.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/KismetRenderingLibrary.h"

void USWeaponAttachmentScope::BeginPlay()
{
    Super::BeginPlay();
    if (GetOwnerRole() == ROLE_Authority)
        return;
    if (!ScopeOverlay)
        return;
    UTextureRenderTarget2D* RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(
        GetWorld(), WHRenderTarget, WHRenderTarget);
    USceneCaptureComponent2D* ScopeSceneCapture = GetOwner<ASWeaponFirearm>()->GetSceneCapture();
    ScopeSceneCapture->TextureTarget = RenderTarget;
    ScopeSceneCapture->FOVAngle = ScopeFOV;
    ScopeSceneCapture->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachSocket);
    UMaterialInstanceDynamic* DynamicScopeMat = CreateDynamicMaterialInstance(ScopeMatIndex);
    DynamicScopeMat->SetTextureParameterValue("ScopeRenderTexture", RenderTarget);
    DynamicScopeMat->SetTextureParameterValue("Overlay", ScopeOverlay);
}
