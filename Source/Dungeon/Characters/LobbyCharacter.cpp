#include "Characters/LobbyCharacter.h"
#include "Global.h"
#include "Components/SkeletalMeshComponent.h"

#include "Components/AppearanceComponent.h"
#include "Components/AppearanceMeshComponent.h"

ALobbyCharacter::ALobbyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CHelpers::CreateComponent(this, &RootMesh, "RootMesh", RootComponent);

	CHelpers::CreateActorComponent<UAppearanceComponent>(this, &Appearance, "Appearance");
}

void ALobbyCharacter::BeginPlay()
{
	Super::BeginPlay();
	//{
	//	TMap<EAppearancePart, USkeletalMeshComponent*> map;
	//	TArray<UAppearanceMeshComponent*> meshComponents;
	//	GetComponents<UAppearanceMeshComponent>(meshComponents);
	//	for (UAppearanceMeshComponent* component : meshComponents)
	//	{
	//		if (!component)continue;
	//		map[component->GetAppearancePart()] = component;
	//		component->SetMasterPoseComponent(RootMesh);
	//	}
	//	Appearance->Init(map);
	//}
}

void ALobbyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALobbyCharacter::ChangeAppearance(EAppearancePart InMeshPart, int32 InIndex)
{
	Appearance->ChangeAppearance(InMeshPart, InIndex);
}
