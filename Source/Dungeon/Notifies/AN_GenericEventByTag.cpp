#include "Notifies/AN_GenericEventByTag.h"
#include "Global.h"
#include "GameFramework/Character.h"

#include "Components/SkillComponent.h"

FString UAN_GenericEventByTag::GetNotifyName_Implementation() const
{
	return EventTag.GetTagName().ToString();
}

void UAN_GenericEventByTag::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	// for editor preview
	CheckNull(MeshComp);
	CheckNull(MeshComp->GetOwner());
	ACharacter* owner = Cast<ACharacter>(MeshComp->GetOwner());
	CheckNull(owner);

	USkillComponent* skill = CHelpers::GetComponent<USkillComponent>(owner);
	CheckNull(skill);
	FGameplayEventData data;
	skill->HandleGameplayEvent(EventTag, &data);
}
