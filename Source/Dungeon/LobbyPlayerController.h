#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

/**
 * 
 */

class ALobbyCharacter;
class UUW_Lobby;

UCLASS()
class DUNGEON_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ALobbyPlayerController();
protected:
	virtual void BeginPlay() override;

	//property
private:
	int32 CurrentCharacter = 0;
	TArray<FString> SlotNames;
	TArray<ALobbyCharacter*> Characters;
	UPROPERTY()UUW_Lobby* Widget;
protected:
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<ALobbyCharacter> LobbyCharacterClass;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UUW_Lobby> LobbyWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "SaveLoad")
		int32 MaxSlot = 4;

	UPROPERTY(EditAnywhere, Category = "Display")
		FTransform OriginDisplayTransform;

	UPROPERTY(EditAnywhere, Category = "Display")
		FVector Offset = FVector(0, 450, 0);
public:

	//function
private:
protected:
public:
	bool TryAddNewCharacter(int32 InIndex);
	void SetViewTarget(int32 InIndex);
	void HideCharater(int32 InIndex);
	void RevealCharater(int32 InIndex);
};
//
// 데이터 슬롯 수만큼 스폰,로드,링크
//