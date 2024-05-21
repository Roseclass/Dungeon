#include "Components/HitEffectComponent.h"
#include "Global.h"
#include "Components/SkeletalMeshComponent.h"


UHitEffectComponent::UHitEffectComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHitEffectComponent::BeginPlay()
{
	Super::BeginPlay();
	FindMesh();
}

void UHitEffectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateGlow(DeltaTime);
}

void UHitEffectComponent::FindMesh()
{
	auto arr = GetOwner()->GetComponentsByClass(USkeletalMeshComponent::StaticClass());
	
	for (auto i : arr)
		Meshes.Add(Cast<USkeletalMeshComponent>(i));

	for (auto i : Meshes)
	{
		int32 max = i->GetNumMaterials();
		for (int32 idx = 0; idx < max; ++idx)
			i->CreateDynamicMaterialInstance(idx);
	}
}

void UHitEffectComponent::UpdateGlow(float DeltaTime)
{
	CheckTrue(CurrentEmissiveColor == FVector());

	CurrentEmissiveColor -= (DeltaTime * CurrentEmissiveColor) / GlowTime;

	if (CurrentEmissiveColor.X < 0)CurrentEmissiveColor.X = 0;
	if (CurrentEmissiveColor.Y < 0)CurrentEmissiveColor.Y = 0;
	if (CurrentEmissiveColor.Z < 0)CurrentEmissiveColor.Z = 0;

	for (auto i : Meshes)
	{
		int32 max = i->GetNumMaterials();
		for (int32 idx = 0; idx < max; ++idx)
		{
			UMaterialInstanceDynamic* mat = Cast<UMaterialInstanceDynamic>(i->GetMaterial(idx));
			if (mat)mat->SetVectorParameterValue(ParameterName, CurrentEmissiveColor);
		}
	}
}

void UHitEffectComponent::Glow()
{
	CurrentEmissiveColor = EmissiveColor;
}
