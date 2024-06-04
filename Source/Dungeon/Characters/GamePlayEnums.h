#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EQuickSlotPosition : uint8
{
	Slot0, Slot1, Slot2, Slot3, Slot4, Slot5, Max
};

UENUM(BlueprintType)
enum class EGameplayAbilityInputID : uint8
{
	None			UMETA(DisplayName = "None"),
};

UENUM(BlueprintType)
enum class EGameplayAbility_Mage : uint8
{
	Frostbolt
};