#include "Widgets/SkillButton.h"
#include "Global.h"
#include "Abilities/GABase.h"

#include "Components/SkillTreeComponent.h"

USkillButton::USkillButton()
{
	SetIsEnabled(0);
}

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
}

void USkillButton::Update(ESkillTreeSkillState NewState)
{
	if (NewState == ESkillTreeSkillState::Locked)OnButtonLocked();
	else if (NewState == ESkillTreeSkillState::Unlocked)OnButtonUnlocked();
	else OnButtonAcquired();
}
