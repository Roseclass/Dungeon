#include "Objects/Projectile.h"
#include "Global.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/ShapeComponent.h"

#include "Characters/DungeonCharacterBase.h"
#include "Components/SkillComponent.h"

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
	CheckFalse(HasAuthority());
	SpawnMuzzleFX();
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
	USkillComponent* skill = CHelpers::GetComponent<USkillComponent>(OtherActor);
	if (!skill)return;
	if (skill->IsDead())return;

	// ignore alliance
	CheckTrue(base->GetGenericTeamId() == TeamID);

	Super::OnComponentBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (int32(Type) & 0b01)
	{
		SpawnDestroyFX(SweepResult);
		Deactivate();
		Destroy();
	}
}

void AProjectile::FindCollision()
{
	Super::FindCollision();
	const TArray<UShapeComponent*>& arr = GetCollisionComponents();
	for(auto i : arr)
		i->OnComponentHit.AddDynamic(this, &AProjectile::OnComponentHit);
}

void AProjectile::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	CheckFalse(HasAuthority());

	if (int32(Type) & 0b10);
	else return;

	SpawnDestroyFX(Hit);
	Deactivate();
	Destroy();
}

void AProjectile::SpawnMuzzleFX()
{
	CheckNull(MuzzleFX);
	ADungeonCharacterBase* owner = Cast<ADungeonCharacterBase>(GetOwner());
	if (!owner)
	{
		CLog::Print("AProjectile::SpawnDestroyFX there is no owner");
		return;
	}

	USkillComponent* skill = Cast<USkillComponent>(owner->GetAbilitySystemComponent());
	if (!skill)
	{
		CLog::Print("AProjectile::SpawnDestroyFX skill is nullptr");
		return;
	}

	FTransform transform;
	transform.SetLocation(GetActorLocation());
	transform.SetRotation(FQuat4d(GetActorRotation()));

	skill->Multicast_FXEffect_Transform(MuzzleFX, transform);
}

void AProjectile::SpawnDestroyFX(const FHitResult& SweepResult)
{
	CheckNull(DestroyFX);
	ADungeonCharacterBase* owner = Cast<ADungeonCharacterBase>(GetOwner());
	if (!owner)
	{
		CLog::Print("AProjectile::SpawnDestroyFX there is no owner");
		return;
	}

	USkillComponent* skill = Cast<USkillComponent>(owner->GetAbilitySystemComponent());
	if (!skill)
	{
		CLog::Print("AProjectile::SpawnDestroyFX skill is nullptr");
		return;
	}

	FTransform transform;
	transform.SetLocation(SweepResult.Location);
	transform.SetRotation(FQuat4d(SweepResult.Normal.Rotation()));

	skill->Multicast_FXEffect_Transform(DestroyFX, transform);
}
