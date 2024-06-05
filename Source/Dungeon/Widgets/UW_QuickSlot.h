#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/SkillComponent.h"
#include "UW_QuickSlot.generated.h"

/**
 * 
 */

class UTexture;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class UTextBlock;
class USkillButton;
class USkillComponent;

UCLASS()
class DUNGEON_API UUW_QuickSlot : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized()override;
	virtual void NativeConstruct()override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime)override;

public:

	//property
private:
	USkillComponent* OwnerComponent;
	FDelegateHandle OnQuickSlotDataChangedHandle;
	FDelegateHandle OnQuickSlotCoolDownHandle;
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		USkillButton* Slot0;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		USkillButton* Slot1;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		USkillButton* Slot2;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		USkillButton* Slot3;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		USkillButton* Slot4;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		USkillButton* Slot5;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* Timer0;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* Timer1;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* Timer2;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* Timer3;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* Timer4;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* Timer5;

	UPROPERTY(EditDefaultsOnly, meta = (DisplayThumbnail = "true"))
		UMaterialInterface* SlotMaterial;

	UPROPERTY(EditDefaultsOnly, meta = (DisplayThumbnail = "true"))
		UTexture* DefaultIcon;

	TArray<USkillButton*> Slots;
	TArray<UTextBlock*> Timers;
	TArray<UMaterialInstanceDynamic*> Materials;
	TArray<float> RemainingCoolDowns;
public:

	//function
private:
	UFUNCTION()void OnQuickSlotDataChanged(int32 InQuickSlotIndex, const FSkillData& InSkillData);
	UFUNCTION()void OnQuickSlotCoolDown(int32 Index, float Time);
protected:
public:
	void ConnectComponent(USkillComponent* InSkillComponent);
};
