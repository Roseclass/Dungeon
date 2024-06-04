#include "Characters/GAUIBase.h"
#include "Global.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"

UGAUIBase::UGAUIBase()
{
	//// Default to Instance Per Actor
	//InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	//// Default tags that block this ability from activating
	//ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Dead")));
	//ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun")));
}

