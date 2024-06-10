#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_Cost.generated.h"

/**
 * 
 */

UCLASS()
class DUNGEON_API UMMC_Cost : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
public:
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
