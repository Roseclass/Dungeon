#include "Objects/Weapon.h"
#include "Global.h"
#include "Components/ShapeComponent.h"
#include "Components/MeshComponent.h"

#include "Characters/DungeonCharacter.h"
#include "Objects/ItemObject.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	ItemObject = CreateDefaultSubobject<UItemObject>(FName("Item"));
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	ItemObject->Init(DimensionX, DimensionY, Icon, IconRotated, this);
	FindComponents();
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::FindComponents()
{
	//콜리전 찾기
	TArray<UShapeComponent*> shapeComponents;
	GetComponents<UShapeComponent>(shapeComponents);
	for (UShapeComponent* component : shapeComponents)
	{
		for (auto i : component->ComponentTags)
		{
			if (i == FName("OverlapCollision"))
			{
				CollisionComponents.Add(component);
				break;
			}
		}
	}
	for (UShapeComponent* component : CollisionComponents)
	{
		component->OnComponentBeginOverlap.Clear();
		component->OnComponentHit.Clear();
		component->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnComponentBeginOverlap);
		component->OnComponentHit.AddDynamic(this, &AWeapon::OnComponentHit);
	}

	//메시 찾기
	GetComponents<UMeshComponent>(MeshComponents);
}

void AWeapon::SetEffectLocation()
{

}

void AWeapon::SortMesh()
{

}

void AWeapon::ActivateEffect()
{

}

void AWeapon::DeactivateEffect()
{

}

void AWeapon::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CheckTrue(HittedActors.Contains(OtherActor));
	IGenericTeamAgentInterface* other = Cast<IGenericTeamAgentInterface>(OtherActor);
	ACharacter* otherCh = Cast<ACharacter>(OtherActor);
	if (!other && !otherCh)
		return;

	//ignore alliance
	CheckTrue(other->GetGenericTeamId() == TeamID);

	SendDamage(Damage, OtherActor, SweepResult);
}

void AWeapon::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

}

void AWeapon::SendDamage(float InDamage, AActor* OtherActor, const FHitResult& SweepResult)
{
	ACharacter* ch = Cast<ACharacter>(GetOwner());
	AController* inst = nullptr;
	if (ch)inst = ch->GetController();

	FDamageEvent f;
	OtherActor->TakeDamage(InDamage, f, inst, this);
	HittedActors.Add(OtherActor);
}

void AWeapon::SetOwnerCharacter(ACharacter* InCharacter)
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

void AWeapon::OnCollision()
{
	for (auto i : CollisionComponents)
		i->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeapon::OffCollision()
{
	for (auto i : CollisionComponents)
		i->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::ResetHittedActors()
{
	HittedActors.Empty();
}

void AWeapon::SetEquipMode()
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

	// Off Collision
	for (UShapeComponent* component : CollisionComponents)
		component->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//InteractCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::SetInventoryMode()
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

	// Off Collision
	for (UShapeComponent* component : CollisionComponents)
		component->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//InteractCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::SetPickableMode()
{
	//bPickable = 1;

	// Save Field Loaction, Save Mesh Location, Adjust Effect Location
	SetEffectLocation();

	// On Item Effects
	ActivateEffect();

	// On Appearance
	for (auto component : MeshComponents)
		component->SetVisibility(1);

	// Off Collision
	for (UShapeComponent* component : CollisionComponents)
		component->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// On Interaction Collision
	//InteractCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	// Change Owner
	SetOwnerCharacter(nullptr);
	FDetachmentTransformRules f = FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepRelative, EDetachmentRule::KeepWorld, 1);
	DetachFromActor(f);
}
