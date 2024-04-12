#include "Widgets/UW_ConfirmPopup.h"
#include "Global.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

#include "DungeonPlayerController.h"
#include "Characters/PlayerCharacter.h"

void UUW_ConfirmPopup::NativeConstruct()
{
	Super::NativeConstruct();

	ConfirmButton->OnClicked.AddDynamic(this, &UUW_ConfirmPopup::OnConfirmButtonClicked);
	CancelButton->OnClicked.AddDynamic(this, &UUW_ConfirmPopup::OnCancelButtonClicked);
	Activate();
}

void UUW_ConfirmPopup::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bActive)
	{
		float time = GetWorld()->GetTimeSeconds() - ActivateTime;
		time = time / TimeLimit;
		if (time > 1)
		{
			RemoveFromParent();
			return;
		}
		TimerBar->SetPercent(time);
	}
}

void UUW_ConfirmPopup::RemoveFromParent()
{
	// get player controller
	ADungeonPlayerController* pc = Cast<ADungeonPlayerController>(GetOwningPlayer());
	if (!pc)
	{
		CLog::Print("UUW_ConfirmPopup::Activate is nullptr", -1, 10, FColor::Red);
		return;
	}
	// restore input
	pc->SetGameAndUIMode();

	Super::RemoveFromParent();
}

void UUW_ConfirmPopup::OnConfirmButtonClicked()
{
	OnConfirm.ExecuteIfBound();
}

void UUW_ConfirmPopup::OnCancelButtonClicked()
{
	OnCancel.ExecuteIfBound();
}

void UUW_ConfirmPopup::SetWaitSign(UImage* InImage)
{

}

void UUW_ConfirmPopup::SetConfirmSign(UImage* InImage)
{

}

void UUW_ConfirmPopup::SetCancelSign(UImage* InImage) 
{

}

void UUW_ConfirmPopup::Init(FString InDescription, float NewTimeLimit)
{
	Description->SetText(FText::FromString(InDescription));
	TimeLimit = NewTimeLimit;
}

void UUW_ConfirmPopup::Activate()
{
	// get player controller
	ADungeonPlayerController* pc = Cast<ADungeonPlayerController>(GetOwningPlayer());
	if (!pc)
	{
		CLog::Print("UUW_ConfirmPopup::Activate is nullptr", -1, 10, FColor::Red);
		return;
	}

	// ignore input
	pc->SetUIOnlyMode();

	// shut down other popup widget
	pc->HideAddtiveWidget();

	// stop character movement
	pc->StopPawnImmediately();

	// set variables
	bActive = 1;
	ActivateTime = GetWorld()->GetTimeSeconds();

	// play opening animation
	PlayAnimationForward(Reveal);
}

void UUW_ConfirmPopup::UpdateSign(int32 PlayerIndex, bool NewState)
{

}
