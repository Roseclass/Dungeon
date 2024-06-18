#include "Abilities/GA_Mage11.h"
#include "Global.h"
#include "Animation/AnimInstance.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"

void UGA_Mage11::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		return;
	}

	if (HasAuthority(&ActivationInfo) && MontageToPlay)
	{
		PlayMontage();
	}
}

void UGA_Mage11::PlayMontage()
{
	IAbilitySystemInterface* Character = Cast<IAbilitySystemInterface>(GetOwningActorFromActorInfo());
	if (Character && MontageToPlay)
	{
		Character->GetAbilitySystemComponent()->PlayMontage(this, GetCurrentActivationInfo(), MontageToPlay, PlayRate);
	}
}
