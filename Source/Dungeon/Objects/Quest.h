#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Quest.generated.h"

class AEnemy;
class IIQuestObjective;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FQuestCompleted, int32, RootNumber, int32, NodeNumber);

UENUM(BlueprintType)
enum class EQuestConditionType : uint8
{
	DestroyingEnemy,
	Survive,
	Interact,
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
private:
	UPROPERTY(EditDefaultsOnly)
		FString Summary;

	UPROPERTY(EditDefaultsOnly)
		EQuestConditionType Type;

	// EQuestConditionType::DestroyingEnemy
	int32 CurrentEnemyCount;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "Type == EQuestConditionType::DestroyingEnemy", EditConditionHides))
		int32 TargetEnemyCount;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "Type == EQuestConditionType::DestroyingEnemy", EditConditionHides))
		TArray<TSubclassOf<IIQuestObjective>> TargetEnemyClasses;

	// EQuestConditionType::Survive
	int32 SurvivalDuration;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "Type == EQuestConditionType::Survive", EditConditionHides))
		int32 TargetTimeToSurvive;

	// EQuestConditionType::Interact
	int32 CurrentInteractCount;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "Type == EQuestConditionType::Interact", EditConditionHides))
		int32 TargetInteractCount;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "Type == EQuestConditionType::Interact", EditConditionHides))
		TArray<TSubclassOf<IIQuestObjective>> TargetInteractClasses;
public:
	FString GetSummary()const 
	{ 
		FString result = Summary;
		if (Type == EQuestConditionType::DestroyingEnemy)
			result += FString::Printf(TEXT("(%i/%i)"), CurrentEnemyCount, TargetEnemyCount);
		else if(Type == EQuestConditionType::Survive)
			result += FString::Printf(TEXT("(%i초)"), TargetTimeToSurvive);
		else if(Type == EQuestConditionType::Interact)
			result += FString::Printf(TEXT("(%i/%i)"), CurrentInteractCount, TargetInteractCount);
		return result;
	}

	FORCEINLINE EQuestConditionType GetType() const { return Type; }

	FORCEINLINE const TArray<TSubclassOf<IIQuestObjective>>& GetTargetEnemyClasses() const { return TargetEnemyClasses; }
	FORCEINLINE const TArray<TSubclassOf<IIQuestObjective>>& GetTargetInteractClasses() const { return TargetInteractClasses; }

	FORCEINLINE void IncreaseEnemyCount() { ++CurrentEnemyCount; }
	FORCEINLINE void IncreaseInteractCount() { ++CurrentInteractCount; }
};

UCLASS()
class UQuest_Objective : public UObject
{
	GENERATED_BODY()
	//property
private:
	TArray<const FQuestCondition*> QuestConditions;
	EQuestObjectiveState State;
protected:
public:

	//function
private:
protected:
public:
	void SetQuestConditions(TArray<FQuestCondition>& InArray) 
	{
		for (int32 i = 0; i < InArray.Num(); ++i)
			QuestConditions.Add(&InArray[i]);
	}

	FORCEINLINE const TArray<const FQuestCondition*>& GetQuestConditions() const { return QuestConditions; }
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
		FString PopupMessage;

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
	void CheckCondition(AActor* InObject);
	void AddToQuestPool(AActor* InObject);

	FORCEINLINE UQuest_Objective* GetMainQuestOjbective() const { return MainQuestOjbective; }
	FORCEINLINE UQuest_Objective* GetAdditiveQuestOjbective() const { return AdditiveQuestOjbective; }
	FORCEINLINE FName GetStreamingLevelName() const { return StreamingLevelName; }
	FORCEINLINE FString GetPopupMessage() const { return PopupMessage; }
};

/*
* 
* 
* 
* 
*/