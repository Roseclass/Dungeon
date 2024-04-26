#include "Widgets/UW_Dead.h"
#include "Global.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "GameFramework/SpringArmComponent.h"

#include "DungeonPlayerController.h"
#include "Characters/PlayerCharacter.h"

void UUW_Dead::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ConfirmButton->OnClicked.AddDynamic(this, &UUW_Dead::OnConfirmButtonClicked);
	NextViewTargetButton->OnClicked.AddDynamic(this, &UUW_Dead::OnNextViewTargetButtonClicked);
}

void UUW_Dead::OnConfirmButtonClicked()
{
	CheckTrue(IsPlayingAnimation());

	PlayAnimationForward(Hide);

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), ViewTargets);
}

void UUW_Dead::OnNextViewTargetButtonClicked()
{
	ViewIndex = ++ViewIndex % ViewTargets.Num();

	CheckFalse(ViewTargets.IsValidIndex(ViewIndex));

	USpringArmComponent* springArm = CHelpers::GetComponent<USpringArmComponent>(GetOwningPlayer()->GetPawn());
	springArm->AttachToComponent(ViewTargets[ViewIndex]->GetRootComponent(), { EAttachmentRule::SnapToTarget,0 });
	springArm->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
}

void UUW_Dead::On()
{
	ConfirmButton->SetFocus();
	PlayAnimationForward(Reveal);
}
