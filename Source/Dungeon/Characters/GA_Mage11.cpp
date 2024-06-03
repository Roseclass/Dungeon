#include "Characters/GA_Mage11.h"
#include "Global.h"
#include "Animation/AnimInstance.h"
#include "AbilitySystemComponent.h"
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
		MulticastPlayMontage();
	}
}

void UGA_Mage11::MulticastPlayMontage_Implementation()
{
	ACharacter* Character = Cast<ACharacter>(GetOwningActorFromActorInfo());
	if (Character && MontageToPlay)
	{
		UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(MontageToPlay);
		}
	}
}
