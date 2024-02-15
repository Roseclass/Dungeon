#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_Inventory.generated.h"

/**
 * 
 */

class UBorder;
class UButton;
class UUW_InventoryGrid;

UCLASS()
class DUNGEON_API UUW_Inventory : public UUserWidget
{
	GENERATED_BODY()

	//property
private:
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UBorder* Background;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* Preset0;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* Preset1;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_InventoryGrid* Grid;

public:

	//function
private:
protected:
public:
};
