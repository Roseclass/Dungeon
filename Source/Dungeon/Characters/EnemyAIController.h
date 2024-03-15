#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class AEnemy;
class UBehaviorComponent;

UCLASS()
class DUNGEON_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
public:
	AEnemyAIController();
protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
public:
	virtual void Tick(float DeltaTime) override;

	//property
private:
	UPROPERTY(VisibleDefaultsOnly)
		UBehaviorComponent* Behavior;

	UPROPERTY(VisibleDefaultsOnly)
		UAIPerceptionComponent* Perception;

	AEnemy* OwnerEnemy;
	UAISenseConfig_Sight* Sight;

protected:
public:

	//function
private:
	UFUNCTION()void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);
protected:
public:
	float GetSightRadius();

	void StopLogic(FString Reason);
	void RestartLogic();

};
