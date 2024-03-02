#include "Objects/SkillActor.h"
#include "Global.h"
#include "GameFramework/GameState.h"

#include "DungeonPlayerController.h"
#include "Characters/DungeonCharacter.h"
#include "Components/SkillComponent.h"
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
	/*
	float FActiveGameplayEffectsContainer::GetServerWorldTime() const
	{
		UWorld* World = Owner->GetWorld();
		AGameStateBase* GameState = World->GetGameState();
		if (GameState)
		{
			return GameState->GetServerWorldTimeSeconds();
		}
	
		return World->GetTimeSeconds();
	}
	
	float FActiveGameplayEffectsContainer::GetWorldTime() const
	{
		UWorld *World = Owner->GetWorld();
		return World->GetTimeSeconds();
	}
	void FActiveGameplayEffect::RecomputeStartWorldTime(const FActiveGameplayEffectsContainer& InArray)
	{
		RecomputeStartWorldTime(InArray.GetWorldTime(), InArray.GetServerWorldTime());
	}
	
	void FActiveGameplayEffect::RecomputeStartWorldTime(const float WorldTime, const float ServerWorldTime)
	{
		StartWorldTime = WorldTime - (ServerWorldTime - StartServerWorldTime);
	}
	*/
}

void ASkillActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicated 변수를 여기에 추가
	DOREPLIFETIME_CONDITION(ASkillActor, OwnerCharacter, COND_None);
	DOREPLIFETIME_CONDITION(ASkillActor, ParentSkill, COND_None);
	DOREPLIFETIME_CONDITION(ASkillActor, ChildrenSkills, COND_None);
	DOREPLIFETIME_CONDITION(ASkillActor, bCoolDown_Server, COND_None);
	DOREPLIFETIME_CONDITION(ASkillActor, StartServerWorldTime, COND_None);
}

void ASkillActor::OnRep_CoolDown_Server()
{
	if (bCoolDown_Server && OwnerCharacter)
	{
		//ui 쿨다운 돌리기
		USkillComponent* skill = CHelpers::GetComponent<USkillComponent>(OwnerCharacter);
		CheckNull(skill);

		float WorldTime, ServerWorldTime;
		WorldTime = ServerWorldTime = GetWorld()->GetTimeSeconds();

		AGameStateBase* GameState = GetWorld()->GetGameState();
		if(GameState)ServerWorldTime = GameState->GetServerWorldTimeSeconds();

		StartWorldTime = WorldTime - (ServerWorldTime - StartServerWorldTime);
		skill->SetCoolDown(this);
	}
	else
	{
		bCoolDown_Client = 0;
	}
}

void ASkillActor::Multicast_Use_Implementation(ADungeonPlayerController* Exception)
{
	CheckTrue(Exception && Exception->IsLocalController());
	if (Data.Montage)OwnerCharacter->PlayAnimMontage(Data.Montage, Data.PlayRate, Data.StartSection);
}

void ASkillActor::Server_Use_Implementation(ADungeonPlayerController* Exception)
{
	bCoolDown_Server = 1;
	StartServerWorldTime = GetWorld()->GetTimeSeconds();
	if (OwnerCharacter)
	{
		//ui 쿨다운 돌리기
		USkillComponent* skill = CHelpers::GetComponent<USkillComponent>(OwnerCharacter);
		CheckNull(skill);
		StartWorldTime = StartServerWorldTime;
		skill->SetCoolDown(this);
	}

	FTimerHandle WaitHandle;
	float WaitTime = Data.CoolDown;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
	{
		bCoolDown_Server = 0;
		bCoolDown_Client = 0;
	}), WaitTime, false);

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
	bCoolDown_Client = 1;
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

float ASkillActor::GetRemainingCoolDown() const
{
	float result = StartWorldTime + Data.CoolDown - GetWorld()->GetTimeSeconds();
	if (result < 0)result = 0;
	return result;
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
