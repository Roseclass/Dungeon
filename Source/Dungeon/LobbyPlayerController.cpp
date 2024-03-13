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

bool ALobbyPlayerController::IsCurrentSlotActive()
{
	if (!SlotNames.IsValidIndex(CurrentCharacter)) { CLog::Print("SlotNames.IsValidIndex"); return 0; }
	if (!Characters.IsValidIndex(CurrentCharacter)) { CLog::Print("Characters.IsValidIndex"); return 0; }

	return USaveManager::IsActivate(SlotNames[CurrentCharacter]);
}

void ALobbyPlayerController::SetViewTarget(int32 InIndex)
{
	if (!SlotNames.IsValidIndex(InIndex)) { CLog::Print("SlotNames.IsValidIndex"); return; }
	if (!Characters.IsValidIndex(InIndex)) { CLog::Print("Characters.IsValidIndex"); return; }

	CurrentCharacter = InIndex;
	SetViewTargetWithBlend(Characters[InIndex]);
	USaveManager::SetCurrentSaveSlot(SlotNames[InIndex]);
}

void ALobbyPlayerController::RefreshCurrentSlot()
{
	if (!SlotNames.IsValidIndex(CurrentCharacter)) { CLog::Print("RefreshCurrentSlot SlotNames.IsValidIndex"); return; }
	if (!Characters.IsValidIndex(CurrentCharacter)) { CLog::Print("RefreshCurrentSlot Characters.IsValidIndex"); return; }
	USaveManager::SetCurrentSaveSlot(SlotNames[CurrentCharacter]);
}

void ALobbyPlayerController::HideCharater(int32 InIndex)
{
	if (!SlotNames.IsValidIndex(InIndex)) { CLog::Print("SlotNames.IsValidIndex"); return; }
	if (!Characters.IsValidIndex(InIndex)) { CLog::Print("Characters.IsValidIndex"); return; }

	if(Characters[InIndex])Characters[InIndex]->SetActorHiddenInGame(1);
	else CLog::Print("Characters[InIndex] is nullptr");
}

void ALobbyPlayerController::HideCharater(ALobbyCharacter* InCharacter)
{
	for (int32 i = 0; i < Characters.Num(); ++i)
		if (Characters[i] == InCharacter)
		{
			HideCharater(i);
			return;
		}
	CLog::Print("Characters not contains InCharacter");
}

void ALobbyPlayerController::RevealCharater(int32 InIndex)
{
	if (!SlotNames.IsValidIndex(InIndex)) { CLog::Print("SlotNames.IsValidIndex"); return; }
	if (!Characters.IsValidIndex(InIndex)) { CLog::Print("Characters.IsValidIndex"); return; }

	if (Characters[InIndex])Characters[InIndex]->SetActorHiddenInGame(0);
	else CLog::Print("Characters[InIndex] is nullptr");
}

void ALobbyPlayerController::RevealCharater(ALobbyCharacter* InCharacter)
{
	for (int32 i = 0; i < Characters.Num(); ++i)
		if (Characters[i] == InCharacter)
		{
			RevealCharater(i);
			return;
		}
	CLog::Print("Characters not contains InCharacter");
}

void ALobbyPlayerController::CreateCharacter(FString InCharacterName)
{
	if (!SlotNames.IsValidIndex(CurrentCharacter)) { CLog::Print("CreateCharacter SlotNames.IsValidIndex"); return; }
	if (!Characters.IsValidIndex(CurrentCharacter)) { CLog::Print("CreateCharacter Characters.IsValidIndex"); return; }

	TScriptInterface<IISave> ptr = Characters[CurrentCharacter];
	USaveManager::SaveActor(ptr);
	USaveManager::SaveCharacterName(InCharacterName);
	USaveManager::ActivateSlot();
}

void ALobbyPlayerController::DeleteCharacter()
{
	if (!SlotNames.IsValidIndex(CurrentCharacter)) { CLog::Print("DeleteCharacter SlotNames.IsValidIndex"); return; }
	if (!Characters.IsValidIndex(CurrentCharacter)) { CLog::Print("DeleteCharacter Characters.IsValidIndex"); return; }
	USaveManager::SetCurrentSaveSlot(SlotNames[CurrentCharacter]);
	USaveManager::ResetSlot();
	HideCharater(CurrentCharacter);
}

ALobbyCharacter* ALobbyPlayerController::GetCurrentCharacter()
{
	if (!SlotNames.IsValidIndex(CurrentCharacter)) { CLog::Print("GetCurrentCharacter SlotNames.IsValidIndex"); return nullptr; }
	if (!Characters.IsValidIndex(CurrentCharacter)) { CLog::Print("GetCurrentCharacter Characters.IsValidIndex"); return nullptr; }
	return Characters[CurrentCharacter];
}
