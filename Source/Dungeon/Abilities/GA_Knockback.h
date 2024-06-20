#pragma once

#include "CoreMinimal.h"
#include "Abilities/GA_MontageWithEvent.h"
#include "GA_Knockback.generated.h"

/**
 *
 */

class UAbilityTask_ApplyRootMotionConstantForce;

UCLASS()
class DUNGEON_API UGA_Knockback : public UGA_MontageWithEvent
{
	GENERATED_BODY()

public:
	UGA_Knockback();
protected:
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	//property
private:
protected:
	UPROPERTY()UAbilityTask_ApplyRootMotionConstantForce* RootMotionTask;
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tags, meta = (Categories = "AbilityTagCategory"))
		FGameplayTag KnockbackEndTag;

	//function
private:
protected:
	virtual void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)override;
public:
};