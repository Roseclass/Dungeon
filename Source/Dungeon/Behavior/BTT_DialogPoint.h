#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_DialogPoint.generated.h"

/**
 * 
 */

UCLASS()
class DUNGEON_API UBTT_DialogPoint : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_DialogPoint();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	//property
private:
protected:
	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector PlayerDatas;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector InteractingPlayer;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector Point;
public:

	//function
private:
protected:
public:

};
