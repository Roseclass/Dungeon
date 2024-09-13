#include "Characters/PlayerCharacter.h"
#include "Global.h"

#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/DecalComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Engine/TextureRenderTarget2D.h"

#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "Abilities/AttributeSetBase.h"
#include "Abilities/AttributeSet_Player.h"
#include "Abilities/GABase.h"

#include "SaveManager.h"
#include "DungeonPlayerController.h"
#include "Components/AppearanceComponent.h"
#include "Components/AppearanceMeshComponent.h"
#include "Components/ClearViewComponent.h"
#include "Components/SkillComponent.h"
#include "Components/SkillTreeComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/QuestComponent.h"
#include "Components/TravelEffectComponent.h"
#include "Objects/Projectile.h"

#include "Widgets/UW_Main.h"
#include "Widgets/UW_QuickSlot.h"
#include "Widgets/UW_HealthBar.h"
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
	CHelpers::CreateActorComponent<UClearViewComponent>(this, &ClearView, "ClearView");
	CHelpers::CreateActorComponent<USkillTreeComponent>(this, &SkillTree, "SkillTree");
	CHelpers::CreateActorComponent<UQuestComponent>(this, &Quest, "Quest");
	CHelpers::CreateActorComponent<UTravelEffectComponent>(this, &TravelPostProcess, "TravelPostProcess");

	AttributeSet_Player = CreateDefaultSubobject<UAttributeSet_Player>(TEXT("AttributeSet_Player"));
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	FGameplayTagContainer OwnedTags;
	Skill->GetOwnedGameplayTags(OwnedTags);
	for (const FGameplayTag& Tag : OwnedTags)
		CLog::Print(Tag.ToString(), -1, 0, FColor::Black);
}

FGenericTeamId APlayerCharacter::GetGenericTeamId() const
{
	return TeamID;
}

void APlayerCharacter::HealthChanged(const FOnAttributeChangeData& Data)
{
	Super::HealthChanged(Data);

	if (!AttributeSet)
	{
		CLog::Print("APlayerCharacter::HealthChanged, AttributeSet is nullptr", -1, 10, FColor::Red);
		return;
	}
	float maxHealth = AttributeSet->GetMaxHealth();

	if (!MainWidget)
	{
		CLog::Print("APlayerCharacter::HealthChanged, MainWidget is nullptr", -1, 10, FColor::Red);
		return;
	}
	MainWidget->HealthChanged(Data.NewValue / maxHealth);
}

void APlayerCharacter::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
	Super::MaxHealthChanged(Data);

	if (!AttributeSet)
	{
		CLog::Print("APlayerCharacter::MaxHealthChanged, AttributeSet is nullptr", -1, 10, FColor::Red);
		return;
	}
	float maxHealth = AttributeSet->GetMaxHealth();

	if (!MainWidget)
	{
		CLog::Print("APlayerCharacter::MaxHealthChanged, MainWidget is nullptr", -1, 10, FColor::Red);
		return;
	}
	//MainWidget->MaxHealthChanged(float NewValue);

}

void APlayerCharacter::ManaChanged(const FOnAttributeChangeData& Data)
{
	Super::ManaChanged(Data);

	if (!AttributeSet)
	{
		CLog::Print("APlayerCharacter::ManaChanged, AttributeSet is nullptr", -1, 10, FColor::Red);
		return;
	}
	float maxHealth = AttributeSet->GetMaxMana();

	if (!MainWidget)
	{
		CLog::Print("APlayerCharacter::ManaChanged, MainWidget is nullptr", -1, 10, FColor::Red);
		return;
	}
	MainWidget->ManaChanged(Data.NewValue / maxHealth);

}

