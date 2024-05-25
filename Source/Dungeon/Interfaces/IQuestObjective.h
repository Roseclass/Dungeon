#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IQuestObjective.generated.h"

UINTERFACE(MinimalAPI)
class UIQuestObjective : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */

class UQuestComponent;

class DUNGEON_API IIQuestObjective
{
	GENERATED_BODY()
protected:
	UQuestComponent* LinkedComponent;
public:
	virtual void QuestObjective_Init(); // must be called from beginplay
	virtual void CompleteCondition() = 0;
};
