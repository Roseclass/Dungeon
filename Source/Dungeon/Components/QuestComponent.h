#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuestComponent.generated.h"

class AQuest;
class UUW_Quest;

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
	AQuest* Quest;
	UPROPERTY()UUW_Quest* QuestWidget;
protected:
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UUW_Quest> QuestWidgetClass;
public:

	//function
private:
protected:
public:
	UFUNCTION(BlueprintCallable)void SetQuest(AQuest* InQuest);
	UFUNCTION(BlueprintCallable)void RemoveQuest(AQuest* InQuest);

	void ToggleWidget();
	UFUNCTION(BlueprintCallable)void OnWidget();
	UFUNCTION(BlueprintCallable)void OffWidget();
};
