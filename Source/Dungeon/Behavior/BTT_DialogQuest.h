#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_DialogQuest.generated.h"

/**
 * 
 */

class UDialogReplyObject;
class UBehaviorTreeComponent;

UCLASS()
class DUNGEON_API UBTT_DialogQuest : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_DialogQuest();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	//property
private:
protected:
	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector InteractingPlayer;

public:

	//function
private:
protected:
public:
};
