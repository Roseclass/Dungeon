#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_DialogSpeak.generated.h"

/**
 * 
 */

UCLASS()
class DUNGEON_API UBTT_DialogSpeak : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_DialogSpeak();
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
		FText Text;

	UPROPERTY(EditAnywhere)
		int32 NextPoint;
public:

	//function
private:
protected:
public:

};
