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

USTRUCT(BlueprintType)
struct FQuestCondition
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
		FText Summary;

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
	//UFUNCTION(BlueprintImplementableEvent) UQuestObjectiveCollection* ConstructRootObjectiveCollection();
	//UFUNCTION(BlueprintImplementableEvent) void PopulateObjectives(UQuestObjectiveCollection* RootObjectCollection);
public:
	//UFUNCTION(BlueprintCallable, BlueprintPure) FORCEINLINE UQuestObjectiveCollection* GetRootObjectCollection() { return RootObjectiveCollection; }
};
