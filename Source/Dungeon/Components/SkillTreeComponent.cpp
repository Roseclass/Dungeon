#include "Components/SkillTreeComponent.h"
#include "Global.h"

#include "Characters/PlayerCharacter.h"
#include "DungeonPlayerController.h"
#include "Objects/SkillActor.h"
#include "Widgets/UW_SkillTree.h"

USkillTreeComponent::USkillTreeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USkillTreeComponent::BeginPlay()
{
	Super::BeginPlay();

}

void USkillTreeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USkillTreeComponent::Init(const TArray<ASkillActor*>& Array, TFunction<void(int32, ASkillActor*)> OnPopupClicked)
{
	for (auto i : Array)
	{
		if (!i->GetParent())
			RootActors.Add(i);
	}

	if (WidgetClass)
	{
		APlayerCharacter* character = Cast<APlayerCharacter>(GetOwner());
		CheckNull(character);
		ADungeonPlayerController* controller = Cast<ADungeonPlayerController>(character->GetController());
		CheckNull(controller);

		Widget = CreateWidget<UUW_SkillTree, ADungeonPlayerController>(controller, WidgetClass);
		Widget->AddToViewport();
		Widget->Init(Array, this, OnPopupClicked);
	}
}

void USkillTreeComponent::AddPoints()
{

}

void USkillTreeComponent::Acquire(ASkillActor* SkillActor)
{
	SkillActor->SetAcquired();
}

bool USkillTreeComponent::IsWidgetVisible()
{
	if (!Widget)return 0;
	return Widget->IsVisible();
}

void USkillTreeComponent::OnWidget()
{
	CheckNull(Widget);
	Widget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void USkillTreeComponent::OffWidget()
{
	CheckNull(Widget);
	Widget->SetVisibility(ESlateVisibility::Collapsed);
}
