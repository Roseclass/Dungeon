#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QuestBoard.generated.h"

class UCameraComponent;
class USpringArmComponent;
class USceneComponent;
class AQuest;
class AQuestPoster;
class ADungeonPlayerController;
class UQuestListComponent;

UCLASS()
class DUNGEON_API AQuestBoard : public AActor
{
	GENERATED_BODY()
	
public:	
	AQuestBoard();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;

	//property
private:
	bool bClicked;

	UPROPERTY()UQuestListComponent* List;
	TArray<AQuest*> Quests;
	TArray<AQuestPoster*> QuestPosters;
	TArray<USceneComponent*> PosterPinRoots;

	UPROPERTY(EditDefaultsOnly, Category = "Poster")
		FName ComponentTag = "PosterPinRoot";

	UPROPERTY(EditDefaultsOnly, Category = "Poster")
		TSubclassOf<AQuestPoster> PosterClass;
protected:
	//scene
	UPROPERTY(VisibleAnywhere)
		USceneComponent* RootScene;

public:

	//function
private:
	void GeneratePosters();
protected:
	UFUNCTION(BlueprintCallable)void ActivateQuestPosters();
	UFUNCTION(BlueprintCallable)void DeactivateQuestPosters();
public:
	void GenerateQuests();
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)void BlendSequence(ADungeonPlayerController* InPlayer);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)void EndSequence();

	FORCEINLINE void SetList(UQuestListComponent* NewList) { List = NewList;}
	FORCEINLINE void SetClicked(bool NewClicked) { bClicked = NewClicked;}
	FORCEINLINE bool IsClicked()const { return bClicked;}
};
