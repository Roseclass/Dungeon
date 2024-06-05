#include "Widgets/SkillButton.h"
#include "Global.h"
#include "Characters/GABase.h"

void USkillButton::OnButtonClicked()
{
	OnSkillButtonClicked.ExecuteIfBound(this);
}

void USkillButton::OnButtonHovered()
{
	OnSkillButtonHovered.ExecuteIfBound(this);
}

void USkillButton::OnButtonUnhovered()
{
	OnSkillButtonUnhovered.ExecuteIfBound(this);
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

void USkillButton::Init(const FSkillData& InSkillData)
{
	SkillID = InSkillData.SkillClass.GetDefaultObject()->GetSkillID();

	Position = InSkillData.PannelPosition;

	WidgetStyle.Normal.SetResourceObject(InSkillData.SkillImage);
	WidgetStyle.Hovered.SetResourceObject(InSkillData.SkillImage);
	WidgetStyle.Pressed.SetResourceObject(InSkillData.SkillImage);
	WidgetStyle.Disabled.SetResourceObject(InSkillData.SkillImage);
	WidgetStyle.Normal.TintColor = FSlateColor(FLinearColor(1, 1, 1, 1));
	WidgetStyle.Disabled.TintColor = FSlateColor(FLinearColor(1, 1, 1, 0.3));

	OnClicked.AddDynamic(this, &USkillButton::OnButtonClicked);
	OnHovered.AddDynamic(this, &USkillButton::OnButtonHovered);
	OnUnhovered.AddDynamic(this, &USkillButton::OnButtonUnhovered);

	////태그를 추가
	//SkillActor->OnLocked.AddUFunction(this, "OnButtonLocked");
	//SkillActor->OnUnlocked.AddUFunction(this, "OnButtonUnlocked");
	//SkillActor->OnAcquired.AddUFunction(this, "OnButtonAcquired");
}