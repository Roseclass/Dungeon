#include "Abilities/GA_Teleport.h"
#include "Global.h"
#include "GameplayTagContainer.h"

#include "DungeonPlayerController.h"

#include "Abilities/AbilityTaskTypes.h"
#include "Abilities/AT_MontageNotifyEvent.h"

#include "Characters/DungeonCharacterBase.h"
#include "Components/SkillComponent.h"
#include "Objects/TeleportAfterimage.h"

UGA_Teleport::UGA_Teleport()
{

}

void UGA_Teleport::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ADungeonCharacterBase* ch = Cast<ADungeonCharacterBase>(CurrentActorInfo->AvatarActor.Get());

	if (ch->IsLocallyControlled())
	{
		ADungeonPlayerController* controller = Cast<ADungeonPlayerController>(ch->GetController());
		if (controller)
			controller->GetHitResultUnderCursor(ECC_Visibility, 1, UnderCursorHitResult);
	}
}

void UGA_Teleport::SpawnDamageDealer(FGameplayTag EventTag)
{

}

void UGA_Teleport::SpawnWarningSign(FGameplayTag EventTag)
{

}

void UGA_Teleport::OnCollision()
{

}

void UGA_Teleport::OffCollision()
{

}

void UGA_Teleport::ResetHitActors()
{

}

void UGA_Teleport::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	// Montage told us to end the ability before the montage finished playing.
	// Montage was set to continue playing animation even after ability ends so this is okay.
	if (EventTag == EndTag)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	ADungeonCharacterBase* ch = Cast<ADungeonCharacterBase>(CurrentActorInfo->AvatarActor.Get());

	FTransform transform = ch->GetActorTransform();
	transform.SetLocation(transform.GetLocation() - FVector(0, 0, 96));

	ATeleportAfterimage* after = GetWorld()->SpawnActorDeferred<ATeleportAfterimage>(ATeleportAfterimage::StaticClass(), transform);
	UGameplayStatics::FinishSpawningActor(after, transform);
	after->Init(ch);

	if (ch->IsLocallyControlled())
	{
		USkillComponent* skill = Cast<USkillComponent>(ch->GetAbilitySystemComponent());
		if (skill)
			skill->Server_Teleport(UnderCursorHitResult, Range);
	}
}


/*
* 클릭(클라)
* 데이터 전송(클라->서버)
* 방향전환, clamp 거리 이동(서버)
*/