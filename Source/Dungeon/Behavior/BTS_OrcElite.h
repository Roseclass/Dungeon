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
public:

	//function
private:
protected:
public:

	//
	// Phase1
	// Phase2
	//
};
