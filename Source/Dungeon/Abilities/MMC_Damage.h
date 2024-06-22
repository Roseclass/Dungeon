#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_Damage.generated.h"

/**
 * 
 */

UCLASS()
class DUNGEON_API UMMC_Damage : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
public:
	UMMC_Damage();
protected:
public:
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

	//property
private:
	FGameplayEffectAttributeCaptureDefinition DefenseDef;
protected:
public:

	//function
private:
protected:
public:
};
