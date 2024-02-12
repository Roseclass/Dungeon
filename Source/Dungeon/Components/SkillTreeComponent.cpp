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

void USkillTreeComponent::Init(const TArray<ASkillActor*>& Array)
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
		Widget->Init(Array);
		Widget->AddToViewport();
	}
}

void USkillTreeComponent::AddPoints()
{

}

void USkillTreeComponent::Unlock()
{
	for (auto i : RootActors)
	{
		for (auto child : i->GetChildren())
		{
			//자식순회하면서 언락
		}
	}
}

void USkillTreeComponent::Acquire()
{
	//특정 액터 습득
}

