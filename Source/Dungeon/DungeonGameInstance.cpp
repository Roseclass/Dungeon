#include "DungeonGameInstance.h"
#include "Global.h"
#include "SaveManager.h"

void UDungeonGameInstance::Init()
{
	Super::Init();

	USaveManager::Init();
}
