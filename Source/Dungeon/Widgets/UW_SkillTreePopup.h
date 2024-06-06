#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_SkillTreePopup.generated.h"

/**
 * 
 */

class UButton;
class ASkillActor;

DECLARE_MULTICAST_DELEGATE_TwoParams(FPopupButtonClicked, int32, int32);

UCLASS()
class DUNGEON_API UUW_SkillTreePopup : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeOnInitialized()override;
public:

	//property
private:
	int32 SkillID;
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* Button0;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* Button1;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* Button2;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* Button3;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* Button4;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* Button5;

public:
	FPopupButtonClicked OnPopupButtonClicked;

	//function
private:
	UFUNCTION() void OnButton0Clicked();
	UFUNCTION() void OnButton1Clicked();
	UFUNCTION() void OnButton2Clicked();
	UFUNCTION() void OnButton3Clicked();
	UFUNCTION() void OnButton4Clicked();
	UFUNCTION() void OnButton5Clicked();
protected:
public:

	FORCEINLINE void SetSkillID(int32 InSkillID) { SkillID = InSkillID; }
};
