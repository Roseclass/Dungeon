#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DungeonGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEON_API UDungeonGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	/* virtual function to allow custom gameinstance an opportunity to set up what it needs */
	virtual void Init() override;
};
