#include "Widgets/SkillButton.h"
#include "Global.h"

#include "Objects/SkillActor.h"

void USkillButton::OnButtonClicked()
{
	OnSkillButtonClicked.ExecuteIfBound(this);
}

void USkillButton::OnButtonLocked()
{
	SetIsEnabled(0);
}

void USkillButton::OnButtonUnlocked()
{
	SetIsEnabled(1);
	WidgetStyle.Normal.TintColor = FSlateColor(FLinearColor(1, 1, 1, 0.3));
}

void USkillButton::OnButtonAcquired()
{
	SetIsEnabled(1);
	WidgetStyle.Normal.TintColor = FSlateColor(FLinearColor(1, 1, 1, 1));
}

void USkillButton::Init(ASkillActor* InSkillActor)
{
	SkillActor = InSkillActor;
	Position = InSkillActor->GetSkillData()->PannelPosition;

	WidgetStyle.Normal.SetResourceObject(InSkillActor->GetSkillData()->SkillImage);
	WidgetStyle.Hovered.SetResourceObject(InSkillActor->GetSkillData()->SkillImage);
	WidgetStyle.Pressed.SetResourceObject(InSkillActor->GetSkillData()->SkillImage);
	WidgetStyle.Disabled.SetResourceObject(InSkillActor->GetSkillData()->SkillImage);
	WidgetStyle.Normal.TintColor = FSlateColor(FLinearColor(1, 1, 1, 1));
	WidgetStyle.Disabled.TintColor = FSlateColor(FLinearColor(1, 1, 1, 0.3));

	OnClicked.AddDynamic(this, &USkillButton::OnButtonClicked);

	SkillActor->OnLocked.AddUFunction(this, "OnButtonLocked");
	SkillActor->OnUnlocked.AddUFunction(this, "OnButtonUnlocked");
	SkillActor->OnAcquired.AddUFunction(this, "OnButtonAcquired");
}