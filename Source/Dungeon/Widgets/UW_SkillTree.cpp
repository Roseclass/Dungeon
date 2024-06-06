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

	if (!OwnerComponent)return result;

	//const TArray<ASkillActor*>& rootActors = OwnerComponent->GetRootActors();

	////Ʈ�������� �°� ���׸���
	//for (auto i : rootActors)
	//{
	//	TQueue<ASkillActor*> q;
	//	q.Enqueue(i);
	//	while (!q.IsEmpty())
	//	{
	//		ASkillActor* parent;
	//		q.Dequeue(parent);
	//		for (auto child : parent->GetChildren())
	//		{
	//			q.Enqueue(child);
	//			FVector2D parentPos = parent->GetSkillData()->PannelPosition;
	//			FVector2D childPos = child->GetSkillData()->PannelPosition;
	//			UScaleBox* a = Icons[parentPos];
	//			UScaleBox* b = Icons[childPos];
	//			//x�� ũ�� ��¦
	//			if (parentPos.X < childPos.X && parentPos.Y >= childPos.Y)
	//			{
	//				parentPos = USlateBlueprintLibrary::GetLocalTopLeft(a->GetPaintSpaceGeometry());
	//				parentPos.X += USlateBlueprintLibrary::GetLocalSize(a->GetPaintSpaceGeometry()).X;
	//				parentPos.Y += USlateBlueprintLibrary::GetLocalSize(a->GetPaintSpaceGeometry()).Y * 0.5;

	//				childPos = USlateBlueprintLibrary::GetLocalTopLeft(b->GetPaintSpaceGeometry());
	//				childPos.Y += USlateBlueprintLibrary::GetLocalSize(b->GetPaintSpaceGeometry()).Y * 0.5;
	//			}
	//			//y�� ũ�� ������
	//			else if (parentPos.X >= childPos.X && parentPos.Y < childPos.Y)
	//			{
	//				parentPos = USlateBlueprintLibrary::GetLocalTopLeft(a->GetPaintSpaceGeometry());
	//				parentPos.X += USlateBlueprintLibrary::GetLocalSize(a->GetPaintSpaceGeometry()).X * 0.5;
	//				parentPos.Y += USlateBlueprintLibrary::GetLocalSize(a->GetPaintSpaceGeometry()).Y;

	//				childPos = USlateBlueprintLibrary::GetLocalTopLeft(b->GetPaintSpaceGeometry());
	//				childPos.X += USlateBlueprintLibrary::GetLocalSize(b->GetPaintSpaceGeometry()).X * 0.5;
	//			}
	//			else
	//				continue;
	//			UWidgetBlueprintLibrary::DrawLine(context, parentPos, childPos, FLinearColor::White, 1, 3);
	//		}
	//	}
	//}

	return result;
}

void UUW_SkillTree::OnButtonClicked(USkillButton* InButton)
{
	//ASkillActor* skill = InButton->GetSkillActor();
	//CheckNull(skill);
	//if (skill->GetSkillTreeState() == ESkillTreeSkillState::Unlocked)
	//{
	//	//������ �� �ʿ��� ex)��ų����Ʈ..
	//	OwnerComponent->Acquire(skill);
	//}
	//else if (skill->GetSkillTreeState() == ESkillTreeSkillState::Acquired)
	//{
	//	FGeometry geo = UWidgetLayoutLibrary::GetViewportWidgetGeometry(GetWorld());
	//	FVector2D mousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
	//	mousePos /= geo.GetLocalSize();
	//	UCanvasPanelSlot* slot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Popup);
	//	FVector2D anchSize = slot->GetAnchors().Maximum - slot->GetAnchors().Minimum;
	//	slot->SetAnchors(FAnchors(mousePos.X, mousePos.Y - anchSize.Y, mousePos.X + anchSize.X, mousePos.Y));
	//	Popup->SetSkillActor(skill);
	//	Popup->SetVisibility(ESlateVisibility::Visible);
	//}
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

void UUW_SkillTree::Init(const TArray<FSkillData*>& Array, USkillTreeComponent* InSkillTreeComp, USkillComponent* InSkillComp, TFunction<void(int32, const FSkillData&)> OnPopupClicked)
{
	OwnerComponent = InSkillTreeComp;
	Info->SetOwnerComponent(InSkillComp);

	//���μ��� �ִ�� ���ϱ�
	int32 h = 0, v = 0;
	for (auto i : Array)
	{
		FVector2D pos = i->PannelPosition;
		h = UKismetMathLibrary::Max(h, pos.X);
		v = UKismetMathLibrary::Max(v, pos.Y);
	}

	//���� ���
	FVector2D gap = BackgroundRightBottomAnchor - BackgroundLeftTopAnchor;
	gap.X -= h * IconSize; gap.Y -= v * IconSize * 1.77777;
	gap.X /= (h + 1); gap.Y /= (v + 1);

	//���̾ƿ� ��ġ
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
	}

	////�˾� ��ư Ŭ�� �Լ� ���ε�
	//Popup->OnPopupButtonClicked.AddLambda(OnPopupClicked);
}