#include "DungeonCharacter.h"
#include "Global.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"

#include "Components/SkillComponent.h"
#include "Components/MontageComponent.h"
#include "Components/StatusComponent.h"
#include "Objects/Projectile.h"

ADungeonCharacter::ADungeonCharacter()
{
	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	//scene
	CHelpers::CreateComponent(this, &CameraBoom, "CameraBoom", RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	CHelpers::CreateComponent(this, &TopDownCameraComponent, "TopDownCamera", CameraBoom);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//actor
	CHelpers::CreateActorComponent<USkillComponent>(this, &Skill, "Skill");
	CHelpers::CreateActorComponent<UMontageComponent>(this, &Montage, "Montage");
	CHelpers::CreateActorComponent<UStatusComponent>(this, &Status, "Status");

}

void ADungeonCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

FGenericTeamId ADungeonCharacter::GetGenericTeamId() const
{
	return TeamID;
}

void ADungeonCharacter::UseSkill(FSkillData* InData)
{
	Status->SetCannotUse();
	if(!InData->bCanMove)Status->SetStop();
	if(InData->Montage)PlayAnimMontage(InData->Montage, InData->PlayRate, InData->StartSection);
	Skill->SetSkill(InData);
}

void ADungeonCharacter::UseSkill(int32 Idx)
{
	FSkillData* data = Skill->GetSkillData(Idx);
	CheckNull(data);
	UseSkill(data);
}

void ADungeonCharacter::UseLeft()
{
	FSkillData* data = Skill->GetLeft();
	CheckNull(data);
	UseSkill(data);
}

void ADungeonCharacter::UseRight()
{
	FSkillData* data = Skill->GetRight();
	CheckNull(data);
	UseSkill(data);
}

void ADungeonCharacter::UseQ()
{
	FSkillData* data = Skill->GetQ();
	CheckNull(data); 
	UseSkill(data);
}

void ADungeonCharacter::UseW()
{
	FSkillData* data = Skill->GetW();
	CheckNull(data);
	UseSkill(data);
}

void ADungeonCharacter::UseE()
{
	FSkillData* data = Skill->GetE();
	CheckNull(data);
	UseSkill(data);
}

void ADungeonCharacter::UseR()
{
	FSkillData* data = Skill->GetR();
	CheckNull(data);
	UseSkill(data);
}

bool ADungeonCharacter::CanUse()
{
	return Status->CanUse();
}

bool ADungeonCharacter::CanMove()
{
	return Status->CanMove();
}

void ADungeonCharacter::SetUse()
{
	Status->SetUse();
}

void ADungeonCharacter::SetMove()
{
	Status->SetMove();
}

void ADungeonCharacter::UnsetSkill()
{
	Skill->UnsetSkill();
}

void ADungeonCharacter::SpawnProjectile()
{
	FSkillData* cur = Skill->GetCurrentSkill();
	CheckNull(cur->ProjectileClass);

	FVector loc = GetMesh()->GetSocketLocation(cur->SocketName);
	FRotator rot = GetMesh()->GetSocketRotation(cur->SocketName);
	if (!cur->bUseSocketRotation)
		rot = GetActorForwardVector().Rotation();

	FActorSpawnParameters f;
	f.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GetWorld()->SpawnActor(cur->ProjectileClass, &loc, &rot, f);
}
