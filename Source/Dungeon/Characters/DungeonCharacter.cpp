#include "Characters/DungeonCharacter.h"
#include "Global.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/DecalComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Engine/TextureRenderTarget2D.h"

#include "SaveManager.h"
#include "DungeonPlayerController.h"
#include "Components/AppearanceComponent.h"
#include "Components/AppearanceMeshComponent.h"
#include "Components/SkillComponent.h"
#include "Components/SkillTreeComponent.h"
#include "Components/MontageComponent.h"
#include "Components/StatusComponent.h"
#include "Components/InventoryComponent.h"
#include "Objects/Projectile.h"

#include "Widgets/UW_Main.h"
#include "Widgets/UW_QuickSlot.h"
#include "Objects/SkillActor.h"
#include "Objects/Weapon.h"
#include "Objects/ItemObject.h"

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
	CameraBoom->TargetArmLength = 2500.0f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	CHelpers::CreateComponent(this, &TopDownCameraComponent, "TopDownCamera", CameraBoom);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	CHelpers::CreateComponent(this, &MinimapIcon, "MinimapIcon", RootComponent);
	MinimapIcon->SetVisibleInSceneCaptureOnly(1);
	MinimapIcon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//actor
	CHelpers::CreateActorComponent<UAppearanceComponent>(this, &Appearance, "Appearance");
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
	//Appearance
	{
		TMap<EAppearancePart, USkeletalMeshComponent*> map;
		TArray<UAppearanceMeshComponent*> meshComponents;
		GetComponents<UAppearanceMeshComponent>(meshComponents);
		for (UAppearanceMeshComponent* component : meshComponents)
		{
			if (!component)continue;
			EAppearancePart part = component->GetAppearancePart();
			if (part == EAppearancePart::Max)continue;
			map.Add({ part,component });
			component->SetMasterPoseComponent(GetMesh());
		}
		Appearance->Init(map);
	}

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
		if (HasAuthority())
			InitClientWidget();
	}
	
	//Inventory
	Inventory->OnInventoryItemChanged.AddDynamic(this, &ADungeonCharacter::ChangeAppearance);

	//init minimap
	MinimapIcon->CreateDynamicMaterialInstance(0);
	if (controller && controller->IsLocalController())
		Cast<UMaterialInstanceDynamic>(MinimapIcon->GetMaterial(0))->SetVectorParameterValue("Color",FLinearColor::Green);
	else
		Cast<UMaterialInstanceDynamic>(MinimapIcon->GetMaterial(0))->SetVectorParameterValue("Color", FLinearColor::Yellow);

	if (controller && controller->IsLocalController())
	{
		MinimapArm = NewObject<USpringArmComponent>(this, "MinimapArm");
		MinimapArm->SetupAttachment(RootComponent);
		MinimapArm->SetUsingAbsoluteRotation(true);
		MinimapArm->TargetArmLength = 1000.f;
		MinimapArm->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
		MinimapArm->bDoCollisionTest = false;
		MinimapArm->RegisterComponent();

		MinimapCapture = NewObject<USceneCaptureComponent2D>(this, "MinimapCapture");
		MinimapCapture->SetupAttachment(MinimapArm);
		MinimapCapture->RegisterComponent();

		MinimapCapture->ProjectionType = ECameraProjectionMode::Orthographic;
		MinimapCapture->OrthoWidth = 2000;

		//showflags
		MinimapCapture->ShowFlags.BSP = 0;
		MinimapCapture->ShowFlags.Decals = 0;
		MinimapCapture->ShowFlags.Landscape = 1;
		MinimapCapture->ShowFlags.Translucency = 1;
		MinimapCapture->ShowFlags.SkeletalMeshes = 0;
		MinimapCapture->ShowFlags.StaticMeshes = 1;
		MinimapCapture->ShowFlags.AntiAliasing = 0;
		MinimapCapture->ShowFlags.Atmosphere = 0;
		MinimapCapture->ShowFlags.Particles = 0;
		MinimapCapture->ShowFlags.Fog = 0;
		MinimapCapture->ShowFlags.InstancedGrass = 1;
		MinimapCapture->ShowFlags.NaniteMeshes = 0;
		MinimapCapture->ShowFlags.SingleLayerWaterRefractionFullPrecision = 1;
		MinimapCapture->ShowFlags.DeferredLighting = 1;
		MinimapCapture->ShowFlags.TextRender = 0;
		MinimapCapture->ShowFlags.InstancedStaticMeshes = 1;
		MinimapCapture->ShowFlags.TemporalAA = 0;
		MinimapCapture->ShowFlags.Paper2DSprites = 0;
		MinimapCapture->ShowFlags.InstancedFoliage = 1;
		MinimapCapture->ShowFlags.EyeAdaptation = 1;
		MinimapCapture->ShowFlags.LocalExposure = 1;
		MinimapCapture->ShowFlags.MotionBlur = 0;
		MinimapCapture->ShowFlags.Bloom = 1;
		MinimapCapture->ShowFlags.ToneCurve= 1;
		MinimapCapture->ShowFlags.SkyLighting= 0;
		MinimapCapture->ShowFlags.DynamicShadows = 0;
		MinimapCapture->ShowFlags.AmbientOcclusion= 0;
		MinimapCapture->ShowFlags.DistanceFieldAO = 1;
		MinimapCapture->ShowFlags.LightShafts = 1;
		MinimapCapture->ShowFlags.LightFunctions = 1;
		MinimapCapture->ShowFlags.ReflectionEnvironment = 1;
		MinimapCapture->ShowFlags.VolumetricFog = 1;
		MinimapCapture->ShowFlags.ScreenSpaceReflections = 1;
		MinimapCapture->ShowFlags.AmbientCubemap = 1;
		MinimapCapture->ShowFlags.TexturedLightProfiles = 1;
		MinimapCapture->ShowFlags.Game = 1;
		MinimapCapture->ShowFlags.Lighting = 1;
		MinimapCapture->ShowFlags.PathTracing = 0;
		MinimapCapture->ShowFlags.PostProcessing = 1;

		//rendertarget
		UTextureRenderTarget2D* target;
		CHelpers::GetAssetDynamic(&target, "TextureRenderTarget2D'/Game/Widgets/Minimap/RT_Minimap.RT_Minimap'");
		MinimapCapture->TextureTarget = target;
	}

	//Load appearance data if its client
	if (controller && controller->IsLocalController())
	{
		USaveGameData* saveGameData = Cast<USaveGameData>(UGameplayStatics::LoadGameFromSlot(USaveManager::GetCurrentSaveSlot(), 0));
		if (!saveGameData) { CLog::Print("saveGameData is nullptr"); return; }
		OnAfterLoad(saveGameData);
	}
}

