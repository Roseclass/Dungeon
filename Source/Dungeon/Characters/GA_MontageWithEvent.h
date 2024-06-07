#pragma once

#include "CoreMinimal.h"
#include "Characters/GABase.h"
#include "GA_MontageWithEvent.generated.h"

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

UCLASS()
class DUNGEON_API UGA_MontageWithEvent : public UGABase
{
	GENERATED_BODY()
public:
	UGA_MontageWithEvent();
protected:
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	//property
private:
protected:
	UPROPERTY(EditAnywhere, Category = "Montage")
		UAnimMontage* Montage;

	UPROPERTY(EditAnywhere, Category = "Montage")
		float PlayRate = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Montage")
		FName StartSection;

	UPROPERTY(EditAnywhere, Category = "Montage")
		FName RepeatSection;

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

	UPROPERTY(EditAnywhere, Category = "CoolDown")
		float CoolDown = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Range")
		float Range = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Condition")
		float ManaCost = 10.0f;

	UPROPERTY(EditAnywhere, Category = "WarningSign")
		TArray<FWaringSignData> WarningSignDatas;
public:

	//function
private:
protected:
	UFUNCTION()virtual void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);
	UFUNCTION()virtual void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);
	UFUNCTION()virtual void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData);
public:
};
