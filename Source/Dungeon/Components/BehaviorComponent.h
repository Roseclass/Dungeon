#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BehaviorComponent.generated.h"

class UBlackboardComponent;
class ADungeonCharacter;

UENUM(BlueprintType)
enum class EBehaviorType : uint8
{
	Wait, Approach, Skill, Patrol, Hitted, Avoid, Strafing, MAX
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FBehaviorTypeChanged, EBehaviorType, EBehaviorType);

UCLASS(Blueprintable)
class DUNGEON_API UBlackBoardPlayerArrayObject : public UObject
{
	GENERATED_BODY()
	//property
private:
	TArray<ADungeonCharacter*> Players;
protected:
public:
	//function
private:
protected:
public:
	FORCEINLINE void AddPlayer(ADungeonCharacter* NewPlayer) { Players.Add(NewPlayer); };
	FORCEINLINE void RemovePlayer(ADungeonCharacter* NewPlayer) { Players.Remove(NewPlayer); };
	FORCEINLINE void SetPlayers(const TArray<ADungeonCharacter*>& NewPlayers) { Players = NewPlayers; };
	FORCEINLINE const TArray<ADungeonCharacter*>& GetPlayers()const { return Players; };
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEON_API UBehaviorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBehaviorComponent();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//property
private:
	UBlackboardComponent* Blackboard;
protected:
	UPROPERTY(EditAnywhere, Category = "BlackboardKey")
		FName BehaviorKey = "Behavior";

	UPROPERTY(EditAnywhere, Category = "BlackboardKey")
		FName TargetKey = "Target";

	UPROPERTY(EditAnywhere, Category = "BlackboardKey")
		FName PerceptedPlayersKey = "PerceptedPlayers";

	UPROPERTY(EditAnywhere, Category = "BlackboardKey")
		FName GoalLocationKey = "GoalLocation";

	UPROPERTY(EditAnywhere, Category = "BehaviorTree")
		float AvoidRange = 150.0f;

	UPROPERTY(EditAnywhere, Category = "BehaviorTree")
		float StrafingRange = 300.0f;

public:
	FBehaviorTypeChanged OnBehaviorTypeChanged;

	//function
private:
	void ChangeType(EBehaviorType InNewType);
	EBehaviorType GetType();
protected:
public:
	void SetWaitMode();
	void SetApproachMode();
	void SetPatrolMode();
	void SetHittedMode();
	void SetAvoidMode();
	void SetSkillMode();
	void SetStrafingMode();

	bool IsWaitMode();
	bool IsApproachMode();
	bool IsPatrolMode();
	bool IsHittedMode();
	bool IsAvoidMode();
	bool IsSkillMode();
	bool IsStrafingMode();

	ADungeonCharacter* GetTarget();
	FVector GetGoalLocation();

	float GetSkillRange(int32 InIndex);
	FORCEINLINE float GetAvoidRange() { return AvoidRange; }
	FORCEINLINE float GetStrafingRange() { return StrafingRange; }
	FORCEINLINE FName GetBehaviorKey() { return BehaviorKey; }
	FORCEINLINE FName GetTargetKey() { return TargetKey; }
	FORCEINLINE FName GetPerceptedPlayersKey() { return PerceptedPlayersKey; }
	FORCEINLINE FName GetGoalLocationKey() { return GoalLocationKey; }

	FORCEINLINE void SetBlackboard(class UBlackboardComponent* InBlackboard) { Blackboard = InBlackboard; }
};
