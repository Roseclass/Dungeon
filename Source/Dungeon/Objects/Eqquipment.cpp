#include "Objects/Eqquipment.h"
#include "Global.h"
#include "Components/ShapeComponent.h"
#include "Components/MeshComponent.h"
#include "Components/SplineComponent.h"
#include "Components/WidgetComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "GameplayEffect.h"

#include "DungeonPlayerController.h"
#include "Characters/PlayerCharacter.h"
#include "Objects/ItemObject.h"
#include "Widgets/UW_Item.h"
#include "Components/InventoryComponent.h"

#define RateMax 1.2f
#define RateMin 0.8f

AEqquipment::AEqquipment()
{
	PrimaryActorTick.bCanEverTick = true;

	HelmsAppearanceDatas.Init(FItemAppearanceData(),2);
	HelmsAppearanceDatas[0].PartType = EAppearancePart::HeadCoverings_NoHair;
	HelmsAppearanceDatas[1].PartType = EAppearancePart::HeadAttachment_Helmet;

	UpperBodyAppearanceDatas.Init(FItemAppearanceData(),7);
	UpperBodyAppearanceDatas[0].PartType = EAppearancePart::Torso;
	UpperBodyAppearanceDatas[1].PartType = EAppearancePart::ShoulderAttachment_Right;
	UpperBodyAppearanceDatas[2].PartType = EAppearancePart::ArmUpperRight;
	UpperBodyAppearanceDatas[3].PartType = EAppearancePart::ArmLowerRight;
	UpperBodyAppearanceDatas[4].PartType = EAppearancePart::ShoulderAttachment_Left;
	UpperBodyAppearanceDatas[5].PartType = EAppearancePart::ArmUpperLeft;
	UpperBodyAppearanceDatas[6].PartType = EAppearancePart::ArmLowerLeft;

	LowerBodyAppearanceDatas.Init(FItemAppearanceData(), 2);
	LowerBodyAppearanceDatas[0].PartType = EAppearancePart::Hips;
	LowerBodyAppearanceDatas[1].PartType = EAppearancePart::HipsAttachment;

	BootsAppearanceDatas.Init(FItemAppearanceData(), 4);
	BootsAppearanceDatas[0].PartType = EAppearancePart::KneeAttachment_Right;
	BootsAppearanceDatas[1].PartType = EAppearancePart::LegRight;
	BootsAppearanceDatas[2].PartType = EAppearancePart::KneeAttachment_Left;
	BootsAppearanceDatas[3].PartType = EAppearancePart::LegLeft;

	GlovesAppearanceDatas.Init(FItemAppearanceData(), 2);
	GlovesAppearanceDatas[0].PartType = EAppearancePart::HandRight;
	GlovesAppearanceDatas[1].PartType = EAppearancePart::HandLeft;

	CHelpers::CreateComponent(this, &Root, "Root");

	CHelpers::CreateComponent(this, &DropSpline, "DropSpline", Root);
	DropSpline->SetAbsolute(1, 1, 1);
	DropSpline->AddSplinePoint(FVector(), ESplineCoordinateSpace::World, 1);

	CHelpers::CreateComponent(this, &NameWidget, "Name", Root);
	NameWidget->SetDrawAtDesiredSize(1);
	NameWidget->SetWidgetSpace(EWidgetSpace::Screen);
	NameWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEqquipment::BeginPlay()
{
	Super::BeginPlay();

	ItemObject = NewObject<UItemObject>(this, FName("Item"));
	ItemObject->Init(DimensionX, DimensionY, Icon, IconRotated, this, ItemType);

	FindComponents();

	SpawnLootEffects();

	// Randomize status
	if (GetWorld()->GetFirstPlayerController()->GetLocalRole() == ENetRole::ROLE_Authority && !ItemStatus.bRandomize)
	{	
		for (auto i : TargetAttributes)
		{
			float rate = UKismetMathLibrary::RandomFloatInRange(RateMax, RateMin);
			ItemStatus.TargetAttributes.Add(i.Key);
			ItemStatus.TargetAttributeValues.Add(i.Value * rate);
			ItemStatus.TargetAttributeGrades.Add(CheckGrade(rate));
		}

		if (EnhancementDataTable)
		{
			TArray<FSkillEnhancement*> enhancementDatas;
			EnhancementDataTable->GetAllRows<FSkillEnhancement>("", enhancementDatas);
			for (auto i : enhancementDatas)
			{
				float rate = UKismetMathLibrary::RandomFloatInRange(RateMax, RateMin);
				i->EnhanceStatus = i->EnhanceStatus * rate;
				ItemStatus.EnhancementDatas.Add(*i);
				ItemStatus.EnhancementGrades.Add(CheckGrade(rate));
			}
		}

		ItemStatus.bRandomize = 1;
	}

	if (DropCurve)
	{
		DropTimelineFloat.BindUFunction(this, "DropTickFunction");
		DropTimeLine.AddInterpFloat(DropCurve, DropTimelineFloat);
	}

	{
		UUW_Item* widget = Cast<UUW_Item>(NameWidget->GetWidget());
		if (widget)
			widget->SetName(ItemStatus.Name);
		else CLog::Print(GetName() + " AEqquipment::BeginPlay widget is nullptr", -1, 10, FColor::Red);
	}
}

#if WITH_EDITOR
void AEqquipment::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (ItemType == EItemType::Helms)
	{
		HelmsAppearanceDatas[0].Index = FMath::Clamp(HelmsAppearanceDatas[0].Index, 0, HeadCoverings_NoHair_MAX);
		HelmsAppearanceDatas[1].Index = FMath::Clamp(HelmsAppearanceDatas[1].Index, 0, HeadAttachment_Helmet_MAX);
	}
	else if (ItemType == EItemType::UpperBody)
	{
		UpperBodyAppearanceDatas[0].Index = FMath::Clamp(UpperBodyAppearanceDatas[0].Index, 0, Torso_MAX);
		UpperBodyAppearanceDatas[1].Index = FMath::Clamp(UpperBodyAppearanceDatas[1].Index, 0, ShoulderAttachment_Right_MAX);
		UpperBodyAppearanceDatas[2].Index = FMath::Clamp(UpperBodyAppearanceDatas[2].Index, 0, ArmUpperRight_MAX);
		UpperBodyAppearanceDatas[3].Index = FMath::Clamp(UpperBodyAppearanceDatas[3].Index, 0, ArmLowerRight_MAX);
		UpperBodyAppearanceDatas[4].Index = FMath::Clamp(UpperBodyAppearanceDatas[4].Index, 0, ShoulderAttachment_Left_MAX);
		UpperBodyAppearanceDatas[5].Index = FMath::Clamp(UpperBodyAppearanceDatas[5].Index, 0, ArmUpperLeft_MAX);
		UpperBodyAppearanceDatas[6].Index = FMath::Clamp(UpperBodyAppearanceDatas[6].Index, 0, ArmLowerLeft_MAX);
	}
	else if (ItemType == EItemType::LowerBody)
	{
		LowerBodyAppearanceDatas[0].Index = FMath::Clamp(LowerBodyAppearanceDatas[0].Index, 0, Hips_MAX);
		LowerBodyAppearanceDatas[1].Index = FMath::Clamp(LowerBodyAppearanceDatas[1].Index, 0, HipsAttachment_MAX);
	}
	else if (ItemType == EItemType::Boots)
	{
		BootsAppearanceDatas[0].Index = FMath::Clamp(BootsAppearanceDatas[0].Index, 0, KneeAttachment_Right_MAX);
		BootsAppearanceDatas[1].Index = FMath::Clamp(BootsAppearanceDatas[1].Index, 0, LegRight_MAX);
		BootsAppearanceDatas[2].Index = FMath::Clamp(BootsAppearanceDatas[2].Index, 0, KneeAttachment_Left_MAX);
		BootsAppearanceDatas[3].Index = FMath::Clamp(BootsAppearanceDatas[3].Index, 0, LegLeft_MAX);
	}
	else if (ItemType == EItemType::Gloves)
	{
		GlovesAppearanceDatas[0].Index = FMath::Clamp(GlovesAppearanceDatas[0].Index, 0, HandRight_MAX);
		GlovesAppearanceDatas[1].Index = FMath::Clamp(GlovesAppearanceDatas[1].Index, 0, HandLeft_MAX);
	}
}
#endif

