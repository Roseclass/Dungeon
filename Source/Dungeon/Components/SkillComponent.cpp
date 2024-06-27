#include "Components/SkillComponent.h"
#include "Global.h"

#include "SaveManager.h"
#include "Characters/DungeonCharacterBase.h"

#include "AbilitySystemGlobals.h"
#include "GameplayCueManager.h"

#include "Abilities/AttributeSetBase.h"
#include "Abilities/GA_Skill.h"
#include "Abilities/GameplayEffectContexts.h"
#include "Components/SkillTreeComponent.h"

#define QUICKSLOT_EMPTY -100

USkillComponent::USkillComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	HitTags.AddTag(FGameplayTag::RequestGameplayTag("State.Hit"));
	HitTags.AddTag(FGameplayTag::RequestGameplayTag("State.Knockback"));
	HitTags.AddTag(FGameplayTag::RequestGameplayTag("State.Knockdown"));

	DeadTags.AddTag(FGameplayTag::RequestGameplayTag("State.Dead"));
}

void USkillComponent::BeginPlay()
{
	Super::BeginPlay();
	GiveDefaultAbilities();
}

void USkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USkillComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void USkillComponent::GiveDefaultAbilities()
{
	CheckNull(DataTable);
	DataTable->GetAllRows<FSkillData>("", SkillDatas);
	for (int32 i = 0; i < int32(EQuickSlotPosition::Max); ++i)
		QuickSlotData[i] = QUICKSLOT_EMPTY;

	CheckTrue(GetOwner()->GetLocalRole() != ROLE_Authority);

	for (auto i : DefaultGAs)
	{
		if (!i)continue;
		GiveAbility(FGameplayAbilitySpec(
			i,
			GetSkillLevel(i.GetDefaultObject()->GetSkillID()),
			i.GetDefaultObject()->GetSkillID(),
			this
		));
	}

	for (auto i : SkillDatas)
	{
		if (!i->SkillClass)continue;
		GiveAbility(FGameplayAbilitySpec(
			i->SkillClass,
			GetSkillLevel(i->SkillClass.GetDefaultObject()->GetSkillID()),
			i->SkillClass.GetDefaultObject()->GetSkillID(),
			this
		));
	}
	OnGameplayEffectAppliedDelegateToSelf.AddUFunction(this, "HitReaction");
}

void USkillComponent::HitReaction(UAbilitySystemComponent* InComponent, const FGameplayEffectSpec& InSpec, FActiveGameplayEffectHandle InHandle)
{
	CheckFalse(GetOwner()->HasAuthority());
	FGameplayTagContainer tags;InSpec.GetAllAssetTags(tags);
	for (auto tag : tags)
	{
		if (tag != FGameplayTag::RequestGameplayTag("Ability.Hit") &&
		tag != FGameplayTag::RequestGameplayTag("Ability.Knockback") &&
		tag != FGameplayTag::RequestGameplayTag("Ability.Knockdown"))
			continue;
		FGameplayTagContainer reactionTags;
		reactionTags.AddTag(tag);
		TArray<FGameplayAbilitySpecHandle> arr;
		FindAllAbilitiesWithTags(arr,reactionTags);
		for (auto i : arr)
		{
			FGameplayEventData* data = new FGameplayEventData();
			data->ContextHandle = InSpec.GetContext();
			InternalTryActivateAbility(i, FPredictionKey(), nullptr, nullptr, data);
		}
	}
}

void USkillComponent::UseSkill(int32 InSkillID)
{
	for (auto& i : ActivatableAbilities.Items)
		if (i.InputID == InSkillID)
		{
			//UEnum* temp = FindObject<UEnum>(ANY_PACKAGE, TEXT("EGameplayAbility_Mage"), true);
			//if (temp)CLog::Print(temp->GetNameStringByIndex(InSkillID));
			TryActivateAbility(i.Handle);
		}
}

void USkillComponent::UseQuickSlotSkill(int32 InQuickSlotIndex)
{
	CheckFalse(InQuickSlotIndex < int32(EQuickSlotPosition::Max));
	UseSkill(QuickSlotData[InQuickSlotIndex]);
}

void USkillComponent::ChangeQuickSlotData(int32 InQuickSlotIndex, int32 InSkillID)
{
	CheckFalse(InQuickSlotIndex < int32(EQuickSlotPosition::Max));
	QuickSlotData[InQuickSlotIndex] = InSkillID;

	// Is Empty
	if (QuickSlotData[InQuickSlotIndex]<0)
	{
		OnQuickSlotDataChanged.Broadcast(InQuickSlotIndex, FSkillData());
		return;
	}

	for (auto i : SkillDatas)
	{
		if (!i->SkillClass)continue;
		if (i->SkillClass.GetDefaultObject()->GetSkillID() == InSkillID)
		{
			OnQuickSlotDataChanged.Broadcast(InQuickSlotIndex, *i);
			break;
		}
	}

	for (int32 i = 0; i < int32(EQuickSlotPosition::Max); ++i)
	{
		if (i == InQuickSlotIndex)continue;
		if (QuickSlotData[i] == InSkillID)
			ChangeQuickSlotData(i, QUICKSLOT_EMPTY);
	}
}

void USkillComponent::SetCoolDown(int32 InSkillID)
{

}

bool USkillComponent::GetQuickSlotCoolDown(int32 InQuickSlotIndex,float& Result)
{
	return 1;
}

bool USkillComponent::IsQuickSlotCoolDown(int32 InQuickSlotIndex)
{
	return 1;
}

