#include "Objects/Projectile.h"
#include "Global.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/ShapeComponent.h"

#include "Characters/DungeonCharacter.h"

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
	FindCollision();
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FGenericTeamId AProjectile::GetGenericTeamId() const
{
	return TeamID;
}

void AProjectile::FindCollision()
{
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
		component->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnComponentBeginOverlap);
		component->OnComponentHit.AddDynamic(this, &AProjectile::OnComponentHit);
	}
}

void AProjectile::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IGenericTeamAgentInterface* other = Cast<IGenericTeamAgentInterface>(OtherActor);
	ACharacter* otherCh = Cast<ACharacter>(OtherActor);
	if (!other && !otherCh)
		return;

	//ignore alliance
	CheckTrue(other->GetGenericTeamId() == TeamID);

	SendDamage(Damage, OtherActor, SweepResult);
}

void AProjectile::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AProjectile::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//FVector loc = OverlapTransform.GetLocation() + Hit.Location;
	//FRotator rot = FRotator(OverlapTransform.GetRotation()) + Hit.Normal.Rotation();
	//FVector scale = OverlapTransform.GetScale3D();
	//float actDamage = Damage;

	////overlap with non-character or non-TeamActor
	//IGenericTeamAgentInterface* other = Cast<IGenericTeamAgentInterface>(OtherActor);
	//ACharacter* otherCh = Cast<ACharacter>(OtherActor);
	//if (!other && !otherCh)
	//{
	//	if (!bAOE)
	//		Destroy();
	//	return;
	//}

	////ignore alliance
	//CheckTrue(other->GetGenericTeamId() == TeamID);

	//// Is Hitted?
	//UCStateComponent* state = CHelpers::GetComponent<UCStateComponent>(otherCh);
	//if (state && !state->IsHitable())return;

	//SendDamage(actDamage, OtherActor, Hit);

	//if (!bAOE)
	//	Destroy();
}

void AProjectile::SendDamage(float InDamage, AActor* OtherActor, const FHitResult& SweepResult)
{
	ACharacter* ch = Cast<ACharacter>(GetOwner());
	AController* inst = nullptr;
	if (ch)inst = ch->GetController();

	FDamageEvent f;
	OtherActor->TakeDamage(InDamage, f, inst, this);
}

void AProjectile::SetCollide(bool InState)
{
	bCanCollide = InState;
}

void AProjectile::Activate()
{

}

void AProjectile::Deactivate()
{

}
