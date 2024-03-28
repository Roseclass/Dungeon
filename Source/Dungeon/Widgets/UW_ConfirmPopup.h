#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_ConfirmPopup.generated.h"

/**
 * 
 */

class UButton;
class UTextBlock;

DECLARE_DELEGATE(FConfirmPopupButtonClicked);

UCLASS()
class DUNGEON_API UUW_ConfirmPopup : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct()override;

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
	FConfirmPopupButtonClicked OnConfirm;
	FConfirmPopupButtonClicked OnCancel;

	//function
private:
	UFUNCTION()void OnConfirmButtonClicked();
	UFUNCTION()void OnCancelButtonClicked();
protected:
public:
	void Init(FString InDescription);
};
