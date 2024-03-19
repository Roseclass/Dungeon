// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_ChangePhase.generated.h"

/**
 * 
 */

UCLASS()
class DUNGEON_API UBTT_ChangePhase : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_ChangePhase();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	//property
private:
protected:
	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector Phase;

	UPROPERTY(EditAnywhere)
		int32 NextPhase;
public:

	//function
private:
protected:
public:
};
