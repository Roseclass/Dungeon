#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_Lobby.generated.h"

/**
 * 
 */
class UUW_LobbyCharacter;
class UUW_LobbySession;

UCLASS()
class DUNGEON_API UUW_Lobby : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct()override;

	//property
private:
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_LobbyCharacter* CharacterWidget;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_LobbySession* SessionWidget;
public:

	//function
private:
	UFUNCTION() void OnSessionWidgetNewButtonClicked();
	UFUNCTION() void OnCharacterWidgetConfirmButtonClicked();
	UFUNCTION() void OnCharacterWidgetCancelButtonClicked();
protected:
public:
	FORCEINLINE UUW_LobbyCharacter* GetCharacterWidget(){return CharacterWidget;}
	FORCEINLINE UUW_LobbySession* GetSessionWidget(){return SessionWidget;}
};
