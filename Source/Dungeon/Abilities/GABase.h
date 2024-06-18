#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/GamePlayEnums.h"
#include "GABase.generated.h"

/**
 * for non-ui abilities
 */

UCLASS()
class DUNGEON_API UGABase : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UGABase();
protected:
public:

	//property
private:
protected:
public:

	//function
private: 
protected:
public:
	UFUNCTION(BlueprintImplementableEvent)float GetSkillCooldown()const;
	UFUNCTION(BlueprintImplementableEvent)FGameplayTagContainer GetSkillCooldownTags()const;
	UFUNCTION(BlueprintImplementableEvent)int32 GetSkillID()const;
};
