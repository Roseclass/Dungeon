#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet_Player.generated.h"

/**
 * 
 */

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class DUNGEON_API UAttributeSet_Player : public UAttributeSet
{
	GENERATED_BODY()
public:
    UAttributeSet_Player();
protected:
public:
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    //property
private:
protected:
    UPROPERTY(BlueprintReadOnly, Category = "Player|Health", ReplicatedUsing = OnRep_Level)
        FGameplayAttributeData Level;

    UPROPERTY(BlueprintReadOnly, Category = "Player|Exp", ReplicatedUsing = OnRep_Exp)
        FGameplayAttributeData Exp;

    UPROPERTY(BlueprintReadOnly, Category = "Player|SkillPoint", ReplicatedUsing = OnRep_SkillPoint)
        FGameplayAttributeData SkillPoint;

public:

    //function
private:
protected:
    UFUNCTION()virtual void OnRep_Level(const FGameplayAttributeData& OldLevel);
    UFUNCTION()virtual void OnRep_Exp(const FGameplayAttributeData& OldExp);
    UFUNCTION()virtual void OnRep_SkillPoint(const FGameplayAttributeData& OldSkillPoint);
public:
    ATTRIBUTE_ACCESSORS(UAttributeSet_Player, Level);
    ATTRIBUTE_ACCESSORS(UAttributeSet_Player, Exp);
    ATTRIBUTE_ACCESSORS(UAttributeSet_Player, SkillPoint);
};