void ADungeonCharacter::OffAllWidget()
{

}

void ADungeonCharacter::InitClientWidget()
{
	CheckFalse(IsLocallyControlled());
	TFunction<void(int32, ASkillActor*)> func;
	func = [this](int32 Idx, ASkillActor* Actor)
	{
		ChangeQuickSlotData(Idx, Actor);
	};
	SkillTree->Init(Skill->GetSkillActors(), func);
}

void ADungeonCharacter::ChangeAppearance(EAppearancePart InMeshPart, int32 InIndex)
{
	Appearance->Server_ChangeAppearance(InMeshPart, InIndex);
}

void ADungeonCharacter::ChangeColorData(EAppearancePart InMeshPart, FName Parameter, FLinearColor NewColor)
{
	Appearance->Server_ChangeColor(InMeshPart, Parameter, NewColor);
}

void ADungeonCharacter::UseSkill(int32 Idx)
{
	Skill->UseSkill(Idx);
}

void ADungeonCharacter::UseLeft()
{
	CheckFalse(CanUse());
	CheckTrue(Skill->IsQuickSlotCoolDown(0));
	if (!Skill->GetSkillActor(0)->GetSkillData()->bCanMove)
		GetCharacterMovement()->StopMovementImmediately();
	UseSkill(0);
}

