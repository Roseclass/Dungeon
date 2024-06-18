#include "Notifies/ANS_KnockBack.h"
#include "Global.h"
#include "GameFramework/Character.h"

#include "GameFramework/CharacterMovementComponent.h"

FString UANS_KnockBack::GetNotifyName_Implementation() const
{
	return "KnockBack";
}

void UANS_KnockBack::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	//// for editor preview
	//CheckNull(MeshComp);
	//CheckNull(MeshComp->GetOwner());
	//ACharacter* owner = Cast<ACharacter>(MeshComp->GetOwner());
	//CheckNull(owner);

	////server only
	//CheckFalse(owner->HasAuthority());

	//UMontageComponent* montage = CHelpers::GetComponent<UMontageComponent>(owner);
	//CheckNull(montage);
	//owner->GetCharacterMovement()->AddForce(montage->GetForce());
}
