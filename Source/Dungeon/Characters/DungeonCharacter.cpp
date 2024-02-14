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

#include "DungeonPlayerController.h"
#include "Components/SkillComponent.h"
#include "Components/SkillTreeComponent.h"
#include "Components/MontageComponent.h"
#include "Components/StatusComponent.h"
#include "Components/InventoryComponent.h"
#include "Objects/Projectile.h"

#include "Widgets/UW_Main.h"
#include "Widgets/UW_QuickSlot.h"

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
	CHelpers::CreateActorComponent<USkillTreeComponent>(this, &SkillTree, "SkillTree");
	CHelpers::CreateActorComponent<UMontageComponent>(this, &Montage, "Montage");
	CHelpers::CreateActorComponent<UStatusComponent>(this, &Status, "Status");
	CHelpers::CreateActorComponent<UInventoryComponent>(this, &Inventory, "Inventory");

}

void ADungeonCharacter::BeginPlay()
{
	Super::BeginPlay();
	Init();
}

void ADungeonCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

float ADungeonCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float result = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	CLog::Print(DamageAmount);

	return result;
}

FGenericTeamId ADungeonCharacter::GetGenericTeamId() const
{
	return TeamID;
}

void ADungeonCharacter::Init()
{
	//Skillcomp
	Skill->SpawnSkillActors();
	ADungeonPlayerController* controller = Cast<ADungeonPlayerController>(this->GetController());
	if (controller)
	{
		UUW_Main* mainWidget = controller->GetMainWidget();
		if (mainWidget)
		{
			mainWidget->GetQuickSlot()->ConnectComponent(Skill);
		}
	}

	//SkillTreecomp
	TFunction<void(int32, ASkillActor*)> func;
	func = [this](int32 Idx, ASkillActor* Actor)
	{
		ChangeQuickSlotData(Idx, Actor);
	};
	SkillTree->Init(Skill->GetSkillActors(), func);

}

void ADungeonCharacter::UseSkill(int32 Idx)
{
	Skill->UseSkill(Idx);
}

void ADungeonCharacter::UseLeft()
{
	UseSkill(0);
}

void ADungeonCharacter::UseRight()
{
	UseSkill(1);
}

void ADungeonCharacter::UseQ()
{
	UseSkill(2);
}

void ADungeonCharacter::UseW()
{
	UseSkill(3);
}

void ADungeonCharacter::UseE()
{
	UseSkill(4);
}

void ADungeonCharacter::UseR()
{
	UseSkill(5);
}

void ADungeonCharacter::ChangeQuickSlotData(int32 Index, ASkillActor* InSkillActor)
{
	Skill->ChangeQuickSlotData(Index, InSkillActor);
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

void ADungeonCharacter::SetCannotUse()
{
	Status->SetCannotUse();
}

void ADungeonCharacter::SetMove()
{
	Status->SetMove();
}

void ADungeonCharacter::SetStop()
{
	Status->SetStop();
}

void ADungeonCharacter::UnsetSkill()
{

}

void ADungeonCharacter::SpawnProjectile()
{
	Skill->SpawnProjectile();
}

void ADungeonCharacter::OnCollision()
{
	Inventory->OnCollision();
}

void ADungeonCharacter::OffCollision()
{
	Inventory->OffCollision();
}

void ADungeonCharacter::ResetHittedActors()
{
	Inventory->ResetHittedActors();
}
