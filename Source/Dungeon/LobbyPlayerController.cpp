#include "LobbyPlayerController.h"
#include "Global.h"

#include "SaveManager.h"
#include "Characters/LobbyCharacter.h"

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	TArray<FSaveMetaData> metaDatas = USaveManager::GetAllSaveMetaData();

	for (auto i : metaDatas)
	{
		USaveGameData* saveGameData = Cast<USaveGameData>(UGameplayStatics::LoadGameFromSlot(i.SlotName, 0));

		ALobbyCharacter* lobby = Cast<ALobbyCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), LobbyCharacterClass));
		lobby->OnAfterLoad(saveGameData);
	}
}
