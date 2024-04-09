#include "DungeonGameMode.h"
#include "Global.h"
#include "DungeonPlayerController.h"
#include "Characters/PlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADungeonGameMode::ADungeonGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ADungeonPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}

	Players.Init(nullptr, 4);
}

void ADungeonGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ADungeonPlayerController* New = Cast<ADungeonPlayerController>(NewPlayer);
	if (!New)
	{
		CLog::Print("New Player is not ADungeonPlayerController", -1, 10, FColor::Red);
		return;
	}

	for (int32 i = 0; i < 4; ++i)
	{
		if (Players[i])continue;
		Players[i] = New;
		Players[i]->SetIndex(i);
		return;
	}

	CLog::Print("ADungeonGameMode::PostLogin Array is FULL", -1, 10, FColor::Red);

}