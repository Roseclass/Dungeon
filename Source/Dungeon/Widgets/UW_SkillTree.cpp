#include "Widgets/UW_SkillTree.h"
#include "Global.h"
#include "Components/ScaleBox.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ScaleBoxSlot.h"
#include "Components/CanvasPanel.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/SlateBlueprintLibrary.h"

#include "Components/SkillTreeComponent.h"
#include "Components/SkillComponent.h"
#include "Widgets/SkillButton.h"
#include "Widgets/UW_SkillTreePopup.h"
#include "Widgets/UW_SkillInfo.h"

void UUW_SkillTree::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

int32 UUW_SkillTree::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled)const
{
	int32 result = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	FPaintContext context = FPaintContext(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	if (!OwnerComponent || !bInit)return result;

	TArray<FVector2D> rootDatas = OwnerComponent->GetRootDatas();
	TMap<FVector2D, TArray<FVector2D>> treeDatas = OwnerComponent->GetTreeDatas();

	//트리구조에 맞게 선그리기
	for (auto i : rootDatas)
	{
		TQueue<FVector2D> q;
		q.Enqueue(i);
		while (!q.IsEmpty())
		{
			FVector2D parent;q.Dequeue(parent);
			if (!treeDatas.Contains(parent))continue;

			TArray<FVector2D> children = treeDatas[parent];
			for (auto child : children)
			{
				q.Enqueue(child);
				UScaleBox* a = Icons[parent];
				UScaleBox* b = Icons[child];

				FVector2D parentPos = parent;
				FVector2D childPos = child;
				
				//x가 크면 우짝
				if (parent.X < child.X && parent.Y >= child.Y)
				{
					parentPos = USlateBlueprintLibrary::GetLocalTopLeft(a->GetPaintSpaceGeometry());
					parentPos.X += USlateBlueprintLibrary::GetLocalSize(a->GetPaintSpaceGeometry()).X;
					parentPos.Y += USlateBlueprintLibrary::GetLocalSize(a->GetPaintSpaceGeometry()).Y * 0.5;

					childPos = USlateBlueprintLibrary::GetLocalTopLeft(b->GetPaintSpaceGeometry());
					childPos.Y += USlateBlueprintLibrary::GetLocalSize(b->GetPaintSpaceGeometry()).Y * 0.5;
				}
				//y가 크면 밑으로
				else if (parent.X >= child.X && parent.Y < child.Y)
				{
					parentPos = USlateBlueprintLibrary::GetLocalTopLeft(a->GetPaintSpaceGeometry());
					parentPos.X += USlateBlueprintLibrary::GetLocalSize(a->GetPaintSpaceGeometry()).X * 0.5;
					parentPos.Y += USlateBlueprintLibrary::GetLocalSize(a->GetPaintSpaceGeometry()).Y;

					childPos = USlateBlueprintLibrary::GetLocalTopLeft(b->GetPaintSpaceGeometry());
					childPos.X += USlateBlueprintLibrary::GetLocalSize(b->GetPaintSpaceGeometry()).X * 0.5;
				}
				else
					continue;
				UWidgetBlueprintLibrary::DrawLine(context, parentPos, childPos, FLinearColor::White, 1, 3);
			}
		}
	}

	return result;
}

void UUW_SkillTree::OnButtonClicked(USkillButton* InButton)
{
	int32 skillID = InButton->GetSkillID();
	CheckFalse(skillID > -1);

	ESkillTreeSkillState state = OwnerComponent->GetSkillTreeSkillState(skillID);
	CheckTrue(state == ESkillTreeSkillState::Locked);

	if (state == ESkillTreeSkillState::Unlocked)Popup->UnlockedMode();
	else if (state == ESkillTreeSkillState::Acquired_Max)Popup->MaxAcquiredMode();
	else Popup->AcquiredMode();

	{
		FGeometry geo = UWidgetLayoutLibrary::GetViewportWidgetGeometry(GetWorld());
		FVector2D mousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
		mousePos /= geo.GetLocalSize();
		UCanvasPanelSlot* slot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Popup);
		FVector2D anchSize = slot->GetAnchors().Maximum - slot->GetAnchors().Minimum;
		slot->SetAnchors(FAnchors(mousePos.X, mousePos.Y - anchSize.Y, mousePos.X + anchSize.X, mousePos.Y));
		Popup->SetSkillID(skillID);
		Popup->SetVisibility(ESlateVisibility::Visible);
	}




	// 팝업
	// -스킬레벨업	<< 스킬의 상태가 acquired10인경우 잠금
	// -1슬롯지정
	// -2슬롯지정..	<< 스킬의 상태가 unlocked이하인경우 잠금
	//
	// 스킬 레벨업
	// 레벨업클릭(클라)
	// 
	// 레벨업(서버)
	// 어트리뷰트 체크, 스킬상태변경
	// 
	// 위젯 갱신(클라)
	//
}

void UUW_SkillTree::OnButtonHovered(USkillButton* InButton)
{
	CheckNull(InButton);
	Info->On(InButton->GetSkillID());
}

void UUW_SkillTree::OnButtonUnhovered(USkillButton* InButton)
{
	Info->Off();
}

void UUW_SkillTree::Init(const TArray<const FSkillData*>& Array, USkillTreeComponent* InSkillTreeComp, USkillComponent* InSkillComp, TFunction<void(int32, int32)> OnPopupClicked)
{
	OwnerComponent = InSkillTreeComp;
	Info->SetOwnerComponent(InSkillComp);

	//가로세로 최대수 구하기
	int32 h = 0, v = 0;
	for (auto i : Array)
	{
		FVector2D pos = i->PannelPosition;
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
		USkillButton* button = NewObject<USkillButton>(GetOwningPlayer(), USkillButton::StaticClass());
		button->Init(*i);
		button->OnSkillButtonClicked.BindUFunction(this, "OnButtonClicked");
		button->OnSkillButtonHovered.BindUFunction(this, "OnButtonHovered");
		button->OnSkillButtonUnhovered.BindUFunction(this, "OnButtonUnhovered");

		UScaleBox* scale = NewObject<UScaleBox>(GetOwningPlayer(), UScaleBox::StaticClass());
		scale->AddChild(button);
		BaseCanvasPanel->AddChild(scale);
		
		UScaleBoxSlot* scaleslot = UWidgetLayoutLibrary::SlotAsScaleBoxSlot(button);
		scaleslot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
		scaleslot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);

		UCanvasPanelSlot* slot = UWidgetLayoutLibrary::SlotAsCanvasSlot(scale);
		slot->SetAutoSize(1);
		FVector2D pos = i->PannelPosition;
		FVector2D anch = pos * gap;
		anch += BackgroundLeftTopAnchor;
		anch += FVector2D(IconSize, IconSize * 1.77777) * FVector2D(pos.X - 1, pos.Y - 1);
		slot->SetAnchors(FAnchors(anch.X, anch.Y, anch.X + IconSize, anch.Y + (IconSize * 1.77777)));
		slot->SetOffsets(FMargin(0));

		Icons.Add(i->PannelPosition, scale);
		Buttons.Add(i->PannelPosition, button);
	}

	//팝업 버튼 클릭 함수 바인딩
	Popup->OnPopupButtonClicked.AddLambda(OnPopupClicked);
	Popup->OnLevelUpButtonClicked.AddUFunction(OwnerComponent, "LevelUp");

	bInit = 1;
}

void UUW_SkillTree::UpdateButtonState(int32 InSkillID, ESkillTreeSkillState NewState)
{
	for (auto i : Buttons)
		if (i.Value && i.Value->GetSkillID() == InSkillID)
			i.Value->Update(NewState);
}
