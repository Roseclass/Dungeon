#include "Characters/GA_MontageWithEvent.h"
#include "Global.h"
#include "GameplayTagContainer.h"

#include "Components/SkillComponent.h"
#include "Characters/DungeonCharacterBase.h"
#include "Characters/AT_MontageNotifyEvent.h"
#include "Objects/DamageDealer.h"

UGA_MontageWithEvent::UGA_MontageWithEvent()
{

}
void UGA_MontageWithEvent::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

	// Play fire montage and wait for event telling us to spawn the projectile
	UAT_MontageNotifyEvent* Task = UAT_MontageNotifyEvent::CreateMontageNotifyEvent(this, NAME_None, Montage, FGameplayTagContainer(), 1.0f, NAME_None, false, 1.0f);
	Task->OnBlendOut.AddDynamic(this, &UGA_MontageWithEvent::OnCompleted);
	Task->OnCompleted.AddDynamic(this, &UGA_MontageWithEvent::OnCompleted);
	Task->OnInterrupted.AddDynamic(this, &UGA_MontageWithEvent::OnCancelled);
	Task->OnCancelled.AddDynamic(this, &UGA_MontageWithEvent::OnCancelled);
	Task->EventReceived.AddDynamic(this, &UGA_MontageWithEvent::EventReceived);
	// ReadyForActivation() is how you activate the AbilityTask in C++. Blueprint has magic from K2Node_LatentGameplayTaskCall that will automatically call ReadyForActivation().
	Task->ReadyForActivation();
}

void UGA_MontageWithEvent::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_MontageWithEvent::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_MontageWithEvent::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	// Montage told us to end the ability before the montage finished playing.
	// Montage was set to continue playing animation even after ability ends so this is okay.
	/*if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.Montage.EndAbility")))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}*/

	// Only spawn projectiles on the Server.
	// Predicting projectiles is an advanced topic not covered in this example.
	if (GetOwningActorFromActorInfo()->GetLocalRole() == ROLE_Authority && DealerClassMap.Contains(EventTag))
	{
		ADungeonCharacterBase* ch = Cast<ADungeonCharacterBase>(GetAvatarActorFromActorInfo());
		if (!ch)
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);


		// TODO:: 여기서 데미지 이펙트 만들어서 적용하기
		// 데미지 이펙트 클래스는 디폴트오브젝트에서 받아오기
		//FGameplayEffectSpecHandle DamageEffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageGameplayEffect, GetAbilityLevel());

		//// Pass the damage to the Damage Execution Calculation through a SetByCaller value on the GameplayEffectSpec
		//DamageEffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), Damage);

		FTransform transform = ch->GetMesh()->GetSocketTransform(FName("Muzzle"));
		transform.SetScale3D(FVector(1.0f));

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ADamageDealer* dealer = GetWorld()->SpawnActorDeferred<ADamageDealer>(DealerClassMap[EventTag], transform, GetOwningActorFromActorInfo(),
			ch, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		//dealer->DamageEffectSpecHandle = DamageEffectSpecHandle;
		//dealer->Range = Range;
		dealer->FinishSpawning(transform);
	}
}
