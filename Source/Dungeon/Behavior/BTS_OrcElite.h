#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_OrcElite.generated.h"

/**
 * 
 */

UCLASS()
class DUNGEON_API UBTS_OrcElite : public UBTService
{
	GENERATED_BODY()
public:
	UBTS_OrcElite();
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	//property
private:
protected:
	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector PerceptedPlayerObject;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector Target;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector Phase;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector Sequence;
public:

	//function
private:
	int32 CheckPhase(float HealthRate, int32 PrevPhase);
protected:
public:

	//
	// Phase0
	// whirlwind
	// Phase1 (health rate under 0.5)
	// start sequence, rolling, actor time dilation 1.5
	// common
	// combo 1 3 5
	//
};
