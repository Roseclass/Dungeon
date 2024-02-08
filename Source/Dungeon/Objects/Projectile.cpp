#include "Objects/Projectile.h"
#include "Global.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/ShapeComponent.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	//actor
	CHelpers::CreateActorComponent<UProjectileMovementComponent>(this, &ProjectileMovement, "ProjectileMovement");
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectile::FindCollision()
{
	TArray<UShapeComponent*> shapeComponents;
	//GetComponents<UShapeComponent>(shapeComponents);
	//for (UShapeComponent* component : shapeComponents)
	//{
	//	for (auto i : component->ComponentTags)
	//	{
	//		if (i == AttackCollisionTag)
	//		{
	//			CollisionComponents.Add(component);
	//			break;
	//		}
	//	}
	//}
	//for (UShapeComponent* component : CollisionComponents)
	//{
	//	component->OnComponentBeginOverlap.Clear();
	//	component->OnComponentEndOverlap.Clear();
	//	component->OnComponentBeginOverlap.AddDynamic(this, &ACAttachment::OnComponentBeginOverlap);
	//	component->OnComponentEndOverlap.AddDynamic(this, &ACAttachment::OnComponentEndOverlap);
	//}
}
