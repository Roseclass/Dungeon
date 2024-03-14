#include "Widgets/UW_QuickSlot.h"
#include "Global.h"
#include "Kismet/KismetMaterialLibrary.h" 
#include "Kismet/KismetTextLibrary.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Components/SkillComponent.h"
#include "Widgets/SkillButton.h"
#include "Objects/SkillActor.h"

void UUW_QuickSlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UUW_QuickSlot::NativeConstruct()
{
	Super::NativeConstruct();

	RemainingCoolDowns.Init(0, 6);
	Slots.Add(Slot0);Slots.Add(Slot1);Slots.Add(Slot2);
	Slots.Add(Slot3);Slots.Add(Slot4);Slots.Add(Slot5);
	Timers.Add(Timer0); Timers.Add(Timer1); Timers.Add(Timer2);
	Timers.Add(Timer3); Timers.Add(Timer4); Timers.Add(Timer5);
	for (auto i : Slots)
	{
		UMaterialInstanceDynamic* dynamicMat = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), SlotMaterial, NAME_None, EMIDCreationFlags::None);
		if (!dynamicMat)continue;
		Materials.Add(dynamicMat);
		i->SetIsEnabled(0);
		i->WidgetStyle.Normal.SetResourceObject(dynamicMat);
		i->WidgetStyle.Hovered.SetResourceObject(dynamicMat);
		i->WidgetStyle.Pressed.SetResourceObject(dynamicMat);
		i->WidgetStyle.Disabled.SetResourceObject(dynamicMat);
		i->WidgetStyle.Normal.TintColor = FSlateColor(FLinearColor(0, 0, 0, 0.8));
		i->WidgetStyle.Disabled.TintColor = FSlateColor(FLinearColor(0, 0, 0, 0.8));
	}
}

void UUW_QuickSlot::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (OwnerComponent)
	{
		for (int32 i = 0; i < Materials.Num(); ++i)
		{
			if (!RemainingCoolDowns.IsValidIndex(i))break;
			RemainingCoolDowns[i] -= InDeltaTime;
			if (RemainingCoolDowns[i] < 0)RemainingCoolDowns[i] = 0;

			float cur = RemainingCoolDowns[i], max = 0;
			bool flag = OwnerComponent->GetQuickSlotCoolDown(i, max);
			cur = max - cur;

			if (max <= 0 || !flag || RemainingCoolDowns[i] <= 0)
			{
				Materials[i]->SetScalarParameterValue("Progress", 0);
				Timers[i]->SetText(FText());
			}
			else
			{
				Materials[i]->SetScalarParameterValue("Progress", cur / max);
				FText time;
				if (max - cur >= max)time = FText();
				else if (max - cur < 10)time = UKismetTextLibrary::Conv_FloatToText(max - cur, ERoundingMode::HalfToEven, 0, 1, 1, 1, 1, 1);
				else time = UKismetTextLibrary::Conv_FloatToText(max - cur, ERoundingMode::HalfToEven, 0, 1, 1, 3, 0, 0);
				Timers[i]->SetText(time);
			}
		}
	}
}

void UUW_QuickSlot::OnQuickSlotDataChanged(int32 Index, ASkillActor* InSkillActor)
{
	CheckFalse(Slots.IsValidIndex(Index));
	if (InSkillActor)
	{
		Slots[Index]->SetIsEnabled(1);
		Slots[Index]->WidgetStyle.Normal.TintColor = FSlateColor(FLinearColor(1, 1, 1, 1));
		Slots[Index]->WidgetStyle.Disabled.TintColor = FSlateColor(FLinearColor(1, 1, 1, 1));
		UTexture* icon = Cast<UTexture>(InSkillActor->GetSkillData()->SkillImage);
		Materials[Index]->SetTextureParameterValue("Icon", icon);
	}
	else
	{ 
		Slots[Index]->SetIsEnabled(0);
		Slots[Index]->WidgetStyle.Normal.TintColor = FSlateColor(FLinearColor(0, 0, 0, 0.8));
		Slots[Index]->WidgetStyle.Disabled.TintColor = FSlateColor(FLinearColor(0, 0, 0, 0.8));
		Materials[Index]->SetTextureParameterValue("Icon", DefaultIcon);
	}
}

void UUW_QuickSlot::OnQuickSlotCoolDown(int32 Index, float Time)
{
	CheckFalse(RemainingCoolDowns.IsValidIndex(Index));
	RemainingCoolDowns[Index] = Time;
}

void UUW_QuickSlot::ConnectComponent(USkillComponent* InSkillComponent)
{
	if (OwnerComponent)
	{
		OwnerComponent->OnQuickSlotDataChanged.Remove(OnQuickSlotDataChangedHandle);
		OwnerComponent->OnQuickSlotCoolDown.Remove(OnQuickSlotCoolDownHandle);
	}

	OwnerComponent = InSkillComponent;
	OnQuickSlotDataChangedHandle = OwnerComponent->OnQuickSlotDataChanged.AddUFunction(this, "OnQuickSlotDataChanged");
	OnQuickSlotCoolDownHandle = OwnerComponent->OnQuickSlotCoolDown.AddUFunction(this, "OnQuickSlotCoolDown");
}
