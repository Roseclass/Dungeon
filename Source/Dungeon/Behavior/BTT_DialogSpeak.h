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
	//property
private:
protected:
	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector DialogWidget;

	UPROPERTY(EditAnywhere)
		FText Text;
public:

	//function
private:
	UFUNCTION() void OnSpeakFinished(UBehaviorTreeComponent* OwnerComp);
protected:
public:

};
