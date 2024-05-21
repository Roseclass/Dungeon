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
	CheckFalse(HasAuthority());

	// is activated?
	if (!IsActivated())return;

	// already hit?
	if (GetDamagedActors().Contains(OtherActor))return;

	// overlap with DungeonCharacterBase?
	ADungeonCharacterBase* base = Cast<ADungeonCharacterBase>(OtherActor);
	if (!base)return;

	// is deadmode?
	UStateComponent* state = CHelpers::GetComponent<UStateComponent>(OtherActor);
	if (!state)return;
	if (state->IsDeadMode())return;

	// ignore alliance
	CheckTrue(base->GetGenericTeamId() == TeamID);

	Super::OnComponentBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

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
