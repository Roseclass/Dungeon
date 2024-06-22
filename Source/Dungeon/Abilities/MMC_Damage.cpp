#include "Abilities/MMC_Damage.h"
#include "Global.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"

#include "Abilities/AttributeSetBase.h"

UMMC_Damage::UMMC_Damage()
{
    //DefenseDef.AttributeToCapture = UAttributeSetBase::GetDefenseAttribute();
    //DefenseDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
    //DefenseDef.bSnapshot = false;

    RelevantAttributesToCapture.Add(DefenseDef);
}

float UMMC_Damage::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
    // Gather the tags from the source and target as that can affect which buffs should be used
    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

    FAggregatorEvaluateParameters EvaluationParameters;
    EvaluationParameters.SourceTags = SourceTags;
    EvaluationParameters.TargetTags = TargetTags;

    float defense = 0.f;
    GetCapturedAttributeMagnitude(DefenseDef, Spec, EvaluationParameters, defense);
    defense = FMath::Max<float>(defense, 0.0f);

    float result = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Effect.Damage")));
    result /= (1 + defense);

    CLog::Print(result);

    return result;
}
