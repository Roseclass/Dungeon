#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_Chat.generated.h"

/**
 * 
 */

class UEditableTextBox;
class UScrollBox;

UCLASS()
class DUNGEON_API UUW_Chat : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeOnInitialized() override;

	//property
private:
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UEditableTextBox* ChatBox;

	UPROPERTY(VisibleDefaultsOnly, meta = (BindWidget))
		UScrollBox* MessageBox;
public:

	//function
private:
	UFUNCTION()void OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);
protected:
public:
	void OnMessageUpdated(const FText& Text, const FLinearColor& Color);
	void OnChat();
};
