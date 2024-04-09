#include "Components/ClearViewComponent.h"
#include "Global.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "GameFramework/SpringArmComponent.h"

#include "DungeonPlayerController.h"
#include "Characters/PlayerCharacter.h"

UClearViewComponent::UClearViewComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UClearViewComponent::BeginPlay()
{
	Super::BeginPlay();

	//only for localplayer

	ACharacter* owner = Cast<ACharacter>(GetOwner());
	if (!owner)
	{
		CLog::Print("UClearViewComponent::BeginPlay owner is nullptr", -1, 10, FColor::Red);
		return;
	}
	if (!Cast<ADungeonPlayerController>(owner->GetController()))return;
	SpringArm = CHelpers::GetComponent<USpringArmComponent>(GetOwner());
}

void UClearViewComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (SpringArm)
	{
		// location
		FVector loc = SpringArm->GetComponentLocation();
		loc -= SpringArm->GetForwardVector() * (SpringArm->TargetArmLength * 0.5);
		UKismetMaterialLibrary::SetVectorParameterValue(GetWorld(), Collection, FName("Location"), FLinearColor(loc));
		// bounds
		UKismetMaterialLibrary::SetVectorParameterValue(GetWorld(), Collection, FName("Bounds"), FLinearColor(SpringArm->TargetArmLength * 0.01, HorizontalSize * 0.01, VerticalSize * 0.01));
		// forward
		UKismetMaterialLibrary::SetVectorParameterValue(GetWorld(), Collection, FName("X"), FLinearColor(SpringArm->GetForwardVector()));
		// right
		UKismetMaterialLibrary::SetVectorParameterValue(GetWorld(), Collection, FName("Y"), FLinearColor(SpringArm->GetRightVector()));
		// up
		UKismetMaterialLibrary::SetVectorParameterValue(GetWorld(), Collection, FName("Z"), FLinearColor(SpringArm->GetUpVector()));
	}
}

