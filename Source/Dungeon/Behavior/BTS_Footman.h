#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_Footman.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEON_API UBTS_Footman : public UBTService
{
	GENERATED_BODY()
public:
	UBTS_Footman();
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
