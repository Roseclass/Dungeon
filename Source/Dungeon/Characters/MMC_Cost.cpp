#include "Characters/MMC_Cost.h"
#include "Global.h"
#include "GameplayTagContainer.h"

float UMMC_Cost::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	float result = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Skill.Cost.Base")));
	float addtive = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Skill.Cost.Additive")));
	float multiplicitive = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Skill.Cost.Multiplicitive")));
	result = (result - addtive) * multiplicitive;
	if (result < 0)result = 0;
	return result;
}
