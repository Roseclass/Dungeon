#include "Characters/GE_InstantDamage.h"
#include "Global.h"
#include "Characters/AttributeSetBase.h"

UGE_InstantDamage::UGE_InstantDamage()
{
    DurationPolicy = EGameplayEffectDurationType::Instant;

    FGameplayModifierInfo ModifierInfo;
    FSetByCallerFloat setByCaller;

    ModifierInfo.Attribute = UAttributeSetBase::GetHealthAttribute();
    ModifierInfo.ModifierOp = EGameplayModOp::Additive;
    setByCaller.DataTag = FGameplayTag::RequestGameplayTag(FName("Effect.Damage"));
    ModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(setByCaller);
    Modifiers.Add(ModifierInfo);
}
