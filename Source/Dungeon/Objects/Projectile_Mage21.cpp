#include "Objects/Projectile_Mage21.h"
#include "Global.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"

#include "Characters/DungeonCharacter.h"

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
	CollisionMesh->OnComponentHit.Clear();
	CollisionMesh->OnComponentBeginOverlap.AddDynamic(this, &AProjectile_Mage21::OnComponentBeginOverlap);
	CollisionMesh->OnComponentEndOverlap.AddDynamic(this, &AProjectile_Mage21::OnComponentEndOverlap);
	CollisionMesh->OnComponentHit.AddDynamic(this, &AProjectile_Mage21::OnComponentHit);
}

void AProjectile_Mage21::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile_Mage21::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OverlappedActors.Contains(OtherActor))
		return;

	IGenericTeamAgentInterface* other = Cast<IGenericTeamAgentInterface>(OtherActor);
	ACharacter* otherCh = Cast<ACharacter>(OtherActor);
	if (!other && !otherCh)
		return;

	//ignore alliance
	CheckTrue(other->GetGenericTeamId() == TeamID);
		
	if (bAct)SendDamage(Damage, OtherActor, SweepResult);
	OverlappedActors.AddUnique(OtherActor);
}

void AProjectile_Mage21::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	OverlappedActors.Remove(OtherActor);
}

void AProjectile_Mage21::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

}

void AProjectile_Mage21::Activate()
{
	Super::Activate();
	bAct = 1;
	for (auto i : OverlappedActors)
		SendDamage(Damage, i, FHitResult());

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
	bAct = 0;
}
