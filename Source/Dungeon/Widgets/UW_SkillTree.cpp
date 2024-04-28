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
#include "Objects/SkillActor.h"
#include "Widgets/SkillButton.h"
#include "Widgets/UW_SkillTreePopup.h"

void UUW_SkillTree::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

int32 UUW_SkillTree::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled)const
{
	int32 result = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	FPaintContext context = FPaintContext(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	if (!OwnerComponent)return result;

	const TArray<ASkillActor*>& rootActors = OwnerComponent->GetRootActors();

	//트리구조에 맞게 선그리기
	for (auto i : rootActors)
	{
		TQueue<ASkillActor*> q;
		q.Enqueue(i);
		while (!q.IsEmpty())
		{
			ASkillActor* parent;
			q.Dequeue(parent);
			for (auto child : parent->GetChildren())
			{
				q.Enqueue(child);
				FVector2D parentPos = parent->GetSkillData()->PannelPosition;
				FVector2D childPos = child->GetSkillData()->PannelPosition;
				UScaleBox* a = Icons[parentPos];
				UScaleBox* b = Icons[childPos];
				//x가 크면 우짝
				if (parentPos.X < childPos.X && parentPos.Y >= childPos.Y)
				{
					parentPos = USlateBlueprintLibrary::GetLocalTopLeft(a->GetPaintSpaceGeometry());
					parentPos.X += USlateBlueprintLibrary::GetLocalSize(a->GetPaintSpaceGeometry()).X;
					parentPos.Y += USlateBlueprintLibrary::GetLocalSize(a->GetPaintSpaceGeometry()).Y * 0.5;

					childPos = USlateBlueprintLibrary::GetLocalTopLeft(b->GetPaintSpaceGeometry());
					childPos.Y += USlateBlueprintLibrary::GetLocalSize(b->GetPaintSpaceGeometry()).Y * 0.5;
				}
				//y가 크면 밑으로
				else if (parentPos.X >= childPos.X && parentPos.Y < childPos.Y)
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
	ASkillActor* skill = InButton->GetSkillActor();
	CheckNull(skill);
	if (skill->GetSkillTreeState() == ESkillTreeSkillState::Unlocked)
	{
		//조건이 더 필요함 ex)스킬포인트..
		OwnerComponent->Acquire(skill);
	}
	else if (skill->GetSkillTreeState() == ESkillTreeSkillState::Acquired)
	{
		FGeometry geo = UWidgetLayoutLibrary::GetViewportWidgetGeometry(GetWorld());
		FVector2D mousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
		mousePos /= geo.GetLocalSize();
		UCanvasPanelSlot* slot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Popup);
		FVector2D anchSize = slot->GetAnchors().Maximum - slot->GetAnchors().Minimum;
		slot->SetAnchors(FAnchors(mousePos.X, mousePos.Y - anchSize.Y, mousePos.X + anchSize.X, mousePos.Y));
		Popup->SetSkillActor(skill);
		Popup->SetVisibility(ESlateVisibility::Visible);
	}
}

void UUW_SkillTree::Init(const TArray<ASkillActor*>& Array, USkillTreeComponent* InSkillTreeComp, TFunction<void(int32, ASkillActor*)> OnPopupClicked)
{
	OwnerComponent = InSkillTreeComp;

	//가로세로 최대수 구하기
	int32 h = 0, v = 0;
	for (auto i : Array)
	{
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
		USkillButton* button = NewObject<USkillButton>(GetOwningPlayer(), USkillButton::StaticClass());
		button->Init(i);
		button->OnSkillButtonClicked.BindUFunction(this, "OnButtonClicked");

		UScaleBox* scale = NewObject<UScaleBox>(GetOwningPlayer(), UScaleBox::StaticClass());
		scale->AddChild(button);
		BaseCanvasPanel->AddChild(scale);
		
		UScaleBoxSlot* scaleslot = UWidgetLayoutLibrary::SlotAsScaleBoxSlot(button);
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

		Icons.Add(i->GetSkillData()->PannelPosition, scale);
		if(i->HasAuthority())i->Load();
	}

	//팝업 버튼 클릭 함수 바인딩
	Popup->OnPopupButtonClicked.AddLambda(OnPopupClicked);
}