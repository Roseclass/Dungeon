#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_DialogDistinguish.generated.h"

/**
 * 
 */

class UDialogReplyObject;
class UBehaviorTreeComponent;

UCLASS()
class DUNGEON_API UBTT_DialogDistinguish : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_DialogDistinguish();
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

	UPROPERTY(EditAnywhere, Category = "Sever")
		int32 Server_NextPoint;

	UPROPERTY(EditAnywhere, Category = "Sever")
		FText Server_Text;

	UPROPERTY(EditAnywhere, Category = "Client")
		int32 Client_NextPoint;

	UPROPERTY(EditAnywhere, Category = "Client")
		FText Client_Text;

public:

	//function
private:
protected:
public:
};
