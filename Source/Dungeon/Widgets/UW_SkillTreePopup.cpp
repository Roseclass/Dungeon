#include "Widgets/UW_SkillTreePopup.h"
#include "Global.h"

#include "Components/Button.h"

void UUW_SkillTreePopup::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	LevelUp->OnClicked.AddDynamic(this, &UUW_SkillTreePopup::OnLevelUp);
	Button0->OnClicked.AddDynamic(this, &UUW_SkillTreePopup::OnButton0Clicked);
	Button1->OnClicked.AddDynamic(this, &UUW_SkillTreePopup::OnButton1Clicked);
	Button2->OnClicked.AddDynamic(this, &UUW_SkillTreePopup::OnButton2Clicked);
	Button3->OnClicked.AddDynamic(this, &UUW_SkillTreePopup::OnButton3Clicked);
	Button4->OnClicked.AddDynamic(this, &UUW_SkillTreePopup::OnButton4Clicked);
	Button5->OnClicked.AddDynamic(this, &UUW_SkillTreePopup::OnButton5Clicked);
}

void UUW_SkillTreePopup::OnLevelUp()
{
	OnLevelUpButtonClicked.Broadcast(SkillID);
	SkillID = -1;
	SetVisibility(ESlateVisibility::Hidden);
}

void UUW_SkillTreePopup::OnButton0Clicked()
{
	OnPopupButtonClicked.Broadcast(0, SkillID);
	SkillID = -1;
	SetVisibility(ESlateVisibility::Hidden);
}

void UUW_SkillTreePopup::OnButton1Clicked()
{
	OnPopupButtonClicked.Broadcast(1, SkillID);
	SkillID = -1;
	SetVisibility(ESlateVisibility::Hidden);
}

void UUW_SkillTreePopup::OnButton2Clicked()
{
	OnPopupButtonClicked.Broadcast(2, SkillID);
	SkillID = -1;
	SetVisibility(ESlateVisibility::Hidden);
}

void UUW_SkillTreePopup::OnButton3Clicked()
{
	OnPopupButtonClicked.Broadcast(3, SkillID);
	SkillID = -1;
	SetVisibility(ESlateVisibility::Hidden);
}

void UUW_SkillTreePopup::OnButton4Clicked()
{
	OnPopupButtonClicked.Broadcast(4, SkillID);
	SkillID = -1;
	SetVisibility(ESlateVisibility::Hidden);
}

void UUW_SkillTreePopup::OnButton5Clicked()
{
	OnPopupButtonClicked.Broadcast(5, SkillID);
	SkillID = -1;
	SetVisibility(ESlateVisibility::Hidden);
}

void UUW_SkillTreePopup::UnlockedMode()
{
	// hide level up button

	// 
	Button0->SetVisibility(ESlateVisibility::Collapsed);
	Button1->SetVisibility(ESlateVisibility::Collapsed);
	Button2->SetVisibility(ESlateVisibility::Collapsed);
	Button3->SetVisibility(ESlateVisibility::Collapsed);
	Button4->SetVisibility(ESlateVisibility::Collapsed);
	Button5->SetVisibility(ESlateVisibility::Collapsed);
}

void UUW_SkillTreePopup::AcquiredMode()
{
	// hide level up button

	// 
	Button0->SetVisibility(ESlateVisibility::Visible);
	Button1->SetVisibility(ESlateVisibility::Visible);
	Button2->SetVisibility(ESlateVisibility::Visible);
	Button3->SetVisibility(ESlateVisibility::Visible);
	Button4->SetVisibility(ESlateVisibility::Visible);
	Button5->SetVisibility(ESlateVisibility::Visible);
}

void UUW_SkillTreePopup::MaxAcquiredMode()
{
	// hide level up button

	//
	Button0->SetVisibility(ESlateVisibility::Collapsed);
	Button1->SetVisibility(ESlateVisibility::Collapsed);
	Button2->SetVisibility(ESlateVisibility::Collapsed);
	Button3->SetVisibility(ESlateVisibility::Collapsed);
	Button4->SetVisibility(ESlateVisibility::Collapsed);
	Button5->SetVisibility(ESlateVisibility::Collapsed);
}
