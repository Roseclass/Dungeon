#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_Dead.generated.h"

/**
 * 
 */

class UButton;
class UTextBlock;
class UWidgetAnimation;

UCLASS()
class DUNGEON_API UUW_Dead : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeOnInitialized()override;

	//property
private:
	TArray<AActor*>ViewTargets;
	int32 ViewIndex;
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* Description;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* ConfirmButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* NextViewTargetButton;

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* Reveal;

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* Hide;

public:

	//function
private:
	UFUNCTION()void OnConfirmButtonClicked();
	UFUNCTION()void OnNextViewTargetButtonClicked();
protected:
public:
	void On();
};
