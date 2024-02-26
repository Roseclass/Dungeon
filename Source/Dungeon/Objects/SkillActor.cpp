#include "Objects/SkillActor.h"
#include "Global.h"

#include "DungeonPlayerController.h"
#include "Characters/DungeonCharacter.h"
#include "Objects/Projectile.h"

ASkillActor::ASkillActor()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = 1;
}

void ASkillActor::BeginPlay()
{
	Super::BeginPlay();
}

void ASkillActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bCoolTime)
	{
		CurrnetCoolTime += DeltaTime;
		if (CurrnetCoolTime >= Data.CoolTime)
		{
			CurrnetCoolTime = 0;
			bCoolTime = 0;
		}
	}
}

void ASkillActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicated 변수를 여기에 추가
	DOREPLIFETIME_CONDITION(ASkillActor, OwnerCharacter, COND_None);
	DOREPLIFETIME_CONDITION(ASkillActor, ParentSkill, COND_None);
	DOREPLIFETIME_CONDITION(ASkillActor, ChildrenSkills, COND_None);
}

void ASkillActor::Multicast_Use_Implementation(ADungeonPlayerController* Exception)
{
	if (Exception && Exception->IsLocalController())return;
	if (Data.Montage)OwnerCharacter->PlayAnimMontage(Data.Montage, Data.PlayRate, Data.StartSection);
}

void ASkillActor::Server_Use_Implementation(ADungeonPlayerController* Exception)
{
	Multicast_Use(Exception);
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

void ASkillActor::Client_Use_Implementation()
{
	CheckNull(OwnerCharacter);
	CoolTimeStart();
	OwnerCharacter->SetCannotUse();

	if(!Data.bCanMove)OwnerCharacter->SetStop();
	if(Data.Montage)OwnerCharacter->PlayAnimMontage(Data.Montage, Data.PlayRate, Data.StartSection);
	ADungeonPlayerController* controller = Cast<ADungeonPlayerController>(OwnerCharacter->GetController());
	if (controller)Server_Use(controller);
}

void ASkillActor::Server_SpawnProjectile_Implementation()
{
	CheckNull(OwnerCharacter);
	FVector loc = OwnerCharacter->GetMesh()->GetSocketLocation(Data.SocketName);
	FRotator rot = OwnerCharacter->GetMesh()->GetSocketRotation(Data.SocketName);
	if (!Data.bUseSocketRotation)
		rot = OwnerCharacter->GetActorForwardVector().Rotation();

	FTransform trans;
	trans.SetLocation(loc);
	trans.SetRotation(FQuat4d(rot));
	//Multicast_SpawnProjectile(trans);
	FActorSpawnParameters f;
	f.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AProjectile* projectile = GetWorld()->SpawnActorDeferred<AProjectile>(Data.ProjectileClass, trans, OwnerCharacter, OwnerCharacter, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	projectile->SetTeamID(OwnerCharacter->GetGenericTeamId());
	projectile->SetDamage(10);//TODO::
	//projectile->SetTarget(InActor);
	projectile->SetCollide(1);

	UGameplayStatics::FinishSpawningActor(projectile, trans);
	projectile->Activate();
}

void ASkillActor::CoolTimeStart()
{
	bCoolTime = 1;
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

	for (auto child : ChildrenSkills)
	{
		if (child->GetSkillTreeState() == ESkillTreeSkillState::Locked)
		{
			child->SetUnlocked();
		}
	}
}
