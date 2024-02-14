#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_Main.generated.h"

/**
 * 
 */

class UUW_QuickSlot;

UCLASS()
class DUNGEON_API UUW_Main : public UUserWidget
{
	GENERATED_BODY()

	//property
private:
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_QuickSlot* QuickSlot;
public:

	//function
private:
protected:
public:	

	FORCEINLINE UUW_QuickSlot* GetQuickSlot() const { return QuickSlot; }
};
