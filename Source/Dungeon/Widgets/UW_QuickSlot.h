#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_QuickSlot.generated.h"

/**
 * 
 */

class USkillButton;
class USkillComponent;
class ASkillActor;

UCLASS()
class DUNGEON_API UUW_QuickSlot : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized()override;
public:

	//property
private:
	USkillComponent* OwnerComponent;
	FDelegateHandle OnQuickSlotDataChangedHandle;
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

public:

	//function
private:
	UFUNCTION()void OnQuickSlotDataChanged(int32 Index, ASkillActor* InSkillActor);
	void ResetSlot(USkillButton* InSlot);
protected:
public:
	void ConnectComponent(USkillComponent* InSkillComponent);
};