void AEqquipment::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DropTimeLine.TickTimeline(DeltaTime);

	CheckTrue(!NiagaraPickEffect && !ParticlePickEffect);
	if (NiagaraPickEffect)
	{
		NiagaraPickEffect->SetWorldRotation(LootEffectWorldRotation);
	}
	if (ParticlePickEffect)
	{
		ParticlePickEffect->SetWorldRotation(LootEffectWorldRotation);
	}

	return;
	for (int32 i = 0; i < ItemStatus.EnhancementDatas.Num(); ++i)
	{
		FString result = "";
		FString statusValue = FString::Printf(TEXT("%.2f"), ItemStatus.EnhancementDatas[i].EnhanceStatus);

		if (ItemStatus.EnhancementDatas[i].EnhanceStatusTag == FGameplayTag::RequestGameplayTag("Skill.Cost.Additive"))
			result += TEXT("마나 소모량 ") + statusValue + TEXT("감소");
		else if (ItemStatus.EnhancementDatas[i].EnhanceStatusTag == FGameplayTag::RequestGameplayTag("Skill.Cost.Multiplicitive"))
			result += TEXT("마나 소모량 ") + statusValue + TEXT("%만큼 감소");
		else if (ItemStatus.EnhancementDatas[i].EnhanceStatusTag == FGameplayTag::RequestGameplayTag("Skill.Cooldown.Additive"))
			result += TEXT("재사용 대기시간 ") + statusValue + TEXT("초 감소");
		else if (ItemStatus.EnhancementDatas[i].EnhanceStatusTag == FGameplayTag::RequestGameplayTag("Skill.Cooldown.Multiplicitive"))
			result += TEXT("재사용 대기시간 ") + statusValue + TEXT("%만큼 감소");
		else if (ItemStatus.EnhancementDatas[i].EnhanceStatusTag == FGameplayTag::RequestGameplayTag("Skill.Damage.Additive"))
			result += TEXT("데미지 ") + statusValue + TEXT("증가");
		else if (ItemStatus.EnhancementDatas[i].EnhanceStatusTag == FGameplayTag::RequestGameplayTag("Skill.Damage.Multiplicitive"))
			result += TEXT("데미지 ") + statusValue + TEXT("%만큼 증가");

		CLog::Print(result, -1, 0);
	}
	CLog::Print(GetName(), -1, 0);

}

