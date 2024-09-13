#pragma once

#include "CoreMinimal.h"
#include "Abilities/GA_Skill.h"
#include "GA_Teleport.generated.h"

/**
 * 
 */

UCLASS()
class DUNGEON_API UGA_Teleport : public UGA_Skill
{
	GENERATED_BODY()

public:
	UGA_Teleport();
protected:
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	//property
private:
	FHitResult UnderCursorHitResult;
protected:
public:

	//function
private:
	virtual void SpawnDamageDealer(FGameplayTag EventTag)override;
	virtual void SpawnWarningSign(FGameplayTag EventTag)override;
	virtual void OnCollision()override;
	virtual void OffCollision()override;
	virtual void ResetHitActors()override;
protected:
	virtual void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)override;
public:

};
