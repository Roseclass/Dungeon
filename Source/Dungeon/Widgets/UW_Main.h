#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_Main.generated.h"

/**
 * 
 */

class UUW_QuickSlot;
class UUW_Orb;
class UUW_Chat;
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

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_Chat* Chat;

public:

	//function
private:
protected:
public:	
	void Init(APawn* NewPawn);

	void HealthChanged(float NewValue);
	void MaxHealthChanged(float NewValue);
	void ManaChanged(float NewValue);
	void MaxManaChanged(float NewValue);

	void OnChat();
	void OnMessageUpdated(const FText& Text, const FLinearColor& Color);
	void OnDead();

	FORCEINLINE UUW_QuickSlot* GetQuickSlot() const { return QuickSlot; }
	FORCEINLINE UUW_Orb* GetHealthOrb() const { return HealthOrb; }
	FORCEINLINE UUW_Orb* GetManaOrb() const { return ManaOrb; }
	FORCEINLINE UUW_Chat* GetChat() const { return Chat; }
};
