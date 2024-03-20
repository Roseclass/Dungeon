#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_DialogReply.generated.h"

/**
 * 
 */

class UBehaviorTreeComponent;

USTRUCT()
struct FReplyData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		int32 NextPoint;

	UPROPERTY(EditAnywhere)
		FText Reply;
};

UCLASS()
class DUNGEON_API UBTT_DialogReply : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_DialogReply();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	//property
private:
protected:
	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector DialogWidget;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector ReplyIndex;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector Point;

	UPROPERTY(EditAnywhere)
		TArray<FReplyData> Replies;
public:

	//function
private:
	UFUNCTION() void OnReplyFinished(int32 InReplyIndex, UBehaviorTreeComponent* OwnerComp);
protected:
public:
};