void AEqquipment::StartInteract(ADungeonPlayerController* InPlayer)
{
	APlayerCharacter* const myPawn = Cast<APlayerCharacter>(InPlayer->GetPawn());
	CheckNull(myPawn);
	myPawn->TryAddItem(this);
}

void AEqquipment::EndInteract(ADungeonPlayerController* InPlayer)
{

}

void AEqquipment::StartCursorOver(ADungeonPlayerController* InPlayer)
{
	UUW_Item* widget = Cast<UUW_Item>(NameWidget->GetWidget());
	if (widget)
		widget->StartCursorOver();

	for (auto i : MeshComponents)
		i->SetRenderCustomDepth(1);
}

void AEqquipment::EndCursorOver(ADungeonPlayerController* InPlayer)
{
	UUW_Item* widget = Cast<UUW_Item>(NameWidget->GetWidget());
	if (widget)
		widget->EndCursorOver();

	for (auto i : MeshComponents)
		i->SetRenderCustomDepth(0);
}

bool AEqquipment::IsInteractable()
{
	return bInteractable;
}

EItemGrade AEqquipment::CheckGrade(float Rate)
{
	float gap = (RateMax - RateMin) / 4;

	if (RateMin <= Rate && Rate < RateMin + gap)return EItemGrade::Common;
	else if (RateMin + gap <= Rate && Rate < RateMin + (gap * 2))return EItemGrade::Uncommon;
	else if (RateMin + (gap * 2) <= Rate && Rate < RateMin + (gap * 3))return EItemGrade::Rare;
	else if (RateMin + (gap * 3) <= Rate && Rate <= RateMax)return EItemGrade::Unique;
	return EItemGrade::Max;
}

void AEqquipment::FindComponents()
{
	// find interact collision
	TArray<UShapeComponent*> shapeComponents;
	GetComponents<UShapeComponent>(shapeComponents);
	for (UShapeComponent* component : shapeComponents)
	{
		for (auto i : component->ComponentTags)
		{
			if (i == FName("InteractCollision"))
			{
				InteractCollisionComponent = component;
				break;
			}
		}
	}
	if (!InteractCollisionComponent)
		CLog::Print("AEqquipment::FindComponents InteractCollisionComponent is nullptr", -1, 10, FColor::Red);

	// find mesh
	GetComponents<UMeshComponent>(MeshComponents);
	MeshComponents.Remove(NameWidget);
}

