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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//property
private:
	bool bCanToggle = 1;
	UPROPERTY(ReplicatedUsing = "OnRep_Quest")AQuest* Quest;
	UPROPERTY()UUW_Quest* QuestWidget;
protected:
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UUW_Quest> QuestWidgetClass;
public:

	//function
private:
	UFUNCTION()void OnRep_Quest();
protected:
public:
	UFUNCTION(BlueprintCallable)void SetQuest(AQuest* InQuest);
	UFUNCTION(BlueprintCallable)void RemoveQuest(AQuest* InQuest);

	void ToggleWidget();
	UFUNCTION(BlueprintCallable)void OnWidget();
	UFUNCTION(BlueprintCallable)void OffWidget();

	// for poster
	void SetCanToggle();
	void SetCannotToggle();
	void SetPreview(AQuest* InQuest);

	// for AQuest
	UFUNCTION(Reliable, Server)void CheckCondition(AActor* InObject);
	void AddToQuestPool(AActor* InObject);

	FORCEINLINE AQuest* GetQuest() const { return Quest; }
};
