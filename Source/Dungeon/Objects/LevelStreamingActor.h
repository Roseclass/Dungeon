#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Interfaces/IInteractable.h"
#include "LevelStreamingActor.generated.h"

class USceneComponent;
class UMeshComponent;
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
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		EStageList StageList;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		FName StageName;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		FString StageDescription;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
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

	virtual void StartInteract(ADungeonPlayerController* InPlayer)override;
	virtual void EndInteract(ADungeonPlayerController* InPlayer)override;
	virtual void StartCursorOver(ADungeonPlayerController* InPlayer)override;
	virtual void EndCursorOver(ADungeonPlayerController* InPlayer)override;
	virtual bool IsInteractable()override;

	//property
private:
	bool bActive;
	bool bLoad;
	EStageList CurrentStage = EStageList::Max;
	TArray<UMeshComponent*> MeshComponents;
protected:
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
		UBoxComponent* InteractCollision;

	UPROPERTY(EditDefaultsOnly)
		UDataTable* DataTable;

	UPROPERTY(BlueprintReadOnly)
		TArray<FStageData> Datas;

	UPROPERTY(BlueprintReadWrite)
		TMap<APlayerCharacter*, bool> SequenceMap;
public:
	//function
private:
	void LoadLevel(FStageData InData);
	void LoadLevel(FName InLevelName);
	void LoadLevelStarted();
	UFUNCTION()void LoadLevelFinished();
	UFUNCTION(NetMulticast, Reliable)void Multicast_ForwardSequence();
	UFUNCTION(NetMulticast, Reliable)void Multicast_ReverseSequence();
	UFUNCTION()void ForwardSequenceFinished(APlayerCharacter* InPlayerCharacter);
	void StartReverseSequence();
protected:
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent) void Activate();
	void Activate_Implementation();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent) void Deactivate();
	void Deactivate_Implementation();

	UFUNCTION(BlueprintCallable)FStageData GetCurrentData()const;
};

//
// quest
// interact
// popup
// complete
// sequence
//