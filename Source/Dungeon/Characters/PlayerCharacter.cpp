#include "Characters/PlayerCharacter.h"
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
#include "Components/StateComponent.h"
#include "Components/StatusComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/QuestComponent.h"
#include "Objects/Projectile.h"

#include "Widgets/UW_Main.h"
#include "Widgets/UW_QuickSlot.h"
#include "Objects/SkillActor.h"
#include "Objects/Weapon.h"
#include "Objects/ItemObject.h"

APlayerCharacter::APlayerCharacter()
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
	CHelpers::CreateActorComponent<USkillTreeComponent>(this, &SkillTree, "SkillTree");
	CHelpers::CreateActorComponent<UQuestComponent>(this, &Quest, "Quest");
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	Init();
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

float APlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float result = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	CLog::Print(DamageAmount);

	return result;
}

FGenericTeamId APlayerCharacter::GetGenericTeamId() const
{
	return TeamID;
}

void APlayerCharacter::OffAllWidget()
{

}

void APlayerCharacter::Init()
{
	Super::Init();
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
	if (HasAuthority())Skill->SpawnSkillActors();
	ADungeonPlayerController* controller = Cast<ADungeonPlayerController>(this->GetController());
	if (controller)
	{
		UUW_Main* mainWidget = controller->GetMainWidget();
		if (mainWidget)
		{
			mainWidget->GetQuickSlot()->ConnectComponent(Skill);
		}
		if (HasAuthority()) // client : initwidget in onrep
			InitClientWidget();
	}
	
	//Inventory
	Inventory->OnInventoryItemChanged.AddDynamic(this, &APlayerCharacter::ChangeAppearance);

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

void APlayerCharacter::InitClientWidget()
{
	CheckFalse(IsLocallyControlled());
	TFunction<void(int32, ASkillActor*)> func;
	func = [this](int32 Idx, ASkillActor* Actor)
	{
		ChangeQuickSlotData(Idx, Actor);
	};
	SkillTree->Init(Skill->GetSkillActors(), func);
}

void APlayerCharacter::ChangeAppearance(EAppearancePart InMeshPart, int32 InIndex)
{
	Appearance->Server_ChangeAppearance(InMeshPart, InIndex);
}

void APlayerCharacter::ChangeColorData(EAppearancePart InMeshPart, FName Parameter, FLinearColor NewColor)
{
	Appearance->Server_ChangeColor(InMeshPart, Parameter, NewColor);
}

void APlayerCharacter::UseSkill(int32 Idx)
{
	Super::UseSkill(Idx);
	Skill->UseQuickSlotSkill(Idx);
}

void APlayerCharacter::UseLeft()
{
	CheckFalse(CanUse());
	CheckTrue(Skill->IsQuickSlotCoolDown(0));
	if (!Skill->GetSkillActor(0)->GetSkillData()->bCanMove)
		GetCharacterMovement()->StopMovementImmediately();
	UseSkill(0);
}

void APlayerCharacter::UseRight()
{
	CheckFalse(CanUse());
	CheckTrue(Skill->IsQuickSlotCoolDown(1));
	if (!Skill->GetSkillActor(1)->GetSkillData()->bCanMove)
		GetCharacterMovement()->StopMovementImmediately();
	UseSkill(1);
}

void APlayerCharacter::UseQ()
{
	CheckFalse(CanUse());
	CheckTrue(Skill->IsQuickSlotCoolDown(2));
	if (!Skill->GetSkillActor(2)->GetSkillData()->bCanMove)
		GetCharacterMovement()->StopMovementImmediately();
	UseSkill(2);
}

void APlayerCharacter::UseW()
{
	CheckFalse(CanUse());
	CheckTrue(Skill->IsQuickSlotCoolDown(3));
	if (!Skill->GetSkillActor(3)->GetSkillData()->bCanMove)
		GetCharacterMovement()->StopMovementImmediately();
	UseSkill(3);
}

void APlayerCharacter::UseE()
{
	CheckFalse(CanUse());
	CheckTrue(Skill->IsQuickSlotCoolDown(4));
	if (!Skill->GetSkillActor(4)->GetSkillData()->bCanMove)
		GetCharacterMovement()->StopMovementImmediately();
	UseSkill(4);
}

void APlayerCharacter::UseR()
{
	CheckFalse(CanUse());
	CheckTrue(Skill->IsQuickSlotCoolDown(5));
	if (!Skill->GetSkillActor(5)->GetSkillData()->bCanMove)
		GetCharacterMovement()->StopMovementImmediately();
	UseSkill(5);
}

void APlayerCharacter::ChangeQuickSlotData(int32 Index, ASkillActor* InSkillActor)
{
	Skill->ChangeQuickSlotData(Index, InSkillActor);
}

void APlayerCharacter::TryAddItem(AWeapon* InObject)
{
	Inventory->Server_TryAddItem(InObject);
}

void APlayerCharacter::ToggleSkillTree()
{
	if (SkillTree->IsWidgetVisible())
		SkillTree->OffWidget();
	else
	{
		OffAllWidget();
		SkillTree->OnWidget();
	}
}

void APlayerCharacter::ToggleInventory()
{
	if (Inventory->IsWidgetVisible())
		Inventory->OffWidget();
	else
	{
		OffAllWidget();
		Inventory->OnWidget();
	}
}

FString APlayerCharacter::GetUniqueSaveName()
{
	return "Player";
}

void APlayerCharacter::OnBeforeSave(USaveGameData* SaveData)
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

void APlayerCharacter::OnAfterLoad(USaveGameData* const ReadData)
{
	FString name = GetUniqueSaveName();
	CheckNull(ReadData);

	Appearance->LoadData(ReadData);
	//Reset();
	//Inventory->LoadData(ReadData);
	//SkillTree->LoadData(ReadData);
}