float USkillComponent::GetQuickSlotManaCost(int32 InQuickSlotIndex)
{
	return 1;
}

bool USkillComponent::GetSkillRange(int32 InSkillID, float& Range)
{
	return 1;
}

bool USkillComponent::GetQuickSlotSkillRange(int32 InQuickSlotIndex, float& Range)
{
	return 1;
}

void USkillComponent::EnhanceAbility(const TArray<FSkillEnhancement>& InDatas, float Rate)
{
	for (auto i : InDatas)
	{
		FGameplayTagContainer tags;
		tags.AddTag(i.SkillTag);
		TArray<FGameplayAbilitySpec*> arr;
		GetActivatableGameplayAbilitySpecsByAllMatchingTags(tags, arr);
		for (auto spec : arr)
			for (auto ability : spec->ReplicatedInstances)
			{
				UGA_Skill* skill = Cast<UGA_Skill>(ability);
				if (!skill)continue;
				skill->Enhance(i.EnhanceStatusTag, i.EnhanceStatus * Rate);
			}
	}
}

void USkillComponent::Cient_DamageText_Implementation(float InDamage, bool IsCritical, FVector InLocation)
{
	FGameplayCueParameters gameplayCueParameters;

	FDamageEffectContext* context = new FDamageEffectContext();
	FGameplayEffectContextHandle EffectContextHandle = FGameplayEffectContextHandle(context);
	context->CalculatedDamage = InDamage;

	gameplayCueParameters.EffectContext = EffectContextHandle;
	gameplayCueParameters.Location = InLocation;

	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), FGameplayTag::RequestGameplayTag("GameplayCue.DamageText"), EGameplayCueEvent::Type::Executed, gameplayCueParameters);
}

void USkillComponent::Multicast_WarningSign_Implementation(TSubclassOf<AWarningSign> Class, FTransform const& Transform, AActor* InOwner, APawn* Instigator, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride, float Duration, float ExtraDuration)
{
	FGameplayCueParameters gameplayCueParameters;

	FWarningSignEffectContext* context = new FWarningSignEffectContext();
	FGameplayEffectContextHandle EffectContextHandle = FGameplayEffectContextHandle(context);
	context->Class = Class;
	context->Transform = Transform;
	context->Owner = InOwner;
	context->AddInstigator(Instigator, nullptr);
	context->CollisionHandlingOverride = CollisionHandlingOverride;
	context->Duration = Duration;
	context->ExtraDuration = ExtraDuration;

	gameplayCueParameters.EffectContext = EffectContextHandle;

	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), FGameplayTag::RequestGameplayTag("GameplayCue.WarningSign"), EGameplayCueEvent::Type::Executed, gameplayCueParameters);
}

bool USkillComponent::CanUse() const
{
	return !HasAnyMatchingGameplayTags(BlockUseTags);
}

bool USkillComponent::CanMove() const
{
	return !HasAnyMatchingGameplayTags(BlockMoveTags);
}

bool USkillComponent::IsDead()const
{
	return HasAnyMatchingGameplayTags(DeadTags);
}

bool USkillComponent::IsHit()const
{
	return HasAnyMatchingGameplayTags(HitTags);
}

void USkillComponent::SaveData(USaveGameData* SaveData)
{
	// reset Datas
	//SaveData->PlayerData.SkillPoints;
	SaveData->PlayerData.SlotSkills.Empty();

	// save Datas
	for (int32 i : QuickSlotData)
		SaveData->PlayerData.SlotSkills.Add(i);
}

void USkillComponent::LoadData(USaveGameData* const ReadData)
{
	// load Datas
	for (int32 i = 0; i < ReadData->PlayerData.SlotSkills.Num(); ++i)
		ChangeQuickSlotData(i, ReadData->PlayerData.SlotSkills[i]);

	bLoad = 1;
}

int32 USkillComponent::GetSkillLevel(int32 InSkillID) const
{
	//if(!SkillState.Items.IsValidIndex(InSkillID))return 0;
	//return (int32)SkillState.Items[InSkillID].SkillState - (int32)ESkillTreeSkillState::Unlocked;
	return 1;
}

const TArray<const FSkillData*> USkillComponent::GetSkillDatas() const
{
	TArray<const FSkillData*> result;
	for (auto i : SkillDatas)
	{
		const FSkillData* temp = i;
		result.Add(temp);
	}
	return result;
}

const FSkillData* USkillComponent::GetSkillData(int32 InSkillID) const
{
	for (auto i : SkillDatas)
	{
		if (!i->SkillClass)continue;
		if (i->SkillClass.GetDefaultObject()->GetSkillID() == InSkillID)
			return i;
	}
	return nullptr;
}

float USkillComponent::GetDefense() const
{
	const UAttributeSetBase* attribute = Cast<UAttributeSetBase>(GetAttributeSet(UAttributeSetBase::StaticClass()));

	float defense = attribute->GetDefense();
	float additiveDefense = attribute->GetAdditiveDefense();
	float multiplicitiveDefense = attribute->GetMultiplicitiveDefense() * 0.01;
	return (defense + additiveDefense) * multiplicitiveDefense;
}

float USkillComponent::GetPower() const
{
	const UAttributeSetBase* attribute = Cast<UAttributeSetBase>(GetAttributeSet(UAttributeSetBase::StaticClass()));

	float power = attribute->GetPower();
	float additivePower = attribute->GetAdditivePower();
	float multiplicitivePower = attribute->GetMultiplicitivePower() * 0.01;
	return (power + additivePower) * multiplicitivePower;
}
