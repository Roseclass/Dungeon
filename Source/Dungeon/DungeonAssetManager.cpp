#include "DungeonAssetManager.h"
#include "AbilitySystemGlobals.h"


UDungeonAssetManager& UDungeonAssetManager::Get()
{
	UDungeonAssetManager* Singleton = Cast<UDungeonAssetManager>(GEngine->AssetManager);

	if (Singleton)
	{
		return *Singleton;
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("Invalid AssetManager in DefaultEngine.ini, must be DungeonAssetManager!"));
		return *NewObject<UDungeonAssetManager>();	 // never calls this
	}
}


void UDungeonAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	UAbilitySystemGlobals::Get().InitGlobalData();
}
