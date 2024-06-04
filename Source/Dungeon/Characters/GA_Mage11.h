#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Mage11.generated.h"

/**
 * 
 */

UCLASS()
class DUNGEON_API UGA_Mage11 : public UGameplayAbility
{
	GENERATED_BODY()
public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly, Category = MontageAbility)
		UAnimMontage* MontageToPlay;

	UPROPERTY(EditDefaultsOnly, Category = MontageAbility)
		float	PlayRate;

	UPROPERTY(EditDefaultsOnly, Category = MontageAbility)
		FName	SectionName;

	void PlayMontage();
};
