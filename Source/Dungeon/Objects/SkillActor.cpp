#include "Objects/SkillActor.h"
#include "Global.h"

#include "Characters/DungeonCharacter.h"
#include "Objects/Projectile.h"

ASkillActor::ASkillActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASkillActor::BeginPlay()
{
	Super::BeginPlay();
}

void ASkillActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASkillActor::Load()
{
	if (SkillTreeState == ESkillTreeSkillState::Locked)
	{
		SetLocked();
	}
	else if (SkillTreeState == ESkillTreeSkillState::Unlocked)
	{
		SetUnlocked();
	}
	else if (SkillTreeState == ESkillTreeSkillState::Acquired)
	{
		SetAcquired();
	}
}

void ASkillActor::Use()
{
	CheckNull(OwnerCharacter);
	OwnerCharacter->SetCannotUse();
	if(!Data.bCanMove)OwnerCharacter->SetStop();
	if(Data.Montage)OwnerCharacter->PlayAnimMontage(Data.Montage, Data.PlayRate, Data.StartSection);
}

void ASkillActor::SpawnProjectile()
{
	CheckNull(OwnerCharacter);
	FVector loc = OwnerCharacter->GetMesh()->GetSocketLocation(Data.SocketName);
	FRotator rot = OwnerCharacter->GetMesh()->GetSocketRotation(Data.SocketName);
	if (!Data.bUseSocketRotation)
		rot = OwnerCharacter->GetActorForwardVector().Rotation();

	FTransform trans;
	trans.SetLocation(loc);
	trans.SetRotation(FQuat4d(rot));

	FActorSpawnParameters f;
	f.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AProjectile* projectile = GetWorld()->SpawnActorDeferred<AProjectile>(Data.ProjectileClass, trans, OwnerCharacter, OwnerCharacter, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	projectile->SetTeamID(OwnerCharacter->GetGenericTeamId());
	projectile->SetDamage(10);//TODO::
	//projectile->SetTarget(InActor);

	UGameplayStatics::FinishSpawningActor(projectile, trans);
}

void ASkillActor::SetLocked() 
{ 
	SkillTreeState = ESkillTreeSkillState::Locked;
	if(OnLocked.IsBound())
		OnLocked.Broadcast();
}

void ASkillActor::SetUnlocked() 
{ 
	SkillTreeState = ESkillTreeSkillState::Unlocked;
	if (OnUnlocked.IsBound())
		OnUnlocked.Broadcast();
}

void ASkillActor::SetAcquired() 
{
	SkillTreeState = ESkillTreeSkillState::Acquired;
	if (OnAcquired.IsBound())
		OnAcquired.Broadcast();

	for (auto child : Children)
	{
		if (child->GetSkillTreeState() == ESkillTreeSkillState::Locked)
		{
			child->SetUnlocked();
		}
	}
}