void APlayerCharacter::MaxManaChanged(const FOnAttributeChangeData& Data)
{
	Super::MaxManaChanged(Data);

	if (!AttributeSet)
	{
		CLog::Print("APlayerCharacter::MaxManaChanged, AttributeSet is nullptr", -1, 10, FColor::Red);
		return;
	}
	float maxHealth = AttributeSet->GetMaxMana();

	if (!MainWidget)
	{
		CLog::Print("APlayerCharacter::MaxManaChanged, MainWidget is nullptr", -1, 10, FColor::Red);
		return;
	}
	//MainWidget->MaxManaChanged(float NewValue);
}

void APlayerCharacter::OffAllWidget()
{

}

FRotator APlayerCharacter::FindCursorRotation()
{
	FHitResult result;
	ADungeonPlayerController* controller = Cast<ADungeonPlayerController>(GetController());
	if (controller)
		controller->GetHitResultUnderCursor(ECC_Visibility, 1, result);

	FVector loc = GetActorLocation();
	
	float ratio = (loc.Z - result.TraceStart.Z) / (result.TraceEnd.Z - result.TraceStart.Z);

	FVector goal = result.TraceStart + (result.TraceEnd - result.TraceStart) * ratio;

	return UKismetMathLibrary::FindLookAtRotation(loc, goal);
}

void APlayerCharacter::Server_ReplicateRotation_Implementation(FRotator NewRotation)
{
	SetActorRotation(NewRotation);
	Multicast_ReplicateRotation(NewRotation);
}

void APlayerCharacter::Multicast_ReplicateRotation_Implementation(FRotator NewRotation)
{
	SetActorRotation(NewRotation);
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
	ADungeonPlayerController* controller = Cast<ADungeonPlayerController>(this->GetController());
	if (controller)
	{
		MainWidget = controller->GetMainWidget();
		if (MainWidget)
			MainWidget->GetQuickSlot()->ConnectComponent(Skill);
	}

	//SkilltreeComp
	{
		TFunction<void(int32, int32)> func;
		func = [this](int32 Idx, int32 SkillID)
		{
			ChangeQuickSlotData(Idx, SkillID);
		};
		SkillTree->Init(Skill->GetSkillDatas(), Skill, func);	
	}

	
	//Inventory
	Inventory->OnInventoryEquippedChanged.AddDynamic(this, &APlayerCharacter::ChangeAppearance);
	Inventory->OnChangeHairVisiblity.AddDynamic(this, &APlayerCharacter::SetShowHair);

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
		MinimapCapture->ShowFlags.Lighting = 0;
		MinimapCapture->ShowFlags.PathTracing = 0;
		MinimapCapture->ShowFlags.PostProcessing = 1;

		//rendertarget
		UTextureRenderTarget2D* target;
		CHelpers::GetAssetDynamic(&target, "TextureRenderTarget2D'/Game/Widgets/Minimap/RT_Minimap.RT_Minimap'");
		MinimapCapture->TextureTarget = target;
	}

	//Load data if its client
	if (controller && controller->IsLocalController())
	{
		USaveGameData* saveGameData = Cast<USaveGameData>(UGameplayStatics::LoadGameFromSlot(USaveManager::GetCurrentSaveSlot(), 0));
		if (!saveGameData) { CLog::Print("saveGameData is nullptr"); return; }
		OnAfterLoad(saveGameData);
	}

	if (HealthBarWidget)
	{
		//HealthBarWidget->Init(Name, Status->GetLevel());
		if (controller && controller->IsLocalController())
			HealthBarWidget->SetPlayerType();
		else
			HealthBarWidget->SetOtherPlayerType();
	}
	else CLog::Print("APlayerCharacter::Init HealthBarWidget cast failed", -1, 10, FColor::Red);

}


void APlayerCharacter::InitClientWidget()
{
}

void APlayerCharacter::ChangeAppearance(EAppearancePart InMeshPart, int32 InIndex)
{
	Appearance->Server_ChangeAppearance(InMeshPart, InIndex);
}

void APlayerCharacter::ChangeColorData(EAppearancePart InMeshPart, FName Parameter, FLinearColor NewColor)
{
	Appearance->Server_ChangeColor(InMeshPart, Parameter, NewColor);
}

