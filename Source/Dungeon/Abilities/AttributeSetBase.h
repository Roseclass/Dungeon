#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AttributeSetBase.generated.h"

/**
 * 
 */

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class DUNGEON_API UAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()
public:
    UAttributeSetBase();
protected:
public:
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//property
private:
protected:
    UPROPERTY(BlueprintReadOnly, Category = "Status|Health", ReplicatedUsing = OnRep_Health)
        FGameplayAttributeData Health;

    UPROPERTY(BlueprintReadOnly, Category = "Status|Health", ReplicatedUsing = OnRep_MaxHealth)
        FGameplayAttributeData MaxHealth;

    UPROPERTY(BlueprintReadOnly, Category = "Status|Mana", ReplicatedUsing = OnRep_Mana)
        FGameplayAttributeData Mana;

    UPROPERTY(BlueprintReadOnly, Category = "Status|Mana", ReplicatedUsing = OnRep_MaxMana)
        FGameplayAttributeData MaxMana;

    UPROPERTY(BlueprintReadOnly, Category = "Status|Defense", ReplicatedUsing = OnRep_Defense)
        FGameplayAttributeData Defense;

    UPROPERTY(BlueprintReadOnly, Category = "Status|Defense", ReplicatedUsing = OnRep_AdditiveDefense)
        FGameplayAttributeData AdditiveDefense;

    UPROPERTY(BlueprintReadOnly, Category = "Status|Defense", ReplicatedUsing = OnRep_MultiplicitiveDefense)
        FGameplayAttributeData MultiplicitiveDefense;

    UPROPERTY(BlueprintReadOnly, Category = "Status|Power", ReplicatedUsing = OnRep_Power)
        FGameplayAttributeData Power;

    UPROPERTY(BlueprintReadOnly, Category = "Status|Power", ReplicatedUsing = OnRep_AdditivePower)
        FGameplayAttributeData AdditivePower;

    UPROPERTY(BlueprintReadOnly, Category = "Status|Power", ReplicatedUsing = OnRep_MultiplicitivePower)
        FGameplayAttributeData MultiplicitivePower;
public:

	//function
private:
protected:
    void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);

    UFUNCTION()virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);
    UFUNCTION()virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
    UFUNCTION()virtual void OnRep_Mana(const FGameplayAttributeData& OldMana);
    UFUNCTION()virtual void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana);
    UFUNCTION()virtual void OnRep_Defense(const FGameplayAttributeData& OldDefense);
    UFUNCTION()virtual void OnRep_AdditiveDefense(const FGameplayAttributeData& OldAdditiveDefense);
    UFUNCTION()virtual void OnRep_MultiplicitiveDefense(const FGameplayAttributeData& OldMultiplicitiveDefense);
    UFUNCTION()virtual void OnRep_Power(const FGameplayAttributeData& OldPower);
    UFUNCTION()virtual void OnRep_AdditivePower(const FGameplayAttributeData& OldAdditivePower);
    UFUNCTION()virtual void OnRep_MultiplicitivePower(const FGameplayAttributeData& OldMultiplicitivePower);
public:
    ATTRIBUTE_ACCESSORS(UAttributeSetBase, Health);
    ATTRIBUTE_ACCESSORS(UAttributeSetBase, MaxHealth);
    ATTRIBUTE_ACCESSORS(UAttributeSetBase, Mana);
    ATTRIBUTE_ACCESSORS(UAttributeSetBase, MaxMana);
    ATTRIBUTE_ACCESSORS(UAttributeSetBase, Defense);
    ATTRIBUTE_ACCESSORS(UAttributeSetBase, AdditiveDefense);
    ATTRIBUTE_ACCESSORS(UAttributeSetBase, MultiplicitiveDefense);
    ATTRIBUTE_ACCESSORS(UAttributeSetBase, Power);
    ATTRIBUTE_ACCESSORS(UAttributeSetBase, AdditivePower);
    ATTRIBUTE_ACCESSORS(UAttributeSetBase, MultiplicitivePower);
};
