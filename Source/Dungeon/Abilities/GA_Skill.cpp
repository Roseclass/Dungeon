#include "Abilities/GA_Skill.h"
#include "Global.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemGlobals.h"

#include "Components/SkillComponent.h"
#include "Abilities/AttributeSetBase.h"
#include "Characters/DungeonCharacterBase.h"
#include "Abilities/AbilityTaskTypes.h"
#include "Abilities/AT_MontageNotifyEvent.h"
#include "Objects/DamageDealer.h"
#include "Objects/WarningSign.h"

#include "Components/InventoryComponent.h"


UGA_Skill::UGA_Skill()
{
}

void UGA_Skill::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UGA_Skill::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		int32 lv = GetAbilityLevel();
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), 1);

		// set cooldown ge duration
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Skill.Cooldown"), GetCooldown(ActorInfo));

		// set DynamicGrantedTags
		TArray<FGameplayTag> tags;
		GetCooldownTags()->GetGameplayTagArray(tags);
		for (auto tag : tags)
			SpecHandle.Data.Get()->DynamicGrantedTags.AddTag(tag);

		// apply
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}

const FGameplayTagContainer* UGA_Skill::GetCooldownTags() const
{
	// get tags applied during cooldown
	FGameplayTagContainer* MutableTags = const_cast<FGameplayTagContainer*>(&TempCooldownTags);
	MutableTags->Reset(); // MutableTags writes to the TempCooldownTags on the CDO so clear it in case the ability cooldown tags change (moved to a different slot)
	const FGameplayTagContainer* ParentTags = Super::GetCooldownTags();
	if (ParentTags)
	{
		MutableTags->AppendTags(*ParentTags);
	}
	MutableTags->AppendTags(CooldownTags);
	return MutableTags;
}

bool UGA_Skill::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	UGameplayEffect* CostGE = GetCostGameplayEffect();
	if (CostGE)
	{
		USkillComponent* skillComp = Cast<USkillComponent>(ActorInfo->AbilitySystemComponent.Get());
		check(skillComp != nullptr);

		float RequiredMana = GetCost(ActorInfo);
		float CurrentMana = skillComp->GetNumericAttribute(UAttributeSetBase::GetManaAttribute());

		if (RequiredMana > CurrentMana)
		{
			const FGameplayTag& CostTag = UAbilitySystemGlobals::Get().ActivateFailCostTag;

			if (OptionalRelevantTags && CostTag.IsValid())
			{
				OptionalRelevantTags->AddTag(CostTag);
			}
			return false;
		}
	}
	return true;
}

void UGA_Skill::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CostGE = GetCostGameplayEffect();
	if (CostGE)
	{
		int32 lv = GetAbilityLevel();
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CostGE->GetClass(), lv);
		SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Skill.Cost"), GetCost(ActorInfo));
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}

void UGA_Skill::SpawnDamageDealer(FGameplayTag EventTag)
{
	ADungeonCharacterBase* ch = Cast<ADungeonCharacterBase>(GetAvatarActorFromActorInfo());
	if (!ch)
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	USkillComponent* skill = Cast<USkillComponent>(ch->GetAbilitySystemComponent());
	if (!skill)
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

	FTransform transform = ch->GetMesh()->GetSocketTransform(DamageDealerDataMap[EventTag].SocketName);
	transform.SetScale3D(FVector(1.0f));
	if (!DamageDealerDataMap[EventTag].bUseSocketLocation)
	{
		FVector loc = ch->GetActorLocation();
		loc += ch->GetActorForwardVector() * DamageDealerDataMap[EventTag].FrontDist;
		loc += ch->GetActorRightVector() * DamageDealerDataMap[EventTag].RightDist;
		loc += ch->GetActorUpVector() * DamageDealerDataMap[EventTag].UpDist;
		transform.SetTranslation(loc);
	}
	if (!DamageDealerDataMap[EventTag].bUseSocketRotation)
	{
		FRotator rot = ch->GetActorRotation();
		transform.SetRotation(FQuat4d(rot + DamageDealerDataMap[EventTag].Rotation));
	}

	//TODO::TEST
	FSkillEhancementData data = skill->GetEhancementData(AbilityTags.First());
	ADamageDealer* dealer = GetWorld()->SpawnActorDeferred<ADamageDealer>(DamageDealerDataMap[EventTag].Class, transform, GetOwningActorFromActorInfo(),
		ch, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	dealer->SetOwner(ch);
	dealer->SetTeamID(ch->GetGenericTeamId());
	dealer->SetDamageAdditive(data.DamageAdditive);
	dealer->SetDamageMultiplicitive(data.DamageMultiplicitive);
	dealer->Activate();
	dealer->FinishSpawning(transform);
}

void UGA_Skill::SpawnWarningSign(FGameplayTag EventTag)
{
	ADungeonCharacterBase* ch = Cast<ADungeonCharacterBase>(GetAvatarActorFromActorInfo());
	if (!ch)
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

	USkillComponent* skill = Cast<USkillComponent>(ch->GetAbilitySystemComponent());
	if (!skill)
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);

	FTransform transform;
	transform.SetScale3D(WarningSignDataMap[EventTag].Scale);
	{
		FVector loc = ch->GetActorLocation();
		loc += ch->GetActorForwardVector() * WarningSignDataMap[EventTag].ForwardOffset;
		loc += ch->GetActorRightVector() * WarningSignDataMap[EventTag].RightOffset;

		FHitResult result;
		GetWorld()->LineTraceSingleByObjectType(result, ch->GetActorLocation() + FVector(0, 0, 1000), ch->GetActorLocation() + FVector(0, 0, -1000),
			FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic));

		transform.SetTranslation(result.Location);
	}

	skill->Multicast_WarningSign(WarningSignDataMap[EventTag].WarningSignClass, transform, GetOwningActorFromActorInfo(),
		ch, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, WarningSignDataMap[EventTag].Duration, WarningSignDataMap[EventTag].ExtraDuration);
}

