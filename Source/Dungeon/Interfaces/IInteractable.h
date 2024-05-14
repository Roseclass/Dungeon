#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IInteractable.generated.h"

UINTERFACE(MinimalAPI)
class UIInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */

class ADungeonPlayerController;

class DUNGEON_API IIInteractable
{
	GENERATED_BODY()
public:
	virtual void StartInteract(ADungeonPlayerController* InPlayer) = 0;
	virtual void EndInteract(ADungeonPlayerController* InPlayer) = 0;
	virtual void StartCursorOver(ADungeonPlayerController* InPlayer) = 0;
	virtual void EndCursorOver(ADungeonPlayerController* InPlayer) = 0;
	virtual bool IsInteractable() = 0;
};
