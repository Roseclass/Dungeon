#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_Main.generated.h"

/**
 * 
 */

class UUW_QuickSlot;
class UUW_Orb;

UCLASS()
class DUNGEON_API UUW_Main : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct()override;

	//property
private:
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_QuickSlot* QuickSlot;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_Orb* HealthOrb;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_Orb* ManaOrb;

public:

	//function
private:
protected:
public:	

	FORCEINLINE UUW_QuickSlot* GetQuickSlot() const { return QuickSlot; }
	FORCEINLINE UUW_Orb* GetHealthOrb() const { return HealthOrb; }
	FORCEINLINE UUW_Orb* GetManaOrb() const { return ManaOrb; }
};
