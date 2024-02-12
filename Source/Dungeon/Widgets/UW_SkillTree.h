#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_SkillTree.generated.h"

/**
 * 
 */

class UCanvasPanel;
class UBorder;
class UScaleBox;
class ASkillActor;

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
	TArray<ASkillActor*>RootActors;
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UCanvasPanel* BaseCanvasPanel;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UBorder* Background;

	UPROPERTY(EditDefaultsOnly, Category = "Size")
		FVector2D BackgroundLeftTopAnchor = FVector2D(0.2, 0.2);

	UPROPERTY(EditDefaultsOnly, Category = "Size")
		FVector2D BackgroundRightBottomAnchor = FVector2D(0.8, 0.8);

	UPROPERTY(EditDefaultsOnly, Category = "Size")
		float IconSize = 0.05;

	TArray<UScaleBox*> Icons;
public:

	//function
private:
protected:
public:
	void Init(const TArray<ASkillActor*>& Array);
};
