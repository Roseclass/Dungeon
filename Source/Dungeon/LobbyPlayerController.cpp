#include "LobbyPlayerController.h"
#include "Global.h"

#include "SaveManager.h"
#include "Characters/LobbyCharacter.h"
#include "Widgets/UW_Lobby.h"
#include "Widgets/UW_LobbyCharacter.h"
#include "Widgets/UW_LobbySession.h"

ALobbyPlayerController::ALobbyPlayerController()
{
}

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	TArray<FSaveMetaData> metaDatas = USaveManager::GetAllSaveMetaData();

	for (auto i : metaDatas)
	{
		ALobbyCharacter* lobby = Cast<ALobbyCharacter>(GetWorld()->SpawnActor(LobbyCharacterClass));
		if (!lobby)CLog::Print("lobby is nullptr");
		Characters.Add(lobby);
		SlotNames.Add(i.SlotName);
		USaveGameData* saveGameData = Cast<USaveGameData>(UGameplayStatics::LoadGameFromSlot(i.SlotName, 0));
		if (!saveGameData){CLog::Print("saveGameData is nullptr");continue;}
		if (lobby)
		{
			lobby->SetActorTransform(OriginDisplayTransform);
			lobby->AddActorWorldOffset(Offset * (Characters.Num() - 1));
			if (i.Activate)lobby->OnAfterLoad(saveGameData);
			else lobby->SetActorHiddenInGame(1);
		}
	}
	SetViewTarget(CurrentCharacter);
	Widget = CreateWidget<UUW_Lobby, APlayerController>(this, LobbyWidgetClass);
	Widget->AddToViewport();
	SetInputMode(FInputModeUIOnly());
}

bool ALobbyPlayerController::TryAddNewCharacter(int32 InIndex)
{
	if (!SlotNames.IsValidIndex(InIndex))return 0;
	if (!Characters.IsValidIndex(InIndex))return 0;

	//lobby->SetActorTransform(OriginDisplayTransform);
	//lobby->AddActorWorldOffset(Offset * InIndex);
	//SlotNames[InIndex] = "Temp" + FString::FromInt(InIndex);
	//Characters[InIndex] = lobby;

	return 1;
}

void ALobbyPlayerController::SetViewTarget(int32 InIndex)
{
	if (!SlotNames.IsValidIndex(InIndex)) { CLog::Print("SlotNames.IsValidIndex"); return; }
	if (!Characters.IsValidIndex(InIndex)) { CLog::Print("Characters.IsValidIndex"); return; }

	CurrentCharacter = InIndex;
	SetViewTargetWithBlend(Characters[InIndex]);
}

void ALobbyPlayerController::HideCharater(int32 InIndex)
{
	if (!SlotNames.IsValidIndex(InIndex)) { CLog::Print("SlotNames.IsValidIndex"); return; }
	if (!Characters.IsValidIndex(InIndex)) { CLog::Print("Characters.IsValidIndex"); return; }

	if(Characters[InIndex])Characters[InIndex]->SetActorHiddenInGame(1);
	else CLog::Print("Characters[InIndex] is nullptr");
}

void ALobbyPlayerController::RevealCharater(int32 InIndex)
{
	if (!SlotNames.IsValidIndex(InIndex)) { CLog::Print("SlotNames.IsValidIndex"); return; }
	if (!Characters.IsValidIndex(InIndex)) { CLog::Print("Characters.IsValidIndex"); return; }

	if (Characters[InIndex])Characters[InIndex]->SetActorHiddenInGame(0);
	else CLog::Print("Characters[InIndex] is nullptr");
}