void AEqquipment::SpawnLootEffects()
{
	UNiagaraSystem* n = Cast<UNiagaraSystem>(LootEffect);
	UParticleSystem* p = Cast<UParticleSystem>(LootEffect);

	UFXSystemComponent* fx = nullptr;
	if (n)
	{
		NiagaraPickEffect = UNiagaraFunctionLibrary::SpawnSystemAttached(n, MeshComponents[0], NAME_None, FVector(), FRotator(), EAttachLocation::SnapToTarget, 1);
		NiagaraPickEffect->SetIsReplicated(1);
		NiagaraPickEffect->SetAutoDestroy(0);
		fx = NiagaraPickEffect;
	}
	else if (p)
	{
		ParticlePickEffect = UGameplayStatics::SpawnEmitterAttached(p, MeshComponents[0], NAME_None, FVector(), FRotator(), EAttachLocation::SnapToTarget);
		ParticlePickEffect->SetIsReplicated(1);
		ParticlePickEffect->bAutoDestroy = 0;
		fx = ParticlePickEffect;
	}

	if (fx)
	{
		for (auto i : LootEffectParams)
		{
			if (i.ParamType == PSPT_Scalar)fx->SetFloatParameter(i.Name, i.Scalar);
			else if (i.ParamType == PSPT_Vector)fx->SetVectorParameter(i.Name, i.Vector);
			else if (i.ParamType == PSPT_Color)fx->SetColorParameter(i.Name, i.Color);
			else if (i.ParamType == PSPT_Actor)fx->SetActorParameter(i.Name, i.Actor);
		}
	}
	if (!bInteractable)DeactivateEffect();
}

void AEqquipment::SetEffectLocation()
{

}

void AEqquipment::SortMesh()
{

}

void AEqquipment::ActivateEffect()
{
	CheckTrue(!NiagaraPickEffect && !ParticlePickEffect);
	if (NiagaraPickEffect)
	{
		NiagaraPickEffect->SetVisibility(1);
	}
	if (ParticlePickEffect)
	{
		ParticlePickEffect->SetVisibility(1);
	}
}

void AEqquipment::DeactivateEffect()
{
	CheckTrue(!NiagaraPickEffect && !ParticlePickEffect);
	if (NiagaraPickEffect)NiagaraPickEffect->SetVisibility(0);
	if (ParticlePickEffect)ParticlePickEffect->SetVisibility(0);
}

void AEqquipment::SetLootMode(const FEquipmentStateUpdateParameters& UpdateParameters)
{
	// Stop DropSequence
	DropTimeLine.Stop();

	// Change State
	Mode = EItemMode::Loot;
	bInteractable = 1;

	// Chnage Location
	SetActorLocation(UpdateParameters.Location);

	// Change Owner
	SetOwnerCharacter(nullptr);

	// Save Field Loaction, Save Mesh Location, Adjust Effect Location
	SetEffectLocation();

	// On Appearance
	for (auto component : MeshComponents)
		component->SetVisibility(1);

	// Show Name Widget
	NameWidget->SetVisibility(1);

	// On Interaction Collision
	if (InteractCollisionComponent)
		InteractCollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	// On Item Effects
	ActivateEffect();
}

void AEqquipment::SetInventoryMode(const FEquipmentStateUpdateParameters& UpdateParameters)
{
	// Stop DropSequence
	DropTimeLine.Stop();

	// Change State
	Mode = EItemMode::Inventory;
	bInteractable = 0;

	// Change Owner
	SetOwnerCharacter(UpdateParameters.NewOwner);

	// Off Appearance
	for (auto component : MeshComponents)
		component->SetVisibility(0);

	// hide name widget
	NameWidget->SetVisibility(0);

	// Off Interaction Collision
	if(InteractCollisionComponent)
		InteractCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Off Item Effects
	DeactivateEffect();
}

void AEqquipment::SetEquipMode(const FEquipmentStateUpdateParameters& UpdateParameters)
{
	// Stop DropSequence
	DropTimeLine.Stop();

	// Change State
	Mode = EItemMode::Equip;
	bInteractable = 0;

	// Change Owner
	SetOwnerCharacter(UpdateParameters.NewOwner);

	// On Appearance
	for (auto component : MeshComponents)
		component->SetVisibility(1);

	// hide name widget
	NameWidget->SetVisibility(0);

	// Off Interaction Collision
	if (InteractCollisionComponent)
		InteractCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Off Item Effects
	DeactivateEffect();
}

void AEqquipment::SetDropMode(const FEquipmentStateUpdateParameters& UpdateParameters)
{
	// Stop DropSequence
	DropTimeLine.Stop();

	// Change State
	Mode = EItemMode::Drop;
	bInteractable = 0;
	DropStart = UpdateParameters.DropStart;
	DropEnd = UpdateParameters.DropEnd;

	// Change Owner
	SetOwnerCharacter(nullptr);

	// On Appearance
	for (auto component : MeshComponents)
		component->SetVisibility(1);

	// hide name widget
	NameWidget->SetVisibility(1);

	// Off Interaction Collision
	if (InteractCollisionComponent)
		InteractCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Off Item Effects
	DeactivateEffect();

	// Play DropSequence
	PlayDropSequence(UpdateParameters.DropStart, UpdateParameters.DropEnd);
}

