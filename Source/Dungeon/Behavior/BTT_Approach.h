#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BTT_Approach.generated.h"

/**
 * 
 */

UCLASS()
class DUNGEON_API UBTT_Approach : public UBTTask_MoveTo
{
	GENERATED_BODY()

public:
	UBTT_Approach();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	//property
private:
	float StartTime;
protected:
	UPROPERTY(EditAnywhere)
		float Duration = 10.0f;
public:

	//function
private:
protected:
public:
};
