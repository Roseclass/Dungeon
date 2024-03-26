#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QuestBoard.generated.h"

class UCameraComponent;
class USpringArmComponent;
class USceneComponent;
class AQuest;
class ADungeonPlayerController;

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
	TArray<USceneComponent*> PosterPinRoots;

	UPROPERTY(EditDefaultsOnly, Category = "Poster")
		FName ComponentTag = "PosterPinRoot";
protected:
	//scene
	UPROPERTY(VisibleAnywhere)
		USceneComponent* RootScene;
public:

	//function
private:
protected:
public:
	void GenerateQuests(TArray<TSubclassOf<AQuest>> InQuests);
	UFUNCTION(BlueprintImplementableEvent)void BlendSequence(ADungeonPlayerController* InPlayer);
	UFUNCTION(BlueprintImplementableEvent)void EndSequence();
};
