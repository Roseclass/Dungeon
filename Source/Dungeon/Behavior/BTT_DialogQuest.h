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

	//property
private:
protected:
	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector DialogWidget;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector Player;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector Point;

	UPROPERTY(EditAnywhere)
		bool bPoint;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bPoint", EditConditionHides))
		int32 PrevPoint;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bPoint", EditConditionHides))
		FText PrevPointText = FText::FromString("....");
public:

	//function
private:
	UFUNCTION() void OnQuestReplyFinished(UBehaviorTreeComponent* OwnerComp, UDialogReplyObject* InObject);
	UFUNCTION() void OnPointReplyFinished(UBehaviorTreeComponent* OwnerComp);
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
public:
};
