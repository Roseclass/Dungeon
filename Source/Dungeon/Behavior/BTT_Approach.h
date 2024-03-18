#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BTT_Approach.generated.h"

/**
 * approach to target or location
 * duration over? return success
 */

UENUM(BlueprintType)
enum class ESuccessRange : uint8
{
	Fixed, Skill
};

UCLASS()
class DUNGEON_API UBTT_Approach : public UBTTask_MoveTo
{
	GENERATED_BODY()

public:
	UBTT_Approach();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type PerformMoveTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	//property
private:
protected:
	UPROPERTY(EditAnywhere)
		float Duration = 10.0f;

	UPROPERTY(EditAnywhere)
		ESuccessRange RangeType;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "RangeType == ESuccessRange::Skill", EditConditionHides))
		int32 SkillIndex;

public:

	//function
private:
protected:
public:
};
