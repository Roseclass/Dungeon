#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "QuestListComponent.generated.h"

class AQuest;
class UFXSystemAsset;
class UParticleSystemComponent;
class UNiagaraComponent;

USTRUCT(BlueprintType)
struct FQuestTreeData : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AQuest> QuestClass;

	UPROPERTY(EditDefaultsOnly)
		int32 QuestNumber;

	UPROPERTY(EditDefaultsOnly)
		FText QuestName;

	UPROPERTY(EditDefaultsOnly)
		int32 QuestLevel;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEON_API UQuestListComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UQuestListComponent();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//property
private:
	TArray<FQuestTreeData> Quests;
	UParticleSystemComponent* PSign;
	UNiagaraComponent* NSign;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Datas")
		UDataTable* DataTable;

	UPROPERTY(EditDefaultsOnly, Category = "Datas")
		UFXSystemAsset* RewardSign;

	UPROPERTY(EditDefaultsOnly, Category = "Datas")
		FName SlotName;

	UPROPERTY(EditDefaultsOnly, Category = "Datas")
		FTransform AdditiveTransform;
public:

	//function
private:
protected:
public:
	bool FindAvailableQuest(TArray<FQuestTreeData>& InArray, int32 InLevel);
};