void UGA_Skill::OnCollision()
{
	UInventoryComponent* inv = CHelpers::GetComponent<UInventoryComponent>(GetOwningActorFromActorInfo());
	CheckNull(inv);
	inv->OnCollision(&DamageData);
}

void UGA_Skill::OffCollision()
{
	UInventoryComponent* inv = CHelpers::GetComponent<UInventoryComponent>(GetOwningActorFromActorInfo());
	CheckNull(inv);
	inv->OffCollision();
}

void UGA_Skill::ResetHitActors()
{
	UInventoryComponent* inv = CHelpers::GetComponent<UInventoryComponent>(GetOwningActorFromActorInfo());
	CheckNull(inv);
	inv->ResetHitActors();
}

void UGA_Skill::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	// Montage told us to end the ability before the montage finished playing.
	// Montage was set to continue playing animation even after ability ends so this is okay.
	if (EventTag == EndTag)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	// Only spawn projectiles on the Server.
	// Predicting projectiles is an advanced topic not covered in this example.
	if (GetOwningActorFromActorInfo()->GetLocalRole() == ROLE_Authority && DamageDealerDataMap.Contains(EventTag))
		SpawnDamageDealer(EventTag);

	if (GetOwningActorFromActorInfo()->GetLocalRole() == ROLE_Authority && WarningSignDataMap.Contains(EventTag))
		SpawnWarningSign(EventTag);

	if (GetOwningActorFromActorInfo()->GetLocalRole() == ROLE_Authority && EventTag == FGameplayTag::RequestGameplayTag(FName("Event.OnCollision")))
		OnCollision();

	else if (GetOwningActorFromActorInfo()->GetLocalRole() == ROLE_Authority && EventTag == FGameplayTag::RequestGameplayTag(FName("Event.OffCollision")))
		OffCollision();

	else if (GetOwningActorFromActorInfo()->GetLocalRole() == ROLE_Authority && EventTag == FGameplayTag::RequestGameplayTag(FName("Event.OffCollision")))
		OffCollision();
}

float UGA_Skill::GetCooldown(const FGameplayAbilityActorInfo* ActorInfo) const
{
	USkillComponent* skill = Cast<USkillComponent>(ActorInfo->AbilitySystemComponent);
	FSkillEhancementData data = skill->GetEhancementData(AbilityTags.First());
	int32 lv = GetAbilityLevel();
	float base = CooldownBase.GetValueAtLevel(lv);
	float addtive = data.CooldownAdditive;
	float multiplicitive = (data.CooldownMultiplicitive) * 0.01;
	float result = (base - addtive) * multiplicitive;

	if (result < base * 0.3)result = base * 0.3;

	return result;
}

float UGA_Skill::GetCost(const FGameplayAbilityActorInfo* ActorInfo) const
{
	USkillComponent* skill = Cast<USkillComponent>(ActorInfo->AbilitySystemComponent);
	FSkillEhancementData data = skill->GetEhancementData(AbilityTags.First());
	int32 lv = GetAbilityLevel();
	float base = CostBase.GetValueAtLevel(lv);
	float addtive = data.CostAdditive;
	float multiplicitive = (data.CostMultiplicitive) * 0.01;
	float result = (base - addtive) * multiplicitive;

	if (result < 0)result = 0;

	return -result;
}

/*
* �������,����,�����ÿ� �����Ƽ�� ��ȭ��ġ�� �����Ѵ�
* �⺻������ ����Ǿ���ϴ� ����Ʈ�� ��� �����Ұ���?
* 1.����,�����ÿ� ����Ʈ�� �����Ѵ�. (��)
* 2.���������ÿ� ĳ������ ��Ʈ����Ʈ�� ĸ���� ����Ѵ�.
* 2-1.�ǽð��������� �����÷������� ���Ŀ� ����
*/