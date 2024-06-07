#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_MontageNotifyEvent.generated.h"

/**
 * 
 */

class UAnimMontage;
class UGameplayAbility;
class USkillComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMontageNotifyEventDelegate, FGameplayTag, EventTag, FGameplayEventData, EventData);

UCLASS()
class DUNGEON_API UAT_MontageNotifyEvent : public UAbilityTask
{
	GENERATED_BODY()
public:
	UAT_MontageNotifyEvent();
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAT_MontageNotifyEvent* CreateMontageNotifyEvent(UGameplayAbility* OwningAbility,FName TaskInstanceName,	UAnimMontage* MontageToPlay,FGameplayTagContainer EventTags,
		float Rate = 1.f,FName StartSection = NAME_None,bool bStopWhenAbilityEnds = true,float AnimRootMotionTranslationScale = 1.f);
protected:
public:
	virtual void Activate() override;
	virtual void ExternalCancel() override;
	virtual void OnDestroy(bool AbilityEnded) override;

	//property
private:
	UAnimMontage* MontageToPlay;			//Montage that is playing
	FGameplayTagContainer EventTags;		//List of tags to match against gameplay events
	float Rate;								//Playback rate
	FName StartSection;						//Section to start montage from
	float AnimRootMotionTranslationScale;	//Modifies how root motion movement to apply
	bool bStopWhenAbilityEnds;				//Rather montage should be aborted if ability ends
	FOnMontageBlendingOutStarted BlendingOutDelegate;
	FOnMontageEnded MontageEndedDelegate;
	FDelegateHandle CancelledHandle;
	FDelegateHandle EventHandle;
protected:
public:
	FMontageNotifyEventDelegate OnCompleted;	//The montage completely finished playing
	FMontageNotifyEventDelegate OnBlendOut;	//The montage started blending out
	FMontageNotifyEventDelegate OnInterrupted;//The montage was interrupted
	FMontageNotifyEventDelegate OnCancelled;	//The ability task was explicitly cancelled by another ability
	FMontageNotifyEventDelegate EventReceived;//One of the triggering gameplay events happened

	//function
private:
	USkillComponent* GetTargetSkillComp();
	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);
	void OnAbilityCancelled();
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload);
	bool StopPlayingMontage();	//Checks if the ability is playing a montage and stops that montage, returns true if a montage was stopped, false if not.
protected:
public:

};
