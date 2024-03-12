#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameFramework/SaveGame.h"
#include "RunTime/Core/Public/Misc/DateTime.h"
#include "Interfaces/ISave.h"
#include "Components/AppearanceComponent.h"
#include "SaveManager.generated.h"

class USkeletalMeshComponent;
class AWeapon;
class ASkillActor;

USTRUCT(BlueprintType)
struct FSaveMetaData
{
	GENERATED_BODY()

public:
	// the name of the slot
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
		FString SlotName;

	// the date that the slot was created (or the game was saved)
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
		FDateTime Date;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
		bool Activate;
};

UCLASS()
class DUNGEON_API USaveGameMetaData : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "Default")
		TMap<FString, FSaveMetaData> SavedGamesMetaData;
};

USTRUCT(BlueprintType)
struct FPlayerSaveData
{
	GENERATED_BODY()
public:
	//Lobby
	UPROPERTY(VisibleAnywhere)
		FString CharacterName;

	//Appearance
	UPROPERTY(VisibleAnywhere)
		TArray<int32> MeshIndices;

	UPROPERTY(VisibleAnywhere)
		TMap<EAppearancePart, FAppearancePartColor> AppearanceColors;

	//Inventory
	UPROPERTY(VisibleAnywhere)
		TArray<TSubclassOf<AWeapon>> PresetClasses;

	UPROPERTY(VisibleAnywhere)
		TArray<TSubclassOf<AWeapon>> InventoryClasses;

	//Skill
	UPROPERTY(VisibleAnywhere)
		int32 SkillPoints;

	UPROPERTY(VisibleAnywhere)
		TArray<TSubclassOf<ASkillActor>> AcquiredSkills;

	UPROPERTY(VisibleAnywhere)
		TArray<TSubclassOf<ASkillActor>> SlotSkills;
};

UCLASS()
class DUNGEON_API USaveGameData : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
		FPlayerSaveData PlayerData;
};

UCLASS()
class DUNGEON_API USaveManager : public UObject
{
	GENERATED_BODY()
		//property
private:
	// the current save slot
	static FString CurrentSaveSlot;
	// all the actors in the game which implement the save interface
	static TArray<TScriptInterface<IISave>> SaveInterfaces;
protected:
public:

	//function
private:
protected:
public:
	// initialize the calss. mustbe called when the first launches
	static void Init();

	// queries all the actors in the game that implement the save interface
	static void QueryAllSaveInterfaces();

	// Save the current state of the game
	UFUNCTION(BlueprintCallable, Category = "SaveLoad")
		static void SaveGame();	
	
	// Save the specific actor
	UFUNCTION(BlueprintCallable, Category = "SaveLoad")
		static void SaveActor(TScriptInterface<IISave> InActor);

	// Set CharacterName
	UFUNCTION(BlueprintCallable, Category = "SaveLoad")
		static void SaveCharacterName(FString InCharacterName);

	// Loads the current state of the game
	UFUNCTION(BlueprintCallable, Category = "SaveLoad")
		static void LoadGame();

	// Deletes the specified slot
	UFUNCTION(BlueprintCallable, Category = "SaveLoad")
		static void DeleteSlot(const FString SlotName);

	// Activate Current Slot Data
	UFUNCTION(BlueprintCallable, Category = "SaveLoad")
		static void ActivateSlot();

	// Reset Current Slot Data
	UFUNCTION(BlueprintCallable, Category = "SaveLoad")
		static void ResetSlot();

	// Gets a new, unused slot
	UFUNCTION(BlueprintPure, Category = "SaveLoad")
		static UPARAM(DisplayName = "Slot") FString GetNewSaveSlot(bool& slot_found);

	// Sets the current save slot to the specified value
	UFUNCTION(BlueprintCallable, Category = "SaveLoad")
		static void SetCurrentSaveSlot(const FString& slot);

	// Sets the current save slot to the specified value
	UFUNCTION(BlueprintPure, Category = "SaveLoad")
		static FString GetCurrentSaveSlot();

	// Gets all the saved game
	UFUNCTION(BlueprintPure, Category = "SaveLoad")
		static TArray<FSaveMetaData> GetAllSaveMetaData();

	// Gets max size of slot
	UFUNCTION(BlueprintPure, Category = "SaveLoad")
		static int32 GetMaxSize();

	// Gets max size of slot
	UFUNCTION(BlueprintPure, Category = "SaveLoad")
		static bool IsActivate(const FString& slot);
};