void ADungeonCharacter::UseRight()
{
	CheckFalse(CanUse());
	CheckTrue(Skill->IsQuickSlotCoolDown(1));
	if (!Skill->GetSkillActor(1)->GetSkillData()->bCanMove)
		GetCharacterMovement()->StopMovementImmediately();
	UseSkill(1);
}

void ADungeonCharacter::UseQ()
{
	CheckFalse(CanUse());
	CheckTrue(Skill->IsQuickSlotCoolDown(2));
	if (!Skill->GetSkillActor(2)->GetSkillData()->bCanMove)
		GetCharacterMovement()->StopMovementImmediately();
	UseSkill(2);
}

void ADungeonCharacter::UseW()
{
	CheckFalse(CanUse());
	CheckTrue(Skill->IsQuickSlotCoolDown(3));
	if (!Skill->GetSkillActor(3)->GetSkillData()->bCanMove)
		GetCharacterMovement()->StopMovementImmediately();
	UseSkill(3);
}

void ADungeonCharacter::UseE()
{
	CheckFalse(CanUse());
	CheckTrue(Skill->IsQuickSlotCoolDown(4));
	if (!Skill->GetSkillActor(4)->GetSkillData()->bCanMove)
		GetCharacterMovement()->StopMovementImmediately();
	UseSkill(4);
}

void ADungeonCharacter::UseR()
{
	CheckFalse(CanUse());
	CheckTrue(Skill->IsQuickSlotCoolDown(5));
	if (!Skill->GetSkillActor(5)->GetSkillData()->bCanMove)
		GetCharacterMovement()->StopMovementImmediately();
	UseSkill(5);
}

void ADungeonCharacter::ChangeQuickSlotData(int32 Index, ASkillActor* InSkillActor)
{
	Skill->ChangeQuickSlotData(Index, InSkillActor);
}

void ADungeonCharacter::TryAddItem(AWeapon* InObject)
{
	Inventory->Server_TryAddItem(InObject);
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

void ADungeonCharacter::ToggleSkillTree()
{
	if (SkillTree->IsWidgetVisible())
		SkillTree->OffWidget();
	else
	{
		OffAllWidget();
		SkillTree->OnWidget();
	}
}

void ADungeonCharacter::ToggleInventory()
{
	if (Inventory->IsWidgetVisible())
		Inventory->OffWidget();
	else
	{
		OffAllWidget();
		Inventory->OnWidget();
	}
}

FString ADungeonCharacter::GetUniqueSaveName()
{
	return "Player";
}

void ADungeonCharacter::OnBeforeSave(USaveGameData* SaveData)
{
	//Inventory
	//PresetData
	{
		SaveData->PlayerData.PresetClasses.Empty();
		SaveData->PlayerData.PresetClasses.Init(nullptr, 3);
		for (int32 i = 0; i < 3; i++)
		{
			AWeapon* weapon = Inventory->GetPresetItems(i);
			if (!weapon)continue;
			SaveData->PlayerData.PresetClasses[i] = weapon->GetClass();
		}
	}

	//InventoryData
	{
		TMap<AWeapon*, TTuple<int32, int32>> m;
		Inventory->GetAllItems(m);
		for (auto i : m)
		{
			if (!i.Key)continue;
			AWeapon* weapon = i.Key;
			if (!weapon)continue;
			SaveData->PlayerData.InventoryClasses.Add(weapon->GetClass());
		}
	}

	////SkillTree
	////SkillPoints
	//SaveData->PlayerData.SkillPoints = SkillTree->GetSkillPoints();

	////SkillData
	//SkillTree->GetAcquiredSkills(SaveData->PlayerData.AcquiredSkills);

	////SlotData
	//SkillTree->GetSlotClassData(SaveData->PlayerData.SlotSkills);
}

void ADungeonCharacter::OnAfterLoad(USaveGameData* const ReadData)
{
	FString name = GetUniqueSaveName();
	CheckNull(ReadData);

	Appearance->LoadData(ReadData);
	//Reset();
	//Inventory->LoadData(ReadData);
	//SkillTree->LoadData(ReadData);
}
