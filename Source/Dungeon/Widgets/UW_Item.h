#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_Item.generated.h"

/**
 * 
 */

class UBorder;
class UTextBlock;

UCLASS()
class DUNGEON_API UUW_Item : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeOnInitialized()override;

	//property
private:
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UBorder* Background;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* Name;

public:

	//function
private:
protected:
public:
	void SetName(FString NewName);
	void StartCursorOver();
	void EndCursorOver();
};
