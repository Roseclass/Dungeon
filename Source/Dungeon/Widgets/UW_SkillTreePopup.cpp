#include "Widgets/UW_SkillTreePopup.h"
#include "Global.h"

#include "Components/Button.h"

void UUW_SkillTreePopup::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	Button0->OnClicked.AddDynamic(this, &UUW_SkillTreePopup::OnButton0Clicked);
	Button1->OnClicked.AddDynamic(this, &UUW_SkillTreePopup::OnButton1Clicked);
	Button2->OnClicked.AddDynamic(this, &UUW_SkillTreePopup::OnButton2Clicked);
	Button3->OnClicked.AddDynamic(this, &UUW_SkillTreePopup::OnButton3Clicked);
	Button4->OnClicked.AddDynamic(this, &UUW_SkillTreePopup::OnButton4Clicked);
	Button5->OnClicked.AddDynamic(this, &UUW_SkillTreePopup::OnButton5Clicked);
}

void UUW_SkillTreePopup::OnButton0Clicked()
{
	OnPopupButtonClicked.Broadcast(0, SkillActor);
	SkillActor = nullptr;
	SetVisibility(ESlateVisibility::Hidden);
}

void UUW_SkillTreePopup::OnButton1Clicked()
{
	OnPopupButtonClicked.Broadcast(1, SkillActor);
	SkillActor = nullptr;
	SetVisibility(ESlateVisibility::Hidden);
}

void UUW_SkillTreePopup::OnButton2Clicked()
{
	OnPopupButtonClicked.Broadcast(2, SkillActor);
	SkillActor = nullptr;
	SetVisibility(ESlateVisibility::Hidden);
}

void UUW_SkillTreePopup::OnButton3Clicked()
{
	OnPopupButtonClicked.Broadcast(3, SkillActor);
	SkillActor = nullptr;
	SetVisibility(ESlateVisibility::Hidden);
}

void UUW_SkillTreePopup::OnButton4Clicked()
{
	OnPopupButtonClicked.Broadcast(4, SkillActor);
	SkillActor = nullptr;
	SetVisibility(ESlateVisibility::Hidden);
}

void UUW_SkillTreePopup::OnButton5Clicked()
{
	OnPopupButtonClicked.Broadcast(5, SkillActor);
	SkillActor = nullptr;
	SetVisibility(ESlateVisibility::Hidden);
}
