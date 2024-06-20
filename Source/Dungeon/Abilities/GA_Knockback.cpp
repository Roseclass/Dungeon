#include "Abilities/GA_Knockback.h"
#include "Global.h"
#include "GameplayTagContainer.h"

#include "Abilities/AbilityTaskTypes.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"

#include "Characters/DungeonCharacterBase.h"

UGA_Knockback::UGA_Knockback()
{

}

void UGA_Knockback::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (TriggerEventData)
	{
		FVector direction = -TriggerEventData->ContextHandle.GetHitResult()->Normal;
		direction.Z = 0;

		RootMotionTask =
			UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce
			(this, NAME_None, direction, TriggerEventData->EventMagnitude, /*float Duration*/1, false, nullptr,
				ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity, FVector(), 0, 1);
		RootMotionTask->ReadyForActivation();
	}
	else CLog::Print(FString::Printf(TEXT("%s UGA_Knockback::ActivateAbility TriggerEventData is nullptr"), *ActorInfo->AvatarActor.Get()->GetName()), -1, 10, FColor::Red);
}

void UGA_Knockback::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	if (EventTag == EndTag)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		RootMotionTask = nullptr;
	}
	else if (EventTag == KnockbackEndTag)
	{
		RootMotionTask->EndTask();
	}
}
