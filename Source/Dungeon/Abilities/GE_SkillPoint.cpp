#include "Abilities/GE_SkillPoint.h"
#include "Global.h"
#include "Abilities/AttributeSet_Player.h"

UGE_SkillPoint::UGE_SkillPoint()
{
    DurationPolicy = EGameplayEffectDurationType::Instant;

    FGameplayModifierInfo ModifierInfo;
    FSetByCallerFloat setByCaller;

    ModifierInfo.Attribute = UAttributeSet_Player::GetSkillPointAttribute();
    ModifierInfo.ModifierOp = EGameplayModOp::Additive;
    setByCaller.DataTag = FGameplayTag::RequestGameplayTag(FName("Effect.SkillPoint"));
    ModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(setByCaller);
    Modifiers.Add(ModifierInfo);
}
