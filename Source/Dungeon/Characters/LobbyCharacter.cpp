#include "Characters/LobbyCharacter.h"
#include "Global.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "SaveManager.h"
#include "Components/AppearanceComponent.h"
#include "Components/AppearanceMeshComponent.h"

ALobbyCharacter::ALobbyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CHelpers::CreateComponent(this, &RootMesh, "RootMesh", RootComponent);

	CHelpers::CreateComponent(this, &CameraBoom, "CameraBoom", RootMesh);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->SetRelativeRotation(FRotator(0.f, -60.f, 0.f));
	CameraBoom->SetRelativeLocation(FVector(50, 30, 120));

	CHelpers::CreateComponent(this, &Camera, "Camera", CameraBoom);

	CHelpers::CreateActorComponent<UAppearanceComponent>(this, &Appearance, "Appearance");
}

void ALobbyCharacter::BeginPlay()
{
	Super::BeginPlay();

	{
		TMap<EAppearancePart, USkeletalMeshComponent*> map;
		TArray<UAppearanceMeshComponent*> meshComponents;
		GetComponents<UAppearanceMeshComponent>(meshComponents);
		for (UAppearanceMeshComponent* component : meshComponents)
		{
			if (!component)continue;
			EAppearancePart part = component->GetAppearancePart();
			if (part == EAppearancePart::Max)continue;
			map.Add({ part,component });
			component->SetMasterPoseComponent(RootMesh);
		}
		Appearance->Init(map);
	}
}

void ALobbyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALobbyCharacter::ChangeAppearance(EAppearancePart InMeshPart, int32 InIndex)
{
	Appearance->ChangeAppearance(InMeshPart, InIndex);
}

void ALobbyCharacter::ChangeColorData(EAppearancePart InMeshPart, FName Parameter, FLinearColor NewColor)
{
	Appearance->ChangeColorData(InMeshPart, Parameter, NewColor);
}

void ALobbyCharacter::OnWheel(float Delta)
{
	if (Delta > 0) ZoomRate = UKismetMathLibrary::Clamp(++ZoomRate, 0, 5);
	else ZoomRate = UKismetMathLibrary::Clamp(--ZoomRate, 0, 5);

	CameraBoom->TargetArmLength = UKismetMathLibrary::Lerp(minLen, maxLen, float(ZoomRate) / 5);
	CameraBoom->SetRelativeLocation(UKismetMathLibrary::VLerp(minLoc, maxLoc, float(ZoomRate)/ 5));
}

FString ALobbyCharacter::GetUniqueSaveName()
{
	return UniqueName;
}

void ALobbyCharacter::OnBeforeSave(USaveGameData* SaveData)
{
	Appearance->SaveData(SaveData);
}

void ALobbyCharacter::OnAfterLoad(USaveGameData* const ReadData)
{
	FString name = GetUniqueSaveName();
	CheckNull(ReadData);

	Appearance->LoadData(ReadData);
}