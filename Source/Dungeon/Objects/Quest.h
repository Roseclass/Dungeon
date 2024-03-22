#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Quest.generated.h"

class AEnemy;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FQuestCompleted, int32, RootNumber, int32, NodeNumber);

UENUM(BlueprintType)
enum class EQuestConditionType : uint8
{
	DestroyingEnemy,
	Survive,
	Max
};

UENUM(BlueprintType)
enum class EQuestObjectiveState : uint8
{
	Main,
	Additive,
	Max
};

USTRUCT(BlueprintType)
struct FQuestCondition
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
		FString Summary;

	UPROPERTY(EditDefaultsOnly)
		EQuestConditionType Type;

	int32 CurrentCount;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "Type == EQuestConditionType::DestroyingEnemy", EditConditionHides))
		int32 TargetCount;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "Type == EQuestConditionType::DestroyingEnemy", EditConditionHides))
		TArray<TSubclassOf<AEnemy>> TargetClasses;

	int32 SurvivalDuration;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "Type == EQuestConditionType::Survive", EditConditionHides))
		int32 TargetTimeToSurvive;
};

UCLASS()
class UQuest_Objective : public UObject
{
	GENERATED_BODY()
	//property
private:
	TArray<FQuestCondition> QuestConditions;
	EQuestObjectiveState State;
protected:
public:

	//function
private:
protected:
public:
	FORCEINLINE void SetQuestConditions(const TArray<FQuestCondition>& InArray) { QuestConditions = InArray; }

	FORCEINLINE const TArray<FQuestCondition>& GetQuestConditions() const { return QuestConditions; }
	FORCEINLINE EQuestObjectiveState GetState() const { return State; }
};

UCLASS()
class DUNGEON_API AQuest : public AActor
{
	GENERATED_BODY()
	
public:	
	AQuest();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;

	//property
private:
	UPROPERTY()UQuest_Objective* MainQuestOjbective;
	UPROPERTY()UQuest_Objective* AdditiveQuestOjbective;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Datas")
		FText Name;

	UPROPERTY(EditDefaultsOnly, Category = "Datas")
		FText Description;

	UPROPERTY(EditDefaultsOnly, Category = "Datas")
		FName StreamingLevelName;
	
	UPROPERTY(EditDefaultsOnly, Category = "Datas")
		TArray<FQuestCondition> MainConditions;

	UPROPERTY(EditDefaultsOnly, Category = "Datas")
		TArray<FQuestCondition> AdditiveConditions;

public:
	FQuestCompleted OnCompleted;
	//function
private:
	UFUNCTION() void OnQuestDestroyed(AActor* DestroyedActor);
protected:
public:
	FORCEINLINE UQuest_Objective* GetMainQuestOjbective() const { return MainQuestOjbective; }
	FORCEINLINE UQuest_Objective* GetAdditiveQuestOjbective() const { return AdditiveQuestOjbective; }
};
