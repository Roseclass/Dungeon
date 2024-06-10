#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "DungeonAssetManager.generated.h"

/**
 *
 */
UCLASS()
class DUNGEON_API UDungeonAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:

	static UDungeonAssetManager& Get();

	/** Starts initial load, gets called from InitializeObjectReferences */
	virtual void StartInitialLoading() override;
};
