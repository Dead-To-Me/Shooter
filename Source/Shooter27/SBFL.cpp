// Shooter27


#include "SBFL.h"
#include "NiagaraFunctionLibrary.h"
#include "SPlayerState.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"


TArray<ASPlayerState*>& USBFL::SortPlayersArray(TArray<ASPlayerState*>& Players)
{
    if (2 > Players.Num())
        return Players;

    for (int32 i = 1; i < Players.Num(); i++)
    {
        ASPlayerState* key = Players[i];
        int32 j = i - 1;
        while (j >= 0 && Players[j]->GetPoints() < key->GetPoints())
        {
            Players[j + 1] = Players[j];
            j--;
        }
        Players[j + 1] = key;
    }
    return Players;
}

void USBFL::BulletHitEffect(const UObject* WorldContextObject, const FHitResult& Hit, const FImpactData& ImpactData)
{
    UNiagaraFunctionLibrary::SpawnSystemAtLocation(WorldContextObject, ImpactData.ImpactParticle, Hit.ImpactPoint,
                                                   FRotationMatrix::MakeFromZ(Hit.ImpactNormal).Rotator());

    UGameplayStatics::PlaySoundAtLocation(WorldContextObject, ImpactData.ImpactSound, Hit.Location);

    UDecalComponent* Decal = UGameplayStatics::SpawnDecalAttached(ImpactData.DecalMaterial, ImpactData.DecalSize,
                                                                  Hit.GetComponent(), NAME_None, Hit.ImpactPoint,
                                                                  FRotationMatrix::MakeFromX(Hit.Normal).Rotator(),
                                                                  EAttachLocation::KeepWorldPosition,
                                                                  ImpactData.LifeSpan);
    if (Decal)
        Decal->SetFadeScreenSize(0.0f);

    if (ImpactData.bLiquidSurface)
        return;

    if (ImpactData.ReadMaterial)
        return; // TODO: RearMaterial
}
