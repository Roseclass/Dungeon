#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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
	//property
private:
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
		UEditableTextBox* SessionName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UVerticalBox* SessionList;

public:

	//function
private:
protected:
public:
};
