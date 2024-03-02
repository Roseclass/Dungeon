#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ISave.generated.h"

class USaveGameData;

UINTERFACE(MinimalAPI)
class UISave : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DUNGEON_API IISave
{
	GENERATED_BODY()

public:
	// Get a unique name to use when saving
	virtual	FString GetUniqueSaveName() = 0;

	// Called right before the owning actor is saved
	virtual	void OnBeforeSave(USaveGameData* SaveData) = 0;

	// Called after the load
	virtual	void OnAfterLoad(USaveGameData* const ReadData) = 0;
};
