#include "Objects/DamageText.h"
#include "Global.h"
#include "Components/WidgetComponent.h"

ADamageText::ADamageText()
{
	PrimaryActorTick.bCanEverTick = true;

	CHelpers::CreateComponent(this, &Scene, "Scene");
	CHelpers::CreateComponent(this, &Widget, "InfoWidget", Scene);

}

void ADamageText::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADamageText::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

