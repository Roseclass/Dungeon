#include "Abilities/GA_Dead.h"
#include "Global.h"
#include "GameplayTagContainer.h"

#include "Abilities/AbilityTaskTypes.h"
#include "Abilities/AT_MontageNotifyEvent.h"

#include "Characters/DungeonCharacterBase.h"

UGA_Dead::UGA_Dead()
{

}

void UGA_Dead::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ADungeonCharacterBase* ch = Cast<ADungeonCharacterBase>(CurrentActorInfo->AvatarActor.Get());
	if (ch)
		ch->Dead();
}
