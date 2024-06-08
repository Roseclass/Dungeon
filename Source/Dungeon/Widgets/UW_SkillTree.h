#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_SkillTree.generated.h"

/**
 * ��ų�� ���� �����Կ� ��ġ ����
 */

class UCanvasPanel;
class UBorder;
class UScaleBox;
class USkillButton;
class USkillTreeComponent;
class USkillComponent;
class UUW_SkillTreePopup;
class UUW_SkillInfo;

struct FSkillData;
enum class ESkillTreeSkillState : uint8;

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
	bool bInit;
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
	TMap<FVector2D, USkillButton*> Buttons;
public:

	//function
private:
	UFUNCTION() void OnButtonClicked(USkillButton* InButton);
	UFUNCTION() void OnButtonHovered(USkillButton* InButton);
	UFUNCTION() void OnButtonUnhovered(USkillButton* InButton);
protected:
public:
	void Init(const TArray<const FSkillData*>& Array, USkillTreeComponent* InSkillTreeComp, USkillComponent* InSkillComp, TFunction<void(int32, int32)> OnPopupClicked);
	void UpdateButtonState(int32 InSkillID, ESkillTreeSkillState NewState);
};

