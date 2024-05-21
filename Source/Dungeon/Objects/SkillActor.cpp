#include "Objects/SkillActor.h"
#include "Global.h"
#include "GameFramework/GameState.h"

#include "DungeonPlayerController.h"
#include "Characters/DungeonCharacterBase.h"

#include "Components/SkillComponent.h"
#include "Components/StatusComponent.h"

#include "Objects/Projectile.h"
#include "Objects/WarningSign.h"

ASkillActor::ASkillActor()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = 1;
	bAlwaysRelevant = 1;
}

void ASkillActor::BeginPlay()
{
	Super::BeginPlay();
}

void ASkillActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASkillActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicated 변수를 여기에 추가
	DOREPLIFETIME_CONDITION(ASkillActor, bCoolDown_Server, COND_None);
	DOREPLIFETIME_CONDITION(ASkillActor, StartServerWorldTime, COND_None);
	DOREPLIFETIME_CONDITION(ASkillActor, SkillTreeState, COND_None);
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

void ASkillActor::OnRep_SkillTreeState()
{
	RefreshWidget();
}

void ASkillActor::Multicast_Use_Implementation()
{
	if (Data.Montage)
		OwnerCharacter->PlayAnimMontage(Data.Montage, Data.PlayRate, Data.StartSection);
}

void ASkillActor::Client_UseFailed_Implementation()
{
	CheckNull(OwnerCharacter);

	// update status
	bCoolDown_Client = 0;
	OwnerCharacter->SetUse();
	OwnerCharacter->SetMove();
}

void ASkillActor::Server_Use_Implementation()
{
	// check owner
	if (!OwnerCharacter)
	{
		Client_UseFailed();
		CLog::Print("ASkillActor::Server_Use Owner is nullptr", -1, 10, FColor::Red);
		return;
	}

	// check status
	UStatusComponent* status = CHelpers::GetComponent<UStatusComponent>(OwnerCharacter);
	if (!status)
	{
		Client_UseFailed();
		CLog::Print("ASkillActor::Server_Use status is nullptr", -1, 10, FColor::Red);
		return;
	}

	// check cost
	if (status->GetCurrentMana_Server() < Data.ManaCost)
	{
		Client_UseFailed();
		CLog::Print("ASkillActor::Server_Use ManaCost is bigger", 1557, 10, FColor::MakeRandomColor());
		return;
	}

	// update current cost
	status->AdjustCurrentMana(-Data.ManaCost);

	// set cooldown
	bCoolDown_Server = 1;
	StartServerWorldTime = GetWorld()->GetTimeSeconds();

	// run ui cooldown
	USkillComponent* skill = CHelpers::GetComponent<USkillComponent>(OwnerCharacter);
	CheckNull(skill);
	StartWorldTime = StartServerWorldTime;
	skill->SetCoolDown(this);	

	// change ownerstate
	OwnerCharacter->SetSkillMode();

	// Broadcast
	OnSkillUsing.Broadcast(this);

	// reserve cooldown end event
	FTimerHandle WaitHandle;
	float WaitTime = Data.CoolDown;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
	{
		bCoolDown_Server = 0;
		bCoolDown_Client = 0;
	}), WaitTime, false);

	// next sequence
	Multicast_Use();
}

void ASkillActor::RefreshWidget()
{
	// update skill button widget
	if (SkillTreeState == ESkillTreeSkillState::Locked)
	{
		if (OnLocked.IsBound())
			OnLocked.Broadcast();
	}
	else if (SkillTreeState == ESkillTreeSkillState::Unlocked)
	{
		if (OnUnlocked.IsBound())
			OnUnlocked.Broadcast();
	}
	else if (SkillTreeState == ESkillTreeSkillState::Acquired)
	{
		if (OnAcquired.IsBound())
			OnAcquired.Broadcast();
	}
}

void ASkillActor::Client_Use_Implementation()
{
	CheckNull(OwnerCharacter);

	// update status
	bCoolDown_Client = 1;
	OwnerCharacter->SetCannotUse();
	Server_Use();
}

void ASkillActor::SpawnProjectile()
{
	CheckFalse(HasAuthority());
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

	float damage = 1;

	// check status
	UStatusComponent* status = CHelpers::GetComponent<UStatusComponent>(OwnerCharacter);
	if (status)
	{
		damage = status->GetDamage();
	}

	AProjectile* projectile = GetWorld()->SpawnActorDeferred<AProjectile>(Data.ProjectileClass, trans, OwnerCharacter, OwnerCharacter, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	projectile->SetTeamID(OwnerCharacter->GetGenericTeamId());
	projectile->SetDamage(CalculateDamage(damage));
	projectile->Activate();
	UGameplayStatics::FinishSpawningActor(projectile, trans);
}

void ASkillActor::SpawnWarningSign(int32 InIndex)
{
	CheckFalse(Data.WarningSignData.IsValidIndex(InIndex));

	FVector offset = OwnerCharacter->GetActorForwardVector() * Data.WarningSignData[InIndex].ForwardOffset;
	offset += OwnerCharacter->GetActorForwardVector() * Data.WarningSignData[InIndex].RightOffset;

	FTransform transform;
	transform.SetTranslation(OwnerCharacter->GetActorLocation() + offset);
	transform.SetScale3D(Data.WarningSignData[InIndex].Scale);

	AWarningSign* sign = GetWorld()->SpawnActor<AWarningSign>(Data.WarningSignData[InIndex].WarningSignClass, transform);
	if (sign)
		sign->Activate(Data.WarningSignData[InIndex].Duration, Data.WarningSignData[InIndex].ExtraDuration);
}

float ASkillActor::GetRemainingCoolDown() const
{
	float result = StartWorldTime + Data.CoolDown - GetWorld()->GetTimeSeconds();
	if (result < 0)result = 0;
	return result;
}

void ASkillActor::Server_SetLocked_Implementation()
{ 
	SkillTreeState = ESkillTreeSkillState::Locked;
	if(HasAuthority())
		OnRep_SkillTreeState();
}

void ASkillActor::Server_SetUnlocked_Implementation()
{ 
	SkillTreeState = ESkillTreeSkillState::Unlocked;
	if (HasAuthority())
		OnRep_SkillTreeState();
}

void ASkillActor::Server_SetAcquired_Implementation()
{
	SkillTreeState = ESkillTreeSkillState::Acquired;
	if (HasAuthority())
	{
		OnRep_SkillTreeState();

		for (auto child : ChildrenSkills)
		{
			if (child->GetSkillTreeState() == ESkillTreeSkillState::Locked)
			{
				child->Server_SetUnlocked();
			}
		}
	}
}