void AEqquipment::SetOwnerCharacter(ADungeonCharacterBase* InCharacter)
{
	OwnerCharacter = InCharacter;

	for (auto i : MeshComponents)
		i->SetRelativeRotation(FRotator(0, 0, 0));

	if (!OwnerCharacter)
	{
		FDetachmentTransformRules f = FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepRelative, EDetachmentRule::KeepWorld, 1);
		DetachFromActor(f);
	}
	else
	{
		FAttachmentTransformRules f = { EAttachmentRule::SnapToTarget, 1 };
		AttachToComponent(OwnerCharacter->GetMesh(), f, GetSocketName());
	}
}

void AEqquipment::PlayDropSequence(FVector Start, FVector End)
{
	DropSpline->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

	FVector mid = Start;
	mid.X += End.X; mid.X /= 2;
	mid.Y += End.Y; mid.Y /= 2;
	mid.Z += UKismetMathLibrary::Abs(Start.Z - End.Z) * 1.5;
	DropSpline->SetLocationAtSplinePoint(0, Start, ESplineCoordinateSpace::World);
	DropSpline->SetLocationAtSplinePoint(1, mid, ESplineCoordinateSpace::World);
	DropSpline->SetLocationAtSplinePoint(2, End, ESplineCoordinateSpace::World);

	// play timeline
	SetActorRotation(FRotator(0, 0, -90));

	FOnTimelineEventStatic end;
	end.BindLambda([this, End]()
	{
		FEquipmentStateUpdateParameters params;
		params.Location = End;
		SetLootMode(params);
	});
	DropTimeLine.SetTimelineFinishedFunc(end);

	DropTimeLine.SetPlayRate(Speed);
	DropTimeLine.PlayFromStart();
}

void AEqquipment::DropTickFunction(float Value)
{
	SetActorLocation(DropSpline->GetLocationAtTime(Value, ESplineCoordinateSpace::World));
	float pitch = UKismetMathLibrary::DegCos(Value * 360 * RotationCount)* RotationAngle;
	float yaw = 0;
	float roll = UKismetMathLibrary::DegSin(Value * 360 * RotationCount)* RotationAngle;
	for (auto i : MeshComponents)
		i->SetRelativeRotation(FRotator(pitch, 0, roll));
}

void AEqquipment::AssignUniqueID(FString NewUniqueID)
{
	CheckTrue(UniqueID != FString());
	UniqueID = NewUniqueID;
}

void AEqquipment::UpdateState(const FEquipmentStateUpdateParameters& UpdateParameters)
{
	if (UpdateParameters.State == EItemMode::Loot)//Visible,Interactable,NoOwner
		SetLootMode(UpdateParameters);
	else if (UpdateParameters.State == EItemMode::Inventory)//Invisible,NotInteractable,HasOwner
		SetInventoryMode(UpdateParameters);
	else if (UpdateParameters.State == EItemMode::Equip)//Visible,NotInteractable,HasOwner
		SetEquipMode(UpdateParameters);
	else if (UpdateParameters.State == EItemMode::Drop)//Visible,Interactable After DropSequence,NoOwner
		SetDropMode(UpdateParameters);
}

void AEqquipment::UpdateStatus(const FItemStatusData& UpdateData)
{
	ItemStatus = UpdateData;
}

void AEqquipment::Load(const FItemStatusData& InData)
{
	UpdateStatus(InData);
}

const TArray<FItemAppearanceData>& AEqquipment::GetAppearanceDatas()const
{
	if (ItemType == EItemType::Helms)
		return HelmsAppearanceDatas;
	else if (ItemType == EItemType::UpperBody)
		return UpperBodyAppearanceDatas;
	else if (ItemType == EItemType::LowerBody)
		return LowerBodyAppearanceDatas;
	else if (ItemType == EItemType::Boots)
		return BootsAppearanceDatas;
	else if (ItemType == EItemType::Gloves)
		return GlovesAppearanceDatas;

	CLog::Print("AEqquipment::GetAppearanceDatas");
	return HelmsAppearanceDatas;
}

FEquipmentStateUpdateParameters AEqquipment::GetEquipmentState() const
{
	FEquipmentStateUpdateParameters result;
	result.State = Mode;
	result.NewOwner = OwnerCharacter;
	result.Location = GetActorLocation();
	result.DropStart = DropStart;
	result.DropEnd = DropEnd;
	return result;
}

void AEqquipment::GetDimensions(int32& X, int32& Y)
{
	X = DimensionX;
	Y = DimensionY;
}