void APlayerCharacter::SetShowHair(bool NewState)
{
	Appearance->Server_SetShowHair(NewState);
}

void APlayerCharacter::UseSkill(int32 Idx)
{
	Super::UseSkill(Idx);
	Skill->UseQuickSlotSkill(Idx);
}

void APlayerCharacter::Dead()
{
	Super::Dead();

	TopDownCameraComponent->PostProcessSettings.ColorSaturation = FVector4(0, 0, 0, 1);
	TopDownCameraComponent->PostProcessSettings.bOverride_ColorSaturation = 1;

	ADungeonPlayerController* controller = Cast<ADungeonPlayerController>(this->GetController());
	if (controller && controller->IsLocalController())
		controller->CreateDeadWidget();
}

void APlayerCharacter::UseLeft()
{
	CheckFalse(CanUse());

	UseSkill(0);
}

void APlayerCharacter::UseRight()
{
	CheckFalse(CanUse());

	Server_ReplicateRotation(FindCursorRotation());
	UseSkill(1);
}

void APlayerCharacter::UseQ()
{
	CheckFalse(CanUse());

	Server_ReplicateRotation(FindCursorRotation());
	UseSkill(2);
}

void APlayerCharacter::UseW()
{
	CheckFalse(CanUse());

	Server_ReplicateRotation(FindCursorRotation());
	UseSkill(3);
}

void APlayerCharacter::UseE()
{
	CheckFalse(CanUse());

	Server_ReplicateRotation(FindCursorRotation());
	UseSkill(4);
}

void APlayerCharacter::UseR()
{
	CheckFalse(CanUse());

	Server_ReplicateRotation(FindCursorRotation());
	UseSkill(5);
}

void APlayerCharacter::ChangeQuickSlotData(int32 Index, int32 InInputID)
{
	Skill->ChangeQuickSlotData(Index, InInputID);
}

void APlayerCharacter::TryAddItem(AEqquipment* InObject)
{
	Inventory->Server_TryAddItem(InObject->GetUniqueID());
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

void APlayerCharacter::ToggleQuest()
{
	Quest->ToggleWidget();
}

void APlayerCharacter::RevealSkillTree()
{
	SkillTree->OnWidget();
}

void APlayerCharacter::RevealInventory()
{
	Inventory->OnWidget();
}

void APlayerCharacter::RevealQuest()
{
	Quest->OnWidget();
}

void APlayerCharacter::HideSkillTree()
{
	SkillTree->OffWidget();
}

void APlayerCharacter::HideInventory()
{
	Inventory->OffWidget();
}

void APlayerCharacter::HideQuest()
{
	Quest->OffWidget();
}

FString APlayerCharacter::GetUniqueSaveName()
{
	return "Player";
}

void APlayerCharacter::OnBeforeSave(USaveGameData* SaveData)
{
	FString name = GetUniqueSaveName();
	CheckNull(SaveData);

	Inventory->SaveData(SaveData);
	Skill->SaveData(SaveData);
	SkillTree->SaveData(SaveData);
}

void APlayerCharacter::OnAfterLoad(USaveGameData* const ReadData)
{
	ADungeonPlayerController* controller = Cast<ADungeonPlayerController>(this->GetController());
	CheckFalse(controller && controller->IsLocalController());

	FString name = GetUniqueSaveName();
	CheckNull(ReadData);

	Server_SetName(FText::FromString(ReadData->PlayerData.CharacterName));

	Appearance->LoadData(ReadData);
	Inventory->LoadData(ReadData);

	if (HasAuthority())
		OnAfterLoad_ClientWidget(ReadData);
}

void APlayerCharacter::OnAfterLoad_ClientWidget(USaveGameData* const ReadData)
{
	// need skillactor repliaction
	Skill->LoadData(ReadData);
	SkillTree->LoadData(ReadData);
}
