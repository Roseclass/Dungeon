#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_InventoryPopup.generated.h"

/**
 * 
 */

class UBorder;
class UTextBlock;
class URichTextBlock;
class UItemObject;

UCLASS()
class DUNGEON_API UUW_InventoryPopup : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime)override;

	//property
private:
	bool bOn;
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* Name;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* Type;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* Damage;

	UPROPERTY(VisibleDefaultsOnly, meta = (BindWidget))
		URichTextBlock* EffectDescription;

public:

	//function
private:
	void RefreshLocation();
protected:
public:
	void On(UItemObject* InObject);
	void Off();
};
