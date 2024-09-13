#include "Components/SkillComponent.h"
#include "Global.h"
#include "GameFramework/CharacterMovementComponent.h"

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

	RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag("State.CannotMove")).AddUFunction(this, "OnCannotMoveTagChanged");

	CostBaseTag = FGameplayTag::RequestGameplayTag("Skill.Cost.Base");
	CostAdditiveTag = FGameplayTag::RequestGameplayTag("Skill.Cost.Additive");
	CostMultiplicitiveTag = FGameplayTag::RequestGameplayTag("Skill.Cost.Multiplicitive");

	CooldownBaseTag = FGameplayTag::RequestGameplayTag("Skill.Cooldown.Base");
	CooldownAdditiveTag = FGameplayTag::RequestGameplayTag("Skill.Cooldown.Additive");
	CooldownMultiplicitiveTag = FGameplayTag::RequestGameplayTag("Skill.Cooldown.Multiplicitive");
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

	DOREPLIFETIME(USkillComponent, EnhancementDatas);
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

	int32 max = 0;

	for (auto i : SkillDatas)
	{
		if (!i->SkillClass)continue;
		GiveAbility(FGameplayAbilitySpec(
			i->SkillClass,
			GetSkillLevel(i->SkillClass.GetDefaultObject()->GetSkillID()),
			i->SkillClass.GetDefaultObject()->GetSkillID(),
			this
		));
		max = UKismetMathLibrary::Max(max, i->SkillClass.GetDefaultObject()->GetSkillID());
	}

	for (int32 i = 0; i < max; i++)
		EnhancementDatas.AddSkillEnhancement(FSkillEhancementData());

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

void USkillComponent::OnCannotMoveTagChanged(FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 0)
	{
		ADungeonCharacterBase* owner = Cast<ADungeonCharacterBase>(GetOwner());
		if (owner)
			owner->GetCharacterMovement()->StopMovementImmediately();
	}
	else
	{
		// 태그가 제거되었을 때 호출할 함수
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
		{
			if (!spec->Ability->AbilityTags.HasTag(i.SkillTag))continue;
			if (!EnhancementDatas.Items.IsValidIndex(spec->InputID))continue;

			FSkillEhancementData& data = EnhancementDatas.Items[spec->InputID];

			if (CostAdditiveTag == i.EnhanceStatusTag)data.CostAdditive += i.EnhanceStatus * Rate;
			if (CostMultiplicitiveTag == i.EnhanceStatusTag)data.CostMultiplicitive -= i.EnhanceStatus * Rate;
			if (CooldownAdditiveTag == i.EnhanceStatusTag)data.CooldownAdditive += i.EnhanceStatus * Rate;
			if (CooldownMultiplicitiveTag == i.EnhanceStatusTag)data.CooldownMultiplicitive -= i.EnhanceStatus * Rate;
		}
	}
}

FSkillEhancementData USkillComponent::GetEhancementData(FGameplayTag AbilityTag)
{	
	FGameplayTagContainer tags;
	tags.AddTag(AbilityTag);
	TArray<FGameplayAbilitySpecHandle> arr;
	FindAllAbilitiesWithTags(arr, tags);

	if (arr.IsEmpty())return FSkillEhancementData();

	if (arr.Num() != 1)
		CLog::Print("USkillComponent::GetEhancementData arr.Num() != 1");

	FGameplayAbilitySpec* spec = FindAbilitySpecFromHandle(arr[0]);

	if (!spec)return FSkillEhancementData();

	return EnhancementDatas.Items.IsValidIndex(spec->InputID) ? EnhancementDatas.Items[spec->InputID] : FSkillEhancementData();
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
	context->StartServerTime = InOwner->GetWorld()->GetTimeSeconds();
	context->Duration = Duration;
	context->ExtraDuration = ExtraDuration;

	gameplayCueParameters.EffectContext = EffectContextHandle;

	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), FGameplayTag::RequestGameplayTag("GameplayCue.WarningSign"), EGameplayCueEvent::Type::Executed, gameplayCueParameters);
}

void USkillComponent::Multicast_FXEffect_Transform_Implementation(UNiagaraSystem* NiagaraFX, FTransform const& Transform)
{
	FGameplayCueParameters gameplayCueParameters;

	FNiagaraFXEffectContext* context = new FNiagaraFXEffectContext();
	FGameplayEffectContextHandle EffectContextHandle = FGameplayEffectContextHandle(context);
	context->FX = NiagaraFX;
	context->Transform = Transform;

	gameplayCueParameters.EffectContext = EffectContextHandle;

	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), FGameplayTag::RequestGameplayTag("GameplayCue.NiagaraFX"), EGameplayCueEvent::Type::Executed, gameplayCueParameters);	
}

void USkillComponent::Multicast_FXEffect_Attached_Implementation(UNiagaraSystem* NiagaraFX, AActor* AttachTarget)
{
	FGameplayCueParameters gameplayCueParameters;

	FNiagaraFXEffectContext* context = new FNiagaraFXEffectContext();
	FGameplayEffectContextHandle EffectContextHandle = FGameplayEffectContextHandle(context);
	context->FX = NiagaraFX;
	context->AttachTarget = AttachTarget;

	gameplayCueParameters.EffectContext = EffectContextHandle;

	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), FGameplayTag::RequestGameplayTag("GameplayCue.NiagaraFX"), EGameplayCueEvent::Type::Executed, gameplayCueParameters);
}

void USkillComponent::Server_Teleport_Implementation(const FHitResult& HitResult, const float MaxDist)
{
	float dist = MaxDist;

	//find direction
	FVector dir = HitResult.Location - GetOwner()->GetActorLocation();
	dir.Z = 0;
	dir = UKismetMathLibrary::Normal(dir);

	//clamp distance
	dist = UKismetMathLibrary::Min(dist, FVector3d::Dist2D(HitResult.Location, GetOwner()->GetActorLocation()));

	//make goal location
	FVector loc = GetOwner()->GetActorLocation();
	loc += (dir * dist);
	loc.Z = HitResult.Location.Z - 500;

	FHitResult hit;
	if (!GetWorld()->LineTraceSingleByChannel(hit, loc + FVector(0, 0, 1000), loc, ECC_Visibility))
	{
		hit.Location = loc;
		hit.Location.Z = HitResult.Location.Z;
	}

	//change location
	GetOwner()->SetActorLocation(hit.Location + FVector(0, 0, 96), 0, nullptr, ETeleportType::TeleportPhysics);
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
