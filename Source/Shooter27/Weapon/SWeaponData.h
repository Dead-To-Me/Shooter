// Shooter27

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SWeaponData.generated.h"

class USWeaponAttachmentLaser;
class ASBulletBase;
class USWeaponAttachmentMagComponent;
class USWeaponAttachmentScope;
class ASWeaponFirearm;
class ASGrenade;
class ASWeapon;

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    Rifle,
    SMG,
    LMG,
    Sniper,
    Shotgun,
    Handgun,
    GrenadeLauncher
};

UENUM(BlueprintType)
enum class EInventorySlot : uint8
{
    Primary,
    Secondary,
    Gadget,
    Gadget2,
    Grenade,
    Melee,
    MAX
};

UENUM(BlueprintType)
enum class EWeaponAttachmentSlot : uint8
{
    Scope,
    Magazine,
    Muzzle,
    Grip,
    Laser
};

USTRUCT(BlueprintType)
struct FInventorySlots
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<ASWeaponFirearm> PrimaryWeapon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<ASWeaponFirearm> SecondaryWeapon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<ASWeapon> Melee;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<ASWeapon> Gadget;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<ASWeapon> Gadget2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<ASGrenade> Grenade;

    FInventorySlots() : PrimaryWeapon(nullptr), SecondaryWeapon(nullptr), Melee(nullptr), Gadget(nullptr),
                        Gadget2(nullptr), Grenade(nullptr)
    {
    }
};

USTRUCT(BlueprintType)
struct FWeaponAttachments
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<USWeaponAttachmentScope> ScopeClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<USWeaponAttachmentMagComponent> MagClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<USWeaponAttachmentLaser> Laser;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<ASBulletBase> BulletClass;
};

/**
 * 
 */
UCLASS()
class SHOOTER27_API USWeaponData : public UObject
{
    GENERATED_BODY()
};
