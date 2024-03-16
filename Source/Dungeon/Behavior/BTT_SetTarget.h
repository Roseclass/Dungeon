#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_SetTarget.generated.h"

/**
 * find target from Percepted Players, with condition
 */

class APlayerCharacter;
class AEnemy;
class UBlackBoardPlayerArrayObject;

UENUM(BlueprintType)
enum class ESetTargetOption : uint8
{
	Closest, Farthest, Random
};

UCLASS()
class DUNGEON_API UBTT_SetTarget : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_SetTarget();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	//property
private:
protected:
	UPROPERTY(EditAnywhere)
		ESetTargetOption Option;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector PerceptedPlayerObject;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector Target;
public:

	//function
private:
	APlayerCharacter* FindClosest(AEnemy* Origin, UBlackBoardPlayerArrayObject* Others);
	APlayerCharacter* FindFarthest(AEnemy* Origin, UBlackBoardPlayerArrayObject* Others);
	APlayerCharacter* FindRandom(AEnemy* Origin, UBlackBoardPlayerArrayObject* Others);
protected:
public:
};
