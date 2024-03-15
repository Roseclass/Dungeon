#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_Skill.generated.h"

/**
 * 
 */

UCLASS()
class DUNGEON_API UBTT_Skill : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_Skill();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	//property
private:
protected:
	UPROPERTY(EditAnywhere)
		int32 SkillIndex;
public:

	//function
private:
protected:
public:

};
