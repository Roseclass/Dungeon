#include "Abilities/GA_Skill.h"
#include "Global.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemGlobals.h"

#include "Components/SkillComponent.h"
#include "Abilities/AttributeSetBase.h"
#include "Characters/DungeonCharacterBase.h"
#include "Abilities/AbilityTaskTypes.h"
#include "Abilities/AT_MontageNotifyEvent.h"
#include "Abilities/AT_PersistentTask.h"
#include "Objects/DamageDealer.h"

UGA_Skill::UGA_Skill()
{
	CooldownTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Skill.Cooldown")));
}

void UGA_Skill::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UGA_Skill::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	FGameplayTagContainer enhancement;
	enhancement.AddTag(EnhancementTag);
	UAT_PersistentTask* Task = UAT_PersistentTask::CreatePersistentTask(this, NAME_None, enhancement);
	Task->EventReceived.AddDynamic(this, &UGA_Skill::OnEnhanced);
	Task->ReadyForActivation();
}

void UGA_Skill::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		int32 lv = GetAbilityLevel();
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), 1);
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(Cooldown.Tag, GetCooldown());
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}

const FGameplayTagContainer* UGA_Skill::GetCooldownTags() const
{
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

		float RequiredMana = GetCost();
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
		SpecHandle.Data->SetSetByCallerMagnitude(ManaCost.Tag, GetCost());
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
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

		FTransform transform = ch->GetMesh()->GetSocketTransform(SocketName);
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

void UGA_Skill::OnEnhanced(FGameplayTag EventTag, FGameplayEventData EventData)
{
	CheckFalse(GetOwningActorFromActorInfo()->GetLocalRole() == ROLE_Authority);
	const UPersistentTaskData* data = Cast<UPersistentTaskData>(EventData.OptionalObject);
	if (data)
	{
		for (auto i : data->Datas)
		{
			if (ManaCost.AdditiveTag == i.Key)ManaCost.Additive += i.Value;
			if (ManaCost.MultiplicitiveTag == i.Key)ManaCost.Multiplicitive -= i.Value;
			if (Cooldown.AdditiveTag == i.Key)Cooldown.Additive += i.Value;
			if (Cooldown.MultiplicitiveTag == i.Key)Cooldown.Multiplicitive -= i.Value;
		}
	}
}

float UGA_Skill::GetCooldown() const
{
	int32 lv = GetAbilityLevel();
	float base = Cooldown.Base.GetValueAtLevel(lv);
	float addtive = Cooldown.Additive;
	float multiplicitive = (Cooldown.Multiplicitive) * 0.01;
	float result = (base - addtive) * multiplicitive;

	if (result < base * 0.3)result = base * 0.3;

	return result;
}

float UGA_Skill::GetCost() const
{
	int32 lv = GetAbilityLevel();
	float base = ManaCost.Base.GetValueAtLevel(lv);
	float addtive = ManaCost.Additive;
	float multiplicitive = (ManaCost.Multiplicitive) * 0.01;
	float result = (base - addtive) * multiplicitive;

	if (result < 0)result = 0;

	return -result;
}
