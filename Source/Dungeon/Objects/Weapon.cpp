#include "Objects/Weapon.h"
#include "Global.h"
#include "Components/ShapeComponent.h"

#include "Characters/DungeonCharacterBase.h"
#include "Components/SkillComponent.h"

#include "Objects/ItemManager.h"
#include "Objects/ItemObject.h"
#include "Objects/CustomDamageType.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = 1;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicated 변수를 여기에 추가
	//DOREPLIFETIME_CONDITION(AWeapon, Mode, COND_None);
}

void AWeapon::OnRep_Mode()
{
	Super::OnRep_Mode();
}

void AWeapon::FindComponents()
{
	Super::FindComponents();

	// find collision
	TArray<UShapeComponent*> shapeComponents;
	GetComponents<UShapeComponent>(shapeComponents);
	for (UShapeComponent* component : shapeComponents)
	{
		for (auto i : component->ComponentTags)
		{
			if (i == FName("OverlapCollision"))
			{
				CollisionComponents.Add(component);
				break;
			}
		}
	}
	for (UShapeComponent* component : CollisionComponents)
	{
		component->OnComponentBeginOverlap.Clear();
		component->OnComponentHit.Clear();
		component->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnComponentBeginOverlap);
		component->OnComponentHit.AddDynamic(this, &AWeapon::OnComponentHit);
	}
}

void AWeapon::SpawnLootEffects()
{
	Super::SpawnLootEffects();
}

void AWeapon::SetEffectLocation()
{
	Super::SetEffectLocation();
}

void AWeapon::SortMesh()
{
	Super::SortMesh();
}

void AWeapon::ActivateEffect()
{
	Super::ActivateEffect();
}

void AWeapon::DeactivateEffect()
{
	Super::DeactivateEffect();
}

void AWeapon::SetPickableMode()
{
	// Off Collision
	for (UShapeComponent* component : CollisionComponents)
		component->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Super::SetPickableMode();

}

void AWeapon::SetInventoryMode()
{
	// Off Collision
	for (UShapeComponent* component : CollisionComponents)
		component->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Super::SetInventoryMode();
}

void AWeapon::SetEquipMode()
{
	// Off Collision
	for (UShapeComponent* component : CollisionComponents)
		component->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	Super::SetEquipMode();
}

void AWeapon::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CheckTrue(HitActors.Contains(OtherActor));
	IGenericTeamAgentInterface* other = Cast<IGenericTeamAgentInterface>(OtherActor);
	ACharacter* otherCh = Cast<ACharacter>(OtherActor);
	if (!other && !otherCh)	return;

	USkillComponent* skill = CHelpers::GetComponent<USkillComponent>(OtherActor);
	if (!skill)return;
	if (skill->IsDead())return;


	//ignore alliance
	CheckTrue(other->GetGenericTeamId() == TeamID);

	SendDamage(Damage, OtherActor, SweepResult);
}

void AWeapon::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

}

void AWeapon::SendDamage(float InDamage, AActor* OtherActor, const FHitResult& SweepResult)
{
	CheckFalse(HasAuthority());
	ACharacter* ch = Cast<ACharacter>(GetOwner());
	AController* inst = nullptr;
	if (ch)inst = ch->GetController();

	FDamageEvent f;
	f.DamageTypeClass = DamageTypeClass;
	OtherActor->TakeDamage(InDamage, f, inst, this);
	HitActors.Add(OtherActor);
}

void AWeapon::SetOwnerCharacter(ACharacter* InCharacter)
{
	Super::SetOwnerCharacter(InCharacter);
	IGenericTeamAgentInterface* id = Cast<IGenericTeamAgentInterface>(InCharacter);
	CheckNull(id)
	SetTeamID(id->GetGenericTeamId());
}

void AWeapon::OnCollision()
{
	for (auto i : CollisionComponents)
		i->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeapon::OffCollision()
{
	for (auto i : CollisionComponents)
		i->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::ResetHitActors()
{
	HitActors.Empty();
}

void AWeapon::SetItemLocation(const FVector& NewLocation, bool bSweep, FHitResult* OutSweepHitResult, ETeleportType Teleport)
{
	Super::SetItemLocation(NewLocation, bSweep, OutSweepHitResult, Teleport);
}

void AWeapon::SetItemRotation(FRotator NewRotation, ETeleportType Teleport)
{
	Super::SetItemRotation(NewRotation, Teleport);
}

void AWeapon::AttachItemToComponent(USceneComponent* Parent, const FAttachmentTransformRules& AttachmentRules, FName InSocketName)
{
	Super::AttachItemToComponent(Parent,AttachmentRules,InSocketName);
}

void AWeapon::ChangeVisibility(EItemMode InMode)
{
	Super::ChangeVisibility(InMode);
}

void AWeapon::SetMode(EItemMode InMode)
{
	Super::SetMode(InMode);
}