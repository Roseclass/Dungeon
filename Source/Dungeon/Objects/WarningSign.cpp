#include "Objects/WarningSign.h"
#include "Global.h"

#include "Kismet/KismetMaterialLibrary.h"
#include "Components/SceneComponent.h"
#include "Components/DecalComponent.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInstanceDynamic.h"

AWarningSign::AWarningSign()
{
	PrimaryActorTick.bCanEverTick = true;

	CHelpers::CreateComponent(this, &Root, "Root");
	CHelpers::CreateComponent(this, &Decal, "Decal", Root);
}

void AWarningSign::BeginPlay()
{
	Super::BeginPlay();
}

void AWarningSign::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bActivate)
	{
		float cur = GetWorld()->GetTimeSeconds() - StartTime;
		float rate = cur / Duration;
		UMaterialInstanceDynamic* mat = Cast< UMaterialInstanceDynamic>(Decal->GetDecalMaterial());
		mat->SetScalarParameterValue(FName("Rate"), rate);
		if (rate > 1 && cur > ExtraDuration + Duration)Destroy();
	}

}

void AWarningSign::Activate(float NewDuration, float NewExtraDuration)
{
	Duration= NewDuration;
	ExtraDuration= NewExtraDuration;

	Decal->SetDecalMaterial(UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), Sign));
	bActivate = 1;
	StartTime = GetWorld()->GetTimeSeconds();
}
