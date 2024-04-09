#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "DungeonGameMode.generated.h"

class ADungeonPlayerController;

UCLASS(minimalapi)
class ADungeonGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ADungeonGameMode();
	virtual void PostLogin(APlayerController* NewPlayer) override;

	//property
private:
	TArray<ADungeonPlayerController*> Players;
protected:
public:

	//function
private:
protected:
public:


};



