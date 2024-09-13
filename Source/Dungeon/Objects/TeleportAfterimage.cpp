#include "Objects/TeleportAfterimage.h"
#include "Global.h"

#include "Components/SceneComponent.h"
#include "Components/PoseableMeshComponent.h"

#include "Characters/DungeonCharacterBase.h"

ATeleportAfterimage::ATeleportAfterimage()
{
	PrimaryActorTick.bCanEverTick = true;
	InitialLifeSpan = 1;

	//scene
	CHelpers::CreateComponent(this, &RootScene, "RootScene");
}

void ATeleportAfterimage::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATeleportAfterimage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float appearance = GetGameTimeSinceCreation() - 0.5;
	if (appearance < 0)appearance = 0;
	for (auto poseable : PoseableMeshes)
	{
		for (int i = 0; i < poseable->GetNumMaterials(); ++i)
		{
			UMaterialInstanceDynamic* mat = Cast<UMaterialInstanceDynamic>(poseable->GetMaterial(i));
			if (!mat)continue;
			mat->SetScalarParameterValue("Appearance", appearance);
		}
	}
}

void ATeleportAfterimage::Init(ADungeonCharacterBase* InBase)
{
	USkeletalMeshComponent* master = InBase->GetMesh();
	TArray<USkeletalMeshComponent*> arr;
	InBase->GetComponents<USkeletalMeshComponent>(arr);

	for (auto comp : arr)
	{
		if (!comp->SkeletalMesh)continue;
		if (comp == master)continue;
		UPoseableMeshComponent* poseable = NewObject<UPoseableMeshComponent>(this);
		poseable->RegisterComponent();
		poseable->SetSkeletalMesh(comp->SkeletalMesh, 1);
		poseable->CopyPoseFromSkeletalComponent(master);
		for (int i = 0; i < poseable->GetNumMaterials(); ++i)
			poseable->CreateDynamicMaterialInstance(i);
		poseable->AttachToComponent(RootScene, FAttachmentTransformRules::SnapToTargetIncludingScale);
		poseable->SetRelativeRotation(FRotator(0, -90, 0));
		PoseableMeshes.Add(poseable);
	}
}