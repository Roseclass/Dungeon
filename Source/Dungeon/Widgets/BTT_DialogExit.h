#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_DialogExit.generated.h"

/**
 * 
 */

UCLASS()
class DUNGEON_API UBTT_DialogExit : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_DialogExit();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	//property
private:
protected:
	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector DialogWidget;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector Reward;

	UPROPERTY(EditAnywhere, Category = "ResetValue")
		bool bReward;
public:

	//function
private:
	void Reset(UBehaviorTreeComponent& OwnerComp);
protected:
public:

};
