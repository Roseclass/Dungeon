#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Quest.generated.h"

class AEnemy;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FQuestCompleted, int32, RootNumber, int32, NodeNumber);
DECLARE_DELEGATE(FQuestCountChanged);

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
	UPROPERTY(EditAnywhere, meta = (EditCondition = "Type == EQuestConditionType::DestroyingEnemy", EditConditionHides))
		int32 TargetEnemyCount;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "Type == EQuestConditionType::DestroyingEnemy", EditConditionHides))
		TArray<TSubclassOf<AActor>> TargetEnemyClasses;

	// EQuestConditionType::Survive
	UPROPERTY(EditAnywhere, meta = (EditCondition = "Type == EQuestConditionType::Survive", EditConditionHides))
		int32 TargetTimeToSurvive;

	// EQuestConditionType::Interact
	UPROPERTY(EditAnywhere, meta = (EditCondition = "Type == EQuestConditionType::Interact", EditConditionHides))
		int32 TargetInteractCount;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "Type == EQuestConditionType::Interact", EditConditionHides))
		TArray<TSubclassOf<AActor>> TargetInteractClasses;
public:
	FORCEINLINE FString GetSummary() const { return Summary; }
	FORCEINLINE EQuestConditionType GetType() const { return Type; }
	FORCEINLINE const TArray<TSubclassOf<AActor>>& GetTargetEnemyClasses() const { return TargetEnemyClasses; }
	FORCEINLINE int32 GetTargetEnemyCount() const { return TargetEnemyCount; }
	FORCEINLINE int32 GetTargetTimeToSurvive() const { return TargetTimeToSurvive; }
	FORCEINLINE const TArray<TSubclassOf<AActor>>& GetTargetInteractClasses() const { return TargetInteractClasses; }
	FORCEINLINE int32 GetTargetInteractCount() const { return TargetInteractCount; }
};

UCLASS()
class UQuest_Objective : public UObject
{
	GENERATED_BODY()
	//property
private:
	TArray<const FQuestCondition*> QuestConditions;
	TArray<const int32*> QuestCounts;
	EQuestObjectiveState State;
protected:
public:
	FQuestCountChanged OnQuestCountChanged;
	//function
private:
protected:
public:
	void SetQuestConditions(TArray<FQuestCondition>& InArray) 
	{
		for (int32 i = 0; i < InArray.Num(); ++i)
			QuestConditions.Add(&InArray[i]);
	}

	void SetQuestCounts(TArray<int32>& InArray)
	{
		for (int32 i = 0; i < InArray.Num(); ++i)
			QuestCounts.Add(&InArray[i]);
	}

	FORCEINLINE const TArray<const FQuestCondition*>& GetQuestConditions() const { return QuestConditions; }
	FORCEINLINE const TArray<const int32*>& GetQuestCounts() const { return QuestCounts; }
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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//property
private:
	UPROPERTY()UQuest_Objective* MainQuestOjbective;
	UPROPERTY()UQuest_Objective* AdditiveQuestOjbective;
	UPROPERTY(Replicated, ReplicatedUsing = "OnRep_MainCounts")TArray<int32> MainCounts;
	UPROPERTY(Replicated, ReplicatedUsing = "OnRep_AdditiveCounts")TArray<int32> AdditiveCounts;
	TArray<TSet<AActor*>> MainQuestOjbectivePool;
	TArray<TSet<AActor*>> AdditiveQuestOjbectivePool;
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
	UFUNCTION() void OnRep_MainCounts();
	UFUNCTION() void OnRep_AdditiveCounts();
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