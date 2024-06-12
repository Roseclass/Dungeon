#include "Characters/AT_PersistentTask.h"
#include "Global.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/Character.h"

#include "Components/SkillComponent.h"

UAT_PersistentTask::UAT_PersistentTask()
{

}

UAT_PersistentTask* UAT_PersistentTask::CreatePersistentTask(UGameplayAbility* OwningAbility, FName TaskInstanceName, FGameplayTagContainer EventTags)
{
	UAT_PersistentTask* result = NewAbilityTask<UAT_PersistentTask>(OwningAbility, TaskInstanceName);
	result->EventTags = EventTags;

	return result;
}

void UAT_PersistentTask::Activate()
{
	CheckNull(Ability);

	USkillComponent* skillComp = GetTargetSkillComp();

	if (skillComp)
	{
		const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
		EventHandle = skillComp->AddGameplayEventTagContainerDelegate(EventTags, FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UAT_PersistentTask::OnGameplayEvent));
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("UGDAbilityTask_PlayMontageAndWaitForEvent called on invalid AbilitySystemComponent"));

	SetWaitingOnAvatar();
}

void UAT_PersistentTask::OnDestroy(bool AbilityEnded)
{
	// Note: Clearing montage end delegate isn't necessary since its not a multicast and will be cleared when the next montage plays.
	// (If we are destroyed, it will detect this and not do anything)

	// This delegate, however, should be cleared as it is a multicast

	USkillComponent* skillComp = GetTargetSkillComp();
	if (skillComp)skillComp->RemoveGameplayEventTagContainerDelegate(EventTags, EventHandle);

	Super::OnDestroy(AbilityEnded);

}

USkillComponent* UAT_PersistentTask::GetTargetSkillComp()
{
	return Cast<USkillComponent>(AbilitySystemComponent);
}

void UAT_PersistentTask::OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	FGameplayEventData TempData = *Payload;
	TempData.EventTag = EventTag;
	
	EventReceived.Broadcast(EventTag, TempData);	
}
