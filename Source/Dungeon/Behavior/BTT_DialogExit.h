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
	virtual FString GetStaticDescription() const override;

	//property
private:
protected:
	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector PlayerDatas;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector InteractingPlayer;

	UPROPERTY(EditAnywhere)
		int32 NextPoint;
public:

	//function
private:
protected:
public:

};
