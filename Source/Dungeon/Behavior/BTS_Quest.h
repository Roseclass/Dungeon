#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_Quest.generated.h"

/**
 * 
 */

class APlayerCharacter;

UCLASS()
class DUNGEON_API UBTS_Quest : public UBTService
{
	GENERATED_BODY()
public:
	UBTS_Quest();
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	//property
private:
protected:
	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector Data;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector InteractingPlayer;
public:

	//function
private:
protected:
public:
};
