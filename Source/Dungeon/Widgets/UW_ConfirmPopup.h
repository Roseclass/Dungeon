#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_ConfirmPopup.generated.h"

/**
 * 
 */

class UImage;
class UButton;
class UTextBlock;
class UProgressBar;
class UWidgetAnimation;

DECLARE_DELEGATE(FConfirmPopupButtonClicked);

UCLASS()
class DUNGEON_API UUW_ConfirmPopup : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct()override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime)override;
	virtual void RemoveFromParent() override;

	//property
private:
	bool bActive;
	float ActivateTime;
	float TimeLimit;
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* Description;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* ConfirmButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* CancelButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UProgressBar* TimerBar;

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* Reveal;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* Sign0;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* Sign1;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* Sign2;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* Sign3;

public:
	FConfirmPopupButtonClicked OnConfirm;
	FConfirmPopupButtonClicked OnCancel;

	//function
private:
	UFUNCTION()void OnConfirmButtonClicked();
	UFUNCTION()void OnCancelButtonClicked();

	void SetWaitSign(UImage* InImage);
	void SetConfirmSign(UImage* InImage);
	void SetCancelSign(UImage* InImage);
protected:
public:
	void Init(FString InDescription, float NewTimeLimit/*, int32 NewNumOfSign*/);//need num of players
	void Activate();
	void UpdateSign(int32 PlayerIndex, bool NewState);
};

/*
* someone canceled?
* update sign, play cancel animation, disable buttons
* 
* someone confirmed?
* update sign
* 
* everyone confirmed?
* remove widget, go to next sequence(other class's)
* 
*/