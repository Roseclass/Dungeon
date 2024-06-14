#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Delegates/DelegateCombinations.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Engine/DataTable.h"
#include "AbilityTaskTypes.generated.h"

/**
 * 
 */

//#include "Characters/AbilityTaskTypes.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAbilityTaskEventDelegate, FGameplayTag, EventTag, FGameplayEventData, EventData);

USTRUCT(BlueprintType)
struct FSkillEnhancement : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		FGameplayTag EnhanceEventTag;

	UPROPERTY(EditAnywhere)
		FGameplayTag EnhanceStatusTag;

	UPROPERTY(EditAnywhere)
		float EnhanceStatus;
};

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
