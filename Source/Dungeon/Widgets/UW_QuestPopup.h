#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_QuestPopup.generated.h"

/**
 * 
 */

class UImage;
class UButton;
class UTextBlock;
class UProgressBar;
class UWidgetAnimation;
class UMaterialInstance;

DECLARE_DELEGATE(FQuestPopupButtonClicked);

UCLASS()
class DUNGEON_API UUW_QuestPopup : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeOnInitialized()override;

	//property
private:
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* Description;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* ConfirmButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* CancelButton;
public:
	FQuestPopupButtonClicked OnConfirm;
	FQuestPopupButtonClicked OnCancel;

	//function
private:
	UFUNCTION()void OnConfirmButtonClicked();
	UFUNCTION()void OnCancelButtonClicked();
protected:
public:
	void Init(FString InDescription);
};
