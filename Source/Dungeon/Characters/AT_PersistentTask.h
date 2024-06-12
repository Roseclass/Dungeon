#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Characters/AbilityTaskTypes.h"
#include "AT_PersistentTask.generated.h"

/**
 * 
 */

class UAnimMontage;
class UGameplayAbility;
class USkillComponent;

UCLASS()
class DUNGEON_API UAT_PersistentTask : public UAbilityTask
{
	GENERATED_BODY()
public:
	UAT_PersistentTask();
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
		static UAT_PersistentTask* CreatePersistentTask(UGameplayAbility* OwningAbility, FName TaskInstanceName, FGameplayTagContainer EventTags);
protected:
public:
	virtual void Activate() override;
	virtual void OnDestroy(bool AbilityEnded) override;

	//property
private:
	FGameplayTagContainer EventTags;		//List of tags to match against gameplay events
	FDelegateHandle EventHandle;
protected:
public:
	FAbilityTaskEventDelegate EventReceived;//One of the triggering gameplay events happened

	//function
private:
	USkillComponent* GetTargetSkillComp();
	void OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload);
protected:
public:


};
