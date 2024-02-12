#include "Widgets/UW_SkillTree.h"
#include "Global.h"
#include "Components/Image.h"
#include "Components/ScaleBox.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ScaleBoxSlot.h"
#include "Components/CanvasPanel.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"

#include "Objects/SkillActor.h"

void UUW_SkillTree::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

int32 UUW_SkillTree::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled)const
{
	int32 result = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	FPaintContext context = FPaintContext(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	for(auto i : RootActors)
	{
		//while (1)
		//{
		//	for (auto child : i->GetSkillData()->Children)
		//	{
		//		//UWidgetBlueprintLibrary::DrawLine(context, topleft + i.Start, topleft + i.End, FLinearColor(0.5, 0.5, 0.5, 0.5));
		//	}
		//} 부모를 향해 선그리기
	}

	return result;
}

void UUW_SkillTree::Init(const TArray<ASkillActor*>& Array)
{
	//가로세로 최대수, 루트구하기
	int32 h = 0, v = 0;
	for (auto i : Array)
	{
		if (!i->GetParent())
			RootActors.Add(i);
		FVector2D pos = i->GetSkillData()->PannelPosition;
		h = UKismetMathLibrary::Max(h, pos.X);
		v = UKismetMathLibrary::Max(v, pos.Y);
	}

	//간격 계산
	FVector2D gap = BackgroundRightBottomAnchor - BackgroundLeftTopAnchor;
	gap.X -= h * IconSize; gap.Y -= v * IconSize * 1.77777;
	gap.X /= (h + 1); gap.Y /= (v + 1);

	//레이아웃 배치
	for (auto i : Array)
	{
		UScaleBox* scale = NewObject<UScaleBox>(GetOwningPlayer(), UScaleBox::StaticClass());
		UImage* image = NewObject<UImage>(GetOwningPlayer(), UImage::StaticClass());
		image->SetBrushResourceObject(i->GetSkillData()->SkillImage);
		scale->AddChild(image);
		BaseCanvasPanel->AddChild(scale);
		Icons.Add(scale);
		
		UScaleBoxSlot* scaleslot = UWidgetLayoutLibrary::SlotAsScaleBoxSlot(image);
		scaleslot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
		scaleslot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);

		UCanvasPanelSlot* slot = UWidgetLayoutLibrary::SlotAsCanvasSlot(scale);
		slot->SetAutoSize(1);
		FVector2D pos = i->GetSkillData()->PannelPosition;
		FVector2D anch = pos * gap;
		anch += BackgroundLeftTopAnchor;
		anch += FVector2D(IconSize, IconSize * 1.77777) * FVector2D(pos.X - 1, pos.Y - 1);
		slot->SetAnchors(FAnchors(anch.X, anch.Y, anch.X + IconSize, anch.Y + (IconSize * 1.77777)));
		slot->SetOffsets(FMargin(0));//기본 오프셋 존재를 모르고 한참 걸림;
	}

}
