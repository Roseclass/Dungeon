#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "UW_LobbySession.generated.h"

/**
 * 
 */

class UCheckBox;
class UButton;
class UTextBlock;
class UEditableTextBox;
class UVerticalBox;

UCLASS()
class DUNGEON_API UUW_LobbySessionTab : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct()override;

	//property
private:
	FOnlineSessionSearchResult SearchResult;
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* Join;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* ServerName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* Ping;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* Players;

public:
	FOnJoinSessionCompleteDelegate OnJoinSessionComplete;
	FDelegateHandle JoinSessionCompleteHandle;

	//function
private:
	UFUNCTION()void OnJoinButtonClicked();
	void OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

protected:
public:
	void SetResult(const FOnlineSessionSearchResult& Result);
};

UCLASS()
class DUNGEON_API UUW_LobbySession : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct()override;

	//property
private:
	int32 MaxNumPlayers = 2;
	bool bIsLAN;

	int32 SlotIndex = 0;
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UCheckBox* MaxPlayers_2;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UCheckBox* MaxPlayers_3;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UCheckBox* MaxPlayers_4;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UCheckBox* Lan;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* New;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* Delete;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* Prev;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* Next;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* Create;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* Find;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UEditableTextBox* SessionNameBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UVerticalBox* SessionList;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UUW_LobbySessionTab> JoinWidgetClass;

public:
	FOnCreateSessionCompleteDelegate OnCreateSessionComplete;
	FOnCreateSessionCompleteDelegate OnStartSessionComplete;
	FDelegateHandle CreateSessionCompleteHandle;
	FDelegateHandle StartSessionCompleteHandle;
	
	FOnFindSessionsCompleteDelegate OnFindSessions;
	FDelegateHandle FindSessionsHandle;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	//function
private:
	//checkbox
	UFUNCTION()void OnMaxPlayers_2StateChanged(bool bIsChecked);
	UFUNCTION()void OnMaxPlayers_3StateChanged(bool bIsChecked);
	UFUNCTION()void OnMaxPlayers_4StateChanged(bool bIsChecked);
	UFUNCTION()void OnLanStateChanged(bool bIsChecked);
	//create, delete character
	UFUNCTION()void OnNewButtonClicked();
	UFUNCTION()void OnDeleteButtonClicked();
	//select character
	UFUNCTION()void OnPrevButtonClicked();
	UFUNCTION()void OnNextButtonClicked();
	//create session
	UFUNCTION()void OnCreateButtonClicked();
	void OnCreateSessionCompleted(FName SessionName, bool bWasSuccessful);
	void OnStartSessionCompleted(FName SessionName, bool bWasSuccessful);
	//find sessions
	UFUNCTION()void OnFindButtonClicked();
	void OnFindSessionsCompleted(bool bSuccess);
protected:
public:
};
