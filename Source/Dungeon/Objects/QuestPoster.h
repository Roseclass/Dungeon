#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QuestPoster.generated.h"

class AQuest;
class AQuestBoard;
class UUW_QuestPopup;

UCLASS()
class DUNGEON_API AQuestPoster : public AActor
{
	GENERATED_BODY()
	
public:	
	AQuestPoster();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;
	//property
private:
	bool bActive;
	bool bPopup;
	AQuest* Quest;
	AQuestBoard* Board;
	UPROPERTY()UUW_QuestPopup* PopupWidget;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Popup")
		TSubclassOf<UUW_QuestPopup> PopupWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Popup")
		FString PopupWidgetDescription;

public:

	//function
private:
	UFUNCTION()void OnActorBeginCursorOver(AActor* TouchedActor);
	UFUNCTION()void OnActorEndCursorOver(AActor* TouchedActor);
	UFUNCTION()void OnActorClicked(AActor* TouchedActor, FKey ButtonPressed);
	void CreatePopup();
	UFUNCTION()void Confirm();
	UFUNCTION()void Cancel();
protected:
	UFUNCTION(BlueprintImplementableEvent)void OnGlow();
	UFUNCTION(BlueprintImplementableEvent)void OffGlow();
public:
	void SetQuest(AQuest* NewQuest);
	void SetBoard(AQuestBoard* NewBoard);

	FORCEINLINE void Activate() { bActive = 1; }
	FORCEINLINE void Deactivate() { bActive = 0; }
};