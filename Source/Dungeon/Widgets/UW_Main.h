#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_Main.generated.h"

/**
 * 
 */

class UUW_QuickSlot;
class UUW_Orb;
class APawn;

UCLASS()
class DUNGEON_API UUW_Main : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeOnInitialized()override;

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
	void Init(APawn* NewPawn);
	FORCEINLINE UUW_QuickSlot* GetQuickSlot() const { return QuickSlot; }
	FORCEINLINE UUW_Orb* GetHealthOrb() const { return HealthOrb; }
	FORCEINLINE UUW_Orb* GetManaOrb() const { return ManaOrb; }
};
