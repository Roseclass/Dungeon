#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Interfaces/IInteractable.h"
#include "LevelStreamingActor.generated.h"

class USceneComponent;
class UBoxComponent;
class APlayerCharacter;

UENUM(BlueprintType)
enum class EStageList : uint8
{
	Main,
	Desert,
	Max
};

USTRUCT(BlueprintType)
struct FStageData : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
		EStageList StageList;

	UPROPERTY(EditDefaultsOnly)
		FName StageName;

	UPROPERTY(EditDefaultsOnly)
		FString StageDescription;

	UPROPERTY(EditDefaultsOnly)
		TArray<FVector> StartLocations;
};

UCLASS()
class DUNGEON_API ALevelStreamingActor : public AActor, public IIInteractable
{
	GENERATED_BODY()
public:
	ALevelStreamingActor();
protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;

	virtual void Interact(ADungeonPlayerController* InPlayer) override;			
	virtual void PreInteractEvent(ADungeonPlayerController* InPlayer) override;	
	virtual void InInteractEvent(ADungeonPlayerController* InPlayer) override;	
	virtual void PostInteractEvent(ADungeonPlayerController* InPlayer) override;
	virtual bool IsInteractable() override;

	//property
private:
	bool bActive;
protected:
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
		UBoxComponent* InteractCollision;
public:
	//function
private:
	void LoadLevel(FStageData InData);
protected:
	UFUNCTION(BlueprintImplementableEvent)void LoadLevelStarted();
	UFUNCTION(BlueprintImplementableEvent)void LoadLevelFinished();
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent) void Activate();
	void Activate_Implementation();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent) void Deactivate();
	void Deactivate_Implementation();


};

//
// quest
// interact
// popup
// complete
// sequence
//