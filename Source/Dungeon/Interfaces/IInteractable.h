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
	virtual void Interact(ADungeonPlayerController* InPlayer) = 0;			//상호작용
	virtual void PreInteractEvent(ADungeonPlayerController* InPlayer) = 0;	//라인트레이스로 감지되면 호출
	virtual void InInteractEvent(ADungeonPlayerController* InPlayer) = 0;	//라인트레이스로 감지되는 동안 호출
	virtual void PostInteractEvent(ADungeonPlayerController* InPlayer) = 0;	//라인트레이스가 끝났을때 호출
	virtual bool IsInteractable() = 0;
};
