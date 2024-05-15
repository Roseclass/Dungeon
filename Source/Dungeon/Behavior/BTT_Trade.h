#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_Trade.generated.h"

/**
 * 
 */

UCLASS()
class DUNGEON_API UBTT_Trade : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_Trade();
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
