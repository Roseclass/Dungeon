#include "Objects/Eqquipment.h"
#include "Global.h"
#include "Components/ShapeComponent.h"
#include "Components/MeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"

#include "Characters/DungeonCharacterBase.h"
#include "Objects/ItemManager.h"
#include "Objects/ItemObject.h"

AEqquipment::AEqquipment()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = 1;
}

void AEqquipment::BeginPlay()
{
	Super::BeginPlay();
	ItemObject = NewObject<UItemObject>(this, FName("Item"));
	ItemObject->Init(DimensionX, DimensionY, Icon, IconRotated, this, ItemType);

	FindComponents();

	SpawnLootEffects();

	/*if (!Manager)
	{
		AActor* manager = UGameplayStatics::GetActorOfClass(GetWorld(), AItemManager::StaticClass());
		Manager = Cast<AItemManager>(manager);
	}*/
}

#if WITH_EDITOR
void AEqquipment::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UpperBodyIndex = FMath::Clamp(UpperBodyIndex, 0, ChestAttachment_MAX);
}
#endif

void AEqquipment::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckTrue(!NiagaraPickEffect && !ParticlePickEffect);
	if (NiagaraPickEffect)
	{
		NiagaraPickEffect->SetWorldRotation(LootEffectWorldRotation);
	}
	if (ParticlePickEffect)
	{
		ParticlePickEffect->SetWorldRotation(LootEffectWorldRotation);
	}
}

void AEqquipment::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicated 변수를 여기에 추가
	DOREPLIFETIME_CONDITION(AEqquipment, Mode, COND_None);
}

void AEqquipment::OnRep_Mode()
{
	switch (Mode)
	{
	case EItemMode::Loot:SetPickableMode(); break;
	case EItemMode::Inventory:SetInventoryMode(); break;
	case EItemMode::Equip:SetEquipMode(); break;
	case EItemMode::Max:break;
	default:break;
	}
}

void AEqquipment::FindComponents()
{
	// find interact collision
	TArray<UShapeComponent*> shapeComponents;
	GetComponents<UShapeComponent>(shapeComponents);
	for (UShapeComponent* component : shapeComponents)
	{
		for (auto i : component->ComponentTags)
		{
			if (i == FName("InteractCollision"))
			{
				InteractCollisionComponents.Add(component);
				break;
			}
		}
	}

	// find mesh
	GetComponents<UMeshComponent>(MeshComponents);
}

void AEqquipment::SpawnLootEffects()
{
	UNiagaraSystem* n = Cast<UNiagaraSystem>(LootEffect);
	UParticleSystem* p = Cast<UParticleSystem>(LootEffect);

	UFXSystemComponent* fx = nullptr;
	if (n)
	{
		NiagaraPickEffect = UNiagaraFunctionLibrary::SpawnSystemAttached(n, MeshComponents[0], NAME_None, FVector(), FRotator(), EAttachLocation::SnapToTarget, 1);
		NiagaraPickEffect->SetIsReplicated(1);
		NiagaraPickEffect->SetAutoDestroy(0);
		fx = NiagaraPickEffect;
	}
	else if (p)
	{
		ParticlePickEffect = UGameplayStatics::SpawnEmitterAttached(p, MeshComponents[0], NAME_None, FVector(), FRotator(), EAttachLocation::SnapToTarget);
		ParticlePickEffect->SetIsReplicated(1);
		ParticlePickEffect->bAutoDestroy = 0;
		fx = ParticlePickEffect;
	}

	if (fx)
	{
		for (auto i : LootEffectParams)
		{
			if (i.ParamType == PSPT_Scalar)fx->SetFloatParameter(i.Name, i.Scalar);
			else if (i.ParamType == PSPT_Vector)fx->SetVectorParameter(i.Name, i.Vector);
			else if (i.ParamType == PSPT_Color)fx->SetColorParameter(i.Name, i.Color);
			else if (i.ParamType == PSPT_Actor)fx->SetActorParameter(i.Name, i.Actor);
		}
	}
	if (!bPickable)DeactivateEffect();
}

