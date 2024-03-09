#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "UW_LobbySession.generated.h"

/**
 * 
 */

class UCheckBox;
class UButton;
class UEditableTextBox;
class UVerticalBox;

UCLASS()
class DUNGEON_API UUW_LobbySession : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct()override;

	//property
private:
	int32 MaxNumPlayers;
	bool bIsLAN;
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
		UEditableTextBox* SessionNameBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UVerticalBox* SessionList;

public:
	FOnCreateSessionCompleteDelegate OnCreateSessionComplete;
	FOnCreateSessionCompleteDelegate OnStartSessionComplete;
	FDelegateHandle CreateSessionCompleteHandle;
	FDelegateHandle StartSessionCompleteHandle;
	
	FOnFindSessionsCompleteDelegate OnFindSessions;
	FDelegateHandle FindSessionsHandle;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	FOnJoinSessionCompleteDelegate OnJoinSessionComplete;
	FDelegateHandle JoinSessionCompleteHandle;

	//function
private:
	//create session
	UFUNCTION()void OnCreateButtonClicked();
	UFUNCTION()void OnCreateSessionCompleted(FName SessionName, bool bWasSuccessful);
	UFUNCTION()void OnStartSessionCompleted(FName SessionName, bool bWasSuccessful);
	//find sessions
	UFUNCTION()void OnFindButtonClicked();
	UFUNCTION()void OnFindSessionsCompleted(bool bSuccess);
	//join
	UFUNCTION()void OnJoinButtonClicked();
	void OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
protected:
public:
};
