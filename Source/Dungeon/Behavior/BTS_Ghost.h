#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_Ghost.generated.h"

/**
 * 
 */

UCLASS()
class DUNGEON_API UBTS_Ghost : public UBTService
{
	GENERATED_BODY()
public:
	UBTS_Ghost();
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	//property
private:
protected:
	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector PerceptedPlayerObject;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector Target;

public:

	//function
private:
protected:
public:
};
