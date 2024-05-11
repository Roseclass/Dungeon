#include "Objects/Projectile.h"
#include "Global.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/ShapeComponent.h"

#include "Characters/DungeonCharacterBase.h"
#include "Components/StateComponent.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = 1;
	SetReplicateMovement(1);

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

void AProjectile::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnComponentBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (!IsActivated())return;
	if (GetDamagedActors().Contains(OtherActor))return;

	UStateComponent* state = CHelpers::GetComponent<UStateComponent>(OtherActor);
	if (!state)return;
	if (state->IsDeadMode())return;

	SendDamage(Damage, OtherActor, SweepResult);

	if (!bAOE)
	{
		Deactivate();
		Destroy();
	}
}

void AProjectile::Activate()
{
	bAct = 1;
}

void AProjectile::Deactivate()
{
	bAct = 0;
}
