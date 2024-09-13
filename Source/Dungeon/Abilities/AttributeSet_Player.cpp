#include "Abilities/AttributeSet_Player.h"
#include "Global.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "GameplayEffectTypes.h"

UAttributeSet_Player::UAttributeSet_Player()
{
    InitLevel(1);
    InitExp(0);
    InitSkillPoint(100);
}

void UAttributeSet_Player::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);
}

void UAttributeSet_Player::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    /*
    * NOTE: PostGameplayEffectExecute()가 호출될 때 Attribute 변경은 이미 일어났지만,
    * 아직 클라이언트에 리플리케이트되지 않았으므로 여기에 값을 클램핑해도 클라이언트에 두 번의 네트워크 업데이트가 일어나지 않습니다.
    * 클라이언트는 클램핑 후에만 업데이트를 수신합니다.
    */

    Super::PostGameplayEffectExecute(Data);
}

void UAttributeSet_Player::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSet_Player, Level, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSet_Player, Exp, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSet_Player, SkillPoint, COND_None, REPNOTIFY_Always);
}

void UAttributeSet_Player::OnRep_Level(const FGameplayAttributeData& OldLevel)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSet_Player, Level, OldLevel);
}

void UAttributeSet_Player::OnRep_Exp(const FGameplayAttributeData& OldExp)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSet_Player, Exp, OldExp);
}

void UAttributeSet_Player::OnRep_SkillPoint(const FGameplayAttributeData& OldSkillPoint)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSet_Player, SkillPoint, OldSkillPoint);
}
