#include "Components/SkillTreeComponent.h"
#include "Global.h"

#include "Characters/DungeonCharacter.h"
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
		ADungeonCharacter* character = Cast<ADungeonCharacter>(GetOwner());
		CheckNull(character);
		ADungeonPlayerController* controller = Cast<ADungeonPlayerController>(character->GetController());
		CheckNull(controller);

		Widget = CreateWidget<UUW_SkillTree, ADungeonPlayerController>(controller, WidgetClass);
		Widget->Init(Array, OnPopupClicked);
		Widget->AddToViewport();
	}
}

void USkillTreeComponent::AddPoints()
{

}

void USkillTreeComponent::Acquire(ASkillActor* SkillActor)
{
	SkillActor->SetAcquired();
}