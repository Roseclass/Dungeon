#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "UW_DialogEntry.generated.h"

/**
 * 
 */


class UButton;
class UTextBlock;
class UDialogReplyObject;

DECLARE_MULTICAST_DELEGATE_OneParam(FDialogReplyObject_OnClicked, UDialogReplyObject*);

UCLASS(BlueprintType)
class DUNGEON_API UDialogReplyObject : public UObject
{
	GENERATED_BODY()
		//property
private:
	FText Reply;
	//FQuestTreeData QuestData;
protected:
public:
	FDialogReplyObject_OnClicked OnClicked;

	//function
private:
protected:
public:
	void SetText(FText InText)
	{
		Reply = InText;
	}
	//void SetData(FQuestTreeData InData)
	//{
	//	QuestData = InData;
	//}
	FText GetText()
	{
		return Reply;
	}
	//FQuestTreeData GetData()
	//{
	//	return QuestData;
	//}
};

UCLASS()
class DUNGEON_API UUW_DialogEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
protected:
	virtual void NativeOnInitialized()override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject)override;

		//property
private:
	UDialogReplyObject* DialogReplyObject;
protected:
	UPROPERTY(EditDefaultsOnly)
		FLinearColor HoveredColor;

	UPROPERTY(EditDefaultsOnly)
		FLinearColor UnhoveredColor;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* ReplyButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* ReplyText;

public:
	//function
private:
	UFUNCTION()void OnReplyButtonHover();
	UFUNCTION()void OnReplyButtonUnhover();
	UFUNCTION()void OnReplyButtonClicked();
protected:
public:
};
