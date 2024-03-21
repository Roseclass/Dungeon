#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuestComponent.generated.h"

class AQuest;
class UQuestObjectiveCollection;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEON_API UQuestComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UQuestComponent();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//property
private:
	UPROPERTY()TArray<AQuest*> Quests;
	//UPROPERTY()UCUserWidget_Quest* QuestWidget;
protected:
	//UPROPERTY(EditDefaultsOnly)
	//	TSubclassOf<UCUserWidget_Quest> QuestWidgetClass;
public:

	//function
private:
protected:
public:
	void Reset();
	UFUNCTION(BlueprintCallable)void AddQuest(AQuest* InQuest);
	UFUNCTION(BlueprintCallable)void RemoveQuest(AQuest* InQuest);
	UFUNCTION(BlueprintCallable, BlueprintPure)AQuest* FindQuest(TSubclassOf<AQuest> InQuestClass);

	void ToggleWidget();
	void OnWidget();
	void OffWidget();

	void GetQuestArray(TArray<AQuest*>& InArray);

	AQuest* GetCompletedQuest();

	void FindInteractQuest(AActor* InActor);

};
