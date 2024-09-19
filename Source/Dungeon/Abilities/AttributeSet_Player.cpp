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
    * NOTE: PostGameplayEffectExecute()�� ȣ��� �� Attribute ������ �̹� �Ͼ����,
    * ���� Ŭ���̾�Ʈ�� ���ø�����Ʈ���� �ʾ����Ƿ� ���⿡ ���� Ŭ�����ص� Ŭ���̾�Ʈ�� �� ���� ��Ʈ��ũ ������Ʈ�� �Ͼ�� �ʽ��ϴ�.
    * Ŭ���̾�Ʈ�� Ŭ���� �Ŀ��� ������Ʈ�� �����մϴ�.
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