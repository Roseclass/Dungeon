#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Delegates/DelegateCombinations.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilityTaskTypes.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAbilityTaskEventDelegate, FGameplayTag, EventTag, FGameplayEventData, EventData);

UCLASS()
class DUNGEON_API UPersistentTaskData : public UObject
{
	GENERATED_BODY()
	//property
private:
protected:
public:

	//function
private:
protected:
public:
	TArray<TTuple<FGameplayTag, float>> Datas;
};
