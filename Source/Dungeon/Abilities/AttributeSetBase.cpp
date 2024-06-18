#include "Abilities/AttributeSetBase.h"
#include "Global.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "GameplayEffectTypes.h"

UAttributeSetBase::UAttributeSetBase()
{
    InitHealth(100);
    InitMaxHealth(100);
    InitMana(100);
    InitMaxMana(100);
}

void UAttributeSetBase::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

    // If a Max value changes, adjust current to keep Current % of Current to Max
    if (Attribute == GetMaxHealthAttribute())
    {
        AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
    }
    else if (Attribute == GetMaxManaAttribute())
    {
        AdjustAttributeForMaxChange(Mana, MaxMana, NewValue, GetManaAttribute());
    }
}

void UAttributeSetBase::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    /*
    * NOTE: PostGameplayEffectExecute()�� ȣ��� �� Attribute ������ �̹� �Ͼ����,
    * ���� Ŭ���̾�Ʈ�� ���ø�����Ʈ���� �ʾ����Ƿ� ���⿡ ���� Ŭ�����ص� Ŭ���̾�Ʈ�� �� ���� ��Ʈ��ũ ������Ʈ�� �Ͼ�� �ʽ��ϴ�.
    * Ŭ���̾�Ʈ�� Ŭ���� �Ŀ��� ������Ʈ�� �����մϴ�.
    */

	Super::PostGameplayEffectExecute(Data);

    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
    }
    else if (Data.EvaluatedData.Attribute == GetManaAttribute())
    {
        SetMana(FMath::Clamp(GetMana(), 0.0f, GetMaxMana()));
    }
}

void UAttributeSetBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetBase, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetBase, MaxHealth, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetBase, Mana, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetBase, MaxMana, COND_None, REPNOTIFY_Always);
}

void UAttributeSetBase::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
    UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
    const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
    if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
    {
        // Change current value to maintain the current Val / Max percent
        const float CurrentValue = AffectedAttribute.GetCurrentValue();
        float NewDelta = (CurrentMaxValue > 0.f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;

        AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
    }
}

void UAttributeSetBase::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetBase, Health, OldHealth);
}

void UAttributeSetBase::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetBase, MaxHealth, OldMaxHealth);
}

void UAttributeSetBase::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetBase, Mana, OldMana);
}

void UAttributeSetBase::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetBase, MaxMana, OldMaxMana);
}

