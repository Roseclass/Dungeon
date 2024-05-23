#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_SkillTree.generated.h"

/**
 * 스킬을 배우고 퀵슬롯에 배치 가능
 */

class UCanvasPanel;
class UBorder;
class UScaleBox;
class USkillButton;
class ASkillActor;
class USkillTreeComponent;
class UUW_SkillTreePopup;
class UUW_SkillInfo;

UCLASS()
class DUNGEON_API UUW_SkillTree : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized()override;
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled)const override;
public:

	//property
private:
	USkillTreeComponent* OwnerComponent;
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UCanvasPanel* BaseCanvasPanel;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UBorder* Background;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_SkillTreePopup* Popup;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_SkillInfo* Info;

	UPROPERTY(EditDefaultsOnly, Category = "Size")
		FVector2D BackgroundLeftTopAnchor = FVector2D(0.2, 0.2);

	UPROPERTY(EditDefaultsOnly, Category = "Size")
		FVector2D BackgroundRightBottomAnchor = FVector2D(0.8, 0.8);

	UPROPERTY(EditDefaultsOnly, Category = "Size")
		float IconSize = 0.05;

	TMap<FVector2D,UScaleBox*> Icons;
public:

	//function
private:
	UFUNCTION() void OnButtonClicked(USkillButton* InButton);
	UFUNCTION() void OnButtonHovered(USkillButton* InButton);
	UFUNCTION() void OnButtonUnhovered(USkillButton* InButton);
protected:
public:
	void Init(const TArray<ASkillActor*>& Array, USkillTreeComponent* InSkillTreeComp, TFunction<void(int32, ASkillActor*)> OnPopupClicked);
};

