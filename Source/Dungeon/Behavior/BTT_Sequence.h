#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_Sequence.generated.h"

/**
 * 
 */

UCLASS()
class DUNGEON_API UBTT_Sequence : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_Sequence();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
	//property
private:
protected:
	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector Sequence;
public:

	//function
private:
protected:
public:
};