void AEqquipment::SetEffectLocation()
{

}

void AEqquipment::SortMesh()
{

}

void AEqquipment::ActivateEffect()
{
	CheckTrue(!NiagaraPickEffect && !ParticlePickEffect);
	if (NiagaraPickEffect)
	{
		NiagaraPickEffect->SetVisibility(1);
	}
	if (ParticlePickEffect)
	{
		ParticlePickEffect->SetVisibility(1);
	}
}

void AEqquipment::DeactivateEffect()
{
	CheckTrue(!NiagaraPickEffect && !ParticlePickEffect);
	if (NiagaraPickEffect)NiagaraPickEffect->SetVisibility(0);
	if (ParticlePickEffect)ParticlePickEffect->SetVisibility(0);
}

void AEqquipment::SetPickableMode()
{
	//bPickable = 1;

	// Save Field Loaction, Save Mesh Location, Adjust Effect Location
	SetEffectLocation();

	// On Appearance
	for (auto component : MeshComponents)
		component->SetVisibility(1);

	// On Interaction Collision
	//InteractCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	// Change Owner
	SetOwnerCharacter(nullptr);
	FDetachmentTransformRules f = FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepRelative, EDetachmentRule::KeepWorld, 1);
	DetachFromActor(f);

	// On Item Effects
	ActivateEffect();

}

void AEqquipment::SetInventoryMode()
{
	// Sort Mesh
	//if (bPickable)SortMesh();

	bPickable = 0;

	// Off Item Effects
	DeactivateEffect();

	// Off Widget
	//InfoWidget->SetVisibility(0);

	// Off Appearance
	for (auto component : MeshComponents)
		component->SetVisibility(0);

	//InteractCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEqquipment::SetEquipMode()
{
	// Sort Mesh
	//if (bPickable)SortMesh();

	bPickable = 0;

	// Off Item Effects
	DeactivateEffect();

	// Off Widget
	//InfoWidget->SetVisibility(0);

	// On Appearance
	for (auto component : MeshComponents)
		component->SetVisibility(1);

	//InteractCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEqquipment::SetOwnerCharacter(ACharacter* InCharacter)
{
	OwnerCharacter = Cast<ACharacter>(InCharacter);
	//if (OwnerCharacter)
	//{
	//	State = CHelpers::GetComponent<UCStateComponent>(OwnerCharacter);
	//	Status = CHelpers::GetComponent<UCStatusComponent>(OwnerCharacter);
	//}
	//else
	//{
	//	State = nullptr;
	//	Status = nullptr;
	//}
}

void AEqquipment::SetItemLocation(const FVector& NewLocation, bool bSweep, FHitResult* OutSweepHitResult, ETeleportType Teleport)
{
	if (!Manager)
	{
		CLog::Print(__FUNCTION__);
		return;
	}
	Manager->SetItemLocation(this, NewLocation, bSweep, OutSweepHitResult, Teleport);
}

void AEqquipment::SetItemRotation(FRotator NewRotation, ETeleportType Teleport)
{
	if (!Manager)
	{
		CLog::Print(__FUNCTION__);
		return;
	}
	Manager->SetItemRotation(this, NewRotation, Teleport);
}

void AEqquipment::AttachItemToComponent(USceneComponent* Parent, const FAttachmentTransformRules& AttachmentRules, FName InSocketName)
{
	if (!Manager)
	{
		CLog::Print(__FUNCTION__);
		return;
	}
	Manager->AttachItemToComponent(this, Parent, AttachmentRules, InSocketName);
}

void AEqquipment::ChangeVisibility(EItemMode InMode)
{
	if (InMode == EItemMode::Max)return;
	if (!Manager)
	{
		CLog::Print(__FUNCTION__);
		return;
	}
	Manager->ChangeVisibility(this, InMode);
}

void AEqquipment::SetMode(EItemMode InMode)
{
	Mode = InMode;
	if (HasAuthority())OnRep_Mode();
}