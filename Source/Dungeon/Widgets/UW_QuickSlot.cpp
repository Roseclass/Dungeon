#include "Widgets/UW_QuickSlot.h"
#include "Global.h"
#include "Widgets/SkillButton.h"

#include "Components/SkillComponent.h"
#include "Objects/SkillActor.h"

void UUW_QuickSlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ResetSlot(Slot0);
	ResetSlot(Slot1);
	ResetSlot(Slot2);
	ResetSlot(Slot3);
	ResetSlot(Slot4);
	ResetSlot(Slot5);
}

void UUW_QuickSlot::OnQuickSlotDataChanged(int32 Index, ASkillActor* InSkillActor)
{
	USkillButton* button = nullptr;
	switch (Index)
	{
	case 0:button=Slot0;break;
	case 1:button=Slot1;break;
	case 2:button=Slot2;break;
	case 3:button=Slot3;break;
	case 4:button=Slot4;break;
	case 5:button=Slot5;break;
	default:break;
	}
	CheckNull(button);
	if (InSkillActor)
	{
		button->SetIsEnabled(1);
		button->WidgetStyle.Normal.SetResourceObject(InSkillActor->GetSkillData()->SkillImage);
		button->WidgetStyle.Hovered.SetResourceObject(InSkillActor->GetSkillData()->SkillImage);
		button->WidgetStyle.Pressed.SetResourceObject(InSkillActor->GetSkillData()->SkillImage);
		button->WidgetStyle.Disabled.SetResourceObject(InSkillActor->GetSkillData()->SkillImage);
		button->WidgetStyle.Normal.TintColor = FSlateColor(FLinearColor(1, 1, 1, 1));
		button->WidgetStyle.Disabled.TintColor = FSlateColor(FLinearColor(1, 1, 1, 1));
	}
	else
	{ 
		ResetSlot(button);
	}
}

void UUW_QuickSlot::ResetSlot(USkillButton* InSlot)
{
	InSlot->SetIsEnabled(0);
	InSlot->WidgetStyle.Normal.SetResourceObject(nullptr);
	InSlot->WidgetStyle.Hovered.SetResourceObject(nullptr);
	InSlot->WidgetStyle.Pressed.SetResourceObject(nullptr);
	InSlot->WidgetStyle.Disabled.SetResourceObject(nullptr);
	InSlot->WidgetStyle.Normal.TintColor = FSlateColor(FLinearColor(0, 0, 0, 0.5));
	InSlot->WidgetStyle.Disabled.TintColor = FSlateColor(FLinearColor(0, 0, 0, 0.5));
}

void UUW_QuickSlot::ConnectComponent(USkillComponent* InSkillComponent)
{
	if (OwnerComponent)
	{
		OwnerComponent->OnQuickSlotDataChanged.Remove(OnQuickSlotDataChangedHandle);
	}

	OwnerComponent = InSkillComponent;
	OnQuickSlotDataChangedHandle = OwnerComponent->OnQuickSlotDataChanged.AddUFunction(this, "OnQuickSlotDataChanged");
}
