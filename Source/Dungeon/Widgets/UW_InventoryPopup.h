#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Abilities/AbilityTaskTypes.h"
#include "UW_InventoryPopup.generated.h"

/**
 * 
 */

class UBorder;
class UTextBlock;
class URichTextBlock;
class UItemObject;

enum class EItemGrade : uint8;

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
		UTextBlock* StatusDescription;

	UPROPERTY(VisibleDefaultsOnly, meta = (BindWidget))
		URichTextBlock* EhancementDescription;

public:

	//function
private:
	void RefreshLocation();
	FString MakeEhancementText(const FSkillEnhancement& Data, EItemGrade Grade);
protected:
public:
	void On(UItemObject* InObject);
	void Off();
};
