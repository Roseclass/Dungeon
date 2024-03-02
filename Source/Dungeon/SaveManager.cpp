#include "SaveManager.h"
#include "Global.h"

static const FString kMetadataSaveSlot = "SaveGameMetadata";
static const int32 kMaxSaveSlot = 100;

FString USaveManager::CurrentSaveSlot;
TArray<TScriptInterface<IISave>> USaveManager::SaveInterfaces;

void USaveManager::Init()
{
	CurrentSaveSlot = "Default";

	// Make sure the metadata file exists incase the game has never been ran
	USaveGame* saveGameMetaData = UGameplayStatics::LoadGameFromSlot(kMetadataSaveSlot, 0);

	if (!saveGameMetaData)
	{
		// since the metadata file doesn't exist, we need to create one
		USaveGame* saveGameOjbect = UGameplayStatics::CreateSaveGameObject(USaveGameMetaData::StaticClass());

		UGameplayStatics::SaveGameToSlot(saveGameOjbect, kMetadataSaveSlot, 0);
	}
}

void USaveManager::QueryAllSaveInterfaces()
{
	// Clear old entries
	SaveInterfaces.Empty();

	// Get all the actors that implement the save interface
	TArray<AActor*>actors;
	UGameplayStatics::GetAllActorsWithInterface(GWorld, UISave::StaticClass(), actors);

	for (auto i : actors)
	{
		SaveInterfaces.Add(i);
	}
}

void USaveManager::SaveGame()
{
	// Create a new save game data instace
	USaveGameData* saveGameData = Cast<USaveGameData>(UGameplayStatics::CreateSaveGameObject(USaveGameData::StaticClass()));

	// Go over all the actros that need to be saved and save them
	for (auto& i : SaveInterfaces)
	{
		if (!i.GetObject())continue;

		// let the object know that it's about to be saved
		i->OnBeforeSave(saveGameData);
	}

	// save the game to the curren slot
	UGameplayStatics::SaveGameToSlot(saveGameData, CurrentSaveSlot, 0);

	// update the metadata file with the new slot
	USaveGameMetaData* saveGameMetaData = Cast<USaveGameMetaData>(UGameplayStatics::LoadGameFromSlot(kMetadataSaveSlot, 0));

	FSaveMetaData& saveMetadata = saveGameMetaData->SavedGamesMetaData.FindOrAdd(CurrentSaveSlot);
	saveMetadata.SlotName = CurrentSaveSlot;
	saveMetadata.Date = FDateTime::Now();

	// save the changes to the metadata file
	UGameplayStatics::SaveGameToSlot(saveGameMetaData, kMetadataSaveSlot, 0);
}

void USaveManager::LoadGame()
{
	USaveGameData* saveGameData = Cast<USaveGameData>(UGameplayStatics::LoadGameFromSlot(CurrentSaveSlot, 0));

	if (!saveGameData)
	{
		// no saves exist yet for this slot. save a default one.
		SaveGame();

		// Reload it
		saveGameData = Cast<USaveGameData>(UGameplayStatics::LoadGameFromSlot(CurrentSaveSlot, 0));
	}

	// Loop over all the actors that need to load data nad load their data
	for (auto& i : SaveInterfaces)
		if (!i.GetObject())continue;
}

void USaveManager::DeleteSlot(const FString SlotName)
{
	// Delete the slot
	UGameplayStatics::DeleteGameInSlot(SlotName, 0);

	// Loading the metadata file
	USaveGameMetaData* saveGameMetaData = Cast<USaveGameMetaData>(UGameplayStatics::LoadGameFromSlot(kMetadataSaveSlot, 0));

	saveGameMetaData->SavedGamesMetaData.Remove(SlotName);

	// Save the metadata slot
	UGameplayStatics::SaveGameToSlot(saveGameMetaData, kMetadataSaveSlot, 0);
}

FString USaveManager::GetNewSaveSlot(bool& slot_found)
{
	slot_found = 0;

	// Loading the metadata file
	USaveGameMetaData* saveGameMetaData = Cast<USaveGameMetaData>(UGameplayStatics::LoadGameFromSlot(kMetadataSaveSlot, 0));

	for (int32 i = 0; i < kMaxSaveSlot; i++)
	{
		// SaveSlot0, SaveSlot1, SaveSlot2, etc....
		FString slotName = "SaveSlot" + FString::FromInt(i);

		if (!saveGameMetaData->SavedGamesMetaData.Contains(slotName))
		{
			// we found a free slot ! return it
			slot_found = 1;
			return slotName;
		}
	}

	return FString();
}

void USaveManager::SetCurrentSaveSlot(const FString& slot)
{
	CurrentSaveSlot = slot;
}

FString USaveManager::GetCurrentSaveSlot()
{
	return CurrentSaveSlot;
}

TArray<FSaveMetaData> USaveManager::GetAllSaveMetaData()
{
	TArray<FSaveMetaData> metadata;

	// Loading the metadata file
	USaveGameMetaData* saveGameMetaData = Cast<USaveGameMetaData>(UGameplayStatics::LoadGameFromSlot(kMetadataSaveSlot, 0));

	metadata.Reserve(saveGameMetaData->SavedGamesMetaData.Num());

	// Add each saved game's metadat to the return array
	for (const auto& i : saveGameMetaData->SavedGamesMetaData)
	{
		metadata.Push(i.Value);
	}

	return metadata;
}
