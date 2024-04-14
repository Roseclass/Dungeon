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
class UMaterialInstance;

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
	bool bConfirm;
	bool bCancel;
	float ActivateTime;
	float TimeLimit;

	TArray<AActor*>PortraitActors;
	TMap<UImage*, TTuple<float, bool>> TickSigns;
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

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* Cancel;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* Portrait0;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* Portrait1;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* Portrait2;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* Portrait3;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* Sign0;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* Sign1;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* Sign2;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* Sign3;

	UPROPERTY(EditDefaultsOnly, Category = "Sign")
		UMaterialInstance* WaitSign;

	UPROPERTY(EditDefaultsOnly, Category = "Sign")
		UMaterialInstance* ConfirmSign;

	UPROPERTY(EditDefaultsOnly, Category = "Sign")
		UMaterialInstance* CancelSign;

public:
	FConfirmPopupButtonClicked OnConfirm;
	FConfirmPopupButtonClicked OnCancel;

	//function
private:
	UFUNCTION()void OnConfirmButtonClicked();
	UFUNCTION()void OnCancelButtonClicked();

	void TickFunction(float DeltaTime);
	void SetWaitSign(UImage* InImage);
	void SetConfirmSign(UImage* InImage);
	void SetCancelSign(UImage* InImage);
protected:
	UFUNCTION(BlueprintImplementableEvent)void CreatePortrait(const TArray<AActor*>& Actors);
public:
	void Init(FString InDescription, TArray<AActor*> NewPortraitActors, float NewTimeLimit = 10);
	void Activate();
	void UpdateSign(AActor* PortraitActor, bool NewState);
	void ConfirmedSequence();
};
