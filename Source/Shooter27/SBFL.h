// Shooter27

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SBFL.generated.h"

class USoundAttenuation;
class USoundCue;
class UNiagaraSystem;
class ASPlayerState;

USTRUCT(BlueprintType)
struct FImpactData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TEnumAsByte<EPhysicalSurface> Surface;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UNiagaraSystem* ImpactParticle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UNiagaraSystem* DebrisParticle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USoundBase* ImpactSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UMaterial* ReadMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bLiquidSurface;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bColdSurface;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float LifeSpan = 30.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UMaterialInterface* DecalMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector DecalSize = FVector::Zero();

    FImpactData(): Surface(SurfaceType1), ImpactParticle(nullptr), DebrisParticle(nullptr),
                   ImpactSound(nullptr), ReadMaterial(nullptr), bLiquidSurface(false), bColdSurface(false), LifeSpan(0),
                   DecalMaterial(nullptr)
    {
    }
};


/**
 * 
 */
UCLASS()
class SHOOTER27_API USBFL : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Sorting")
    static TArray<ASPlayerState*>& SortPlayersArray(UPARAM(ref) TArray<ASPlayerState*>& Players);

    UFUNCTION(BlueprintCallable, Category="BulletEffect")
    static void BulletHitEffect(const UObject* WorldContextObject, const FHitResult& Hit,
                                const FImpactData& ImpactData);
};
