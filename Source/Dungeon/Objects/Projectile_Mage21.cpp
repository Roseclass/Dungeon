#include "Objects/Projectile_Mage21.h"
#include "Global.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"

#include "Characters/DungeonCharacterBase.h"

AProjectile_Mage21::AProjectile_Mage21()
{
	CHelpers::CreateComponent(this, &Root, "Root");
	CHelpers::CreateComponent(this, &CollisionMesh, "CollisionMesh", Root);
	CHelpers::CreateComponent(this, &NiagaraEffect, "NiagaraEffect", Root);
}

void AProjectile_Mage21::BeginPlay()
{
	Super::BeginPlay();

	CollisionMesh->OnComponentBeginOverlap.Clear();
	CollisionMesh->OnComponentEndOverlap.Clear();

	CollisionMesh->OnComponentBeginOverlap.AddDynamic(this, &AProjectile_Mage21::OnComponentBeginOverlap);
	CollisionMesh->OnComponentEndOverlap.AddDynamic(this, &AProjectile_Mage21::OnComponentEndOverlap);
}

void AProjectile_Mage21::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectile_Mage21::Activate()
{
	Super::Activate();

	//for (auto i : GetOverlappedActors())
	//	SendDamage(Damage, i, FHitResult());

	FTimerHandle WaitHandle;
	float WaitTime = 2.0f;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
	{
		Deactivate();
	}), WaitTime, false);
}

void AProjectile_Mage21::Deactivate()
{
	Super::Deactivate();
	Destroy();
}
