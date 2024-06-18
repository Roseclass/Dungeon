#pragma once

#include "CoreMinimal.h"
#include "Abilities/GA_MontageWithEvent.h"
#include "GA_Skill.generated.h"

/**
 *
 */

class UAnimMontage;
class ADamageDealer;
class AWarningSign;

USTRUCT(BlueprintType)
struct FWaringSignData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		TSubclassOf<AWarningSign> WarningSignClass;

	UPROPERTY(EditAnywhere)
		float ForwardOffset = 0;

	UPROPERTY(EditAnywhere)
		float RightOffset = 0;

	UPROPERTY(EditAnywhere)
		FVector Scale = FVector(1, 1, 1);

	UPROPERTY(EditAnywhere)
		float Duration;

	UPROPERTY(EditAnywhere)
		float ExtraDuration;
};

USTRUCT(BlueprintType)
struct FMMCData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
		FGameplayTag Tag;

	UPROPERTY(EditDefaultsOnly)
		FGameplayTag BaseTag;

	UPROPERTY(EditDefaultsOnly)
		FGameplayTag AdditiveTag;

	UPROPERTY(EditDefaultsOnly)
		FGameplayTag MultiplicitiveTag;

	UPROPERTY(EditDefaultsOnly, meta = (DisplayName = "BaseCost"))
		FScalableFloat Base;
	float Additive;
	float Multiplicitive = 100;
};

UCLASS()
class DUNGEON_API UGA_Skill : public UGA_MontageWithEvent
{
	GENERATED_BODY()
public:
	UGA_Skill();
protected:
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)override;
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
	virtual const FGameplayTagContainer* GetCooldownTags() const override;
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

	//property
private:
protected:
	UPROPERTY(EditAnywhere, Category = "Dealer")
		TMap<FGameplayTag, TSubclassOf<ADamageDealer>> DealerClassMap;

	UPROPERTY(EditAnywhere, Category = "Dealer")
		FName SocketName;

	UPROPERTY(EditAnywhere, Category = "Dealer")
		bool bUseSocketLocation = 1;

	UPROPERTY(EditAnywhere, Category = "Dealer")
		bool bUseSocketRotation = 0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Damage")
		float BaseDamage = 10;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Damage")
		float AdditiveDamage = 10;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Damage")
		float BaseDamageRate = 1;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Damage")
		float AdditiveDamageRate = 1;

	UPROPERTY(EditAnywhere, Category = "Condition|Range")
		float Range = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Condition|Cost")
		FMMCData ManaCost;

	UPROPERTY(EditAnywhere, Category = "Condition|Cost")
		FMMCData Cooldown;

	UPROPERTY(EditAnywhere, Category = "WarningSign")
		TArray<FWaringSignData> WarningSignDatas;

	UPROPERTY(Transient)
		FGameplayTagContainer TempCooldownTags;
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tags, meta = (Categories = "AbilityTagCategory"))
		FGameplayTagContainer CooldownTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tags, meta = (Categories = "AbilityTagCategory"))
		FGameplayTag EnhancementTag;

	//function
private:
protected:
	virtual void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)override;
	virtual void OnEnhanced(FGameplayTag EventTag, FGameplayEventData EventData);

	virtual float GetCooldown()const;
	virtual float GetCost()const;
public:
};
