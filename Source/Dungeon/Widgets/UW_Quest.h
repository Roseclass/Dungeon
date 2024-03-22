#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "UW_Quest.generated.h"

/**
 * 
 */

class AQuest;
class UTreeView;
class UTextBlock;
class UButton;
class UVerticalBox;
class UHorizontalBox;
class UQuest_Objective;

UCLASS()
class UUW_QuestEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
protected:
	virtual void NativeOnInitialized()override;

	virtual void NativeOnItemExpansionChanged(bool bIsExpanded)override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject)override;

	//property
private:
	UPROPERTY()UQuest_Objective* Objective;
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UVerticalBox* Vertical;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UHorizontalBox* Horizontal;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* Expand;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* Object;
public:

	//function
private:
	UFUNCTION()void OnExpandButtonClicked();
protected:
public:
};

UCLASS()
class DUNGEON_API UUW_Quest : public UUserWidget
{
	GENERATED_BODY()
protected:

	//property
private:
	AQuest* Quest;
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTreeView* ObjectTreeView;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* Name;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* Description;
public:

	//function
private:
protected:
public:
	UFUNCTION(BlueprintCallable)void SetQuest(AQuest* NewQuest);
};
