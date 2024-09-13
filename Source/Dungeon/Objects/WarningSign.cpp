#include "Objects/WarningSign.h"
#include "Global.h"

#include "Kismet/KismetMaterialLibrary.h"
#include "Components/SceneComponent.h"
#include "Components/DecalComponent.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/GameStateBase.h"

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

void AWarningSign::Activate(float StartServerTime, float NewDuration, float NewExtraDuration)
{
	UWorld* World = Owner->GetWorld();
	AGameStateBase* GameState = World->GetGameState();

	StartTime = GetWorld()->GetTimeSeconds();
	float ServerWorldTime = StartTime;

	if (GameState)GameState->GetServerWorldTimeSeconds();

	StartTime -= (ServerWorldTime - StartServerTime);

	Duration= NewDuration;
	ExtraDuration= NewExtraDuration;

	Decal->SetDecalMaterial(UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), Sign));
	bActivate = 1;

}

/*
float FActiveGameplayEffectsContainer::GetServerWorldTime() const
{
	UWorld* World = Owner->GetWorld();
	AGameStateBase* GameState = World->GetGameState();
	if (GameState)
	{
		return GameState->GetServerWorldTimeSeconds();
	}

	return World->GetTimeSeconds();
}

float FActiveGameplayEffectsContainer::GetWorldTime() const
{
	UWorld *World = Owner->GetWorld();
	return World->GetTimeSeconds();
}
void FActiveGameplayEffect::RecomputeStartWorldTime(const FActiveGameplayEffectsContainer& InArray)
{
	RecomputeStartWorldTime(InArray.GetWorldTime(), InArray.GetServerWorldTime());
}

void FActiveGameplayEffect::RecomputeStartWorldTime(const float WorldTime, const float ServerWorldTime)
{
	StartWorldTime = WorldTime - (ServerWorldTime - StartServerWorldTime);
}
*/