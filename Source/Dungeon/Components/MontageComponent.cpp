#include "Components/MontageComponent.h"
#include "Global.h"

#include "Characters/DungeonCharacterBase.h"

UMontageComponent::UMontageComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMontageComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMontageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

int32 UMontageComponent::FindDirection(float FDot, float RDot)
{
	//  7  0
	// 6    1
	// 5    2
	//  4  3

	int32 result = 0;
	if (FDot >= 0) //forward
	{
		if (RDot >= 0)//right
		{
			if (FDot * FDot > 0.5)//forward-right
				result = 0;
			else //right-forward
				result = 1;
		}
		else //left
		{
			if (FDot * FDot > 0.5)//forward-left
				result = 7;
			else //left-forward
				result = 6;
		}
	}
	else //back
	{
		if (RDot >= 0)//right
		{
			if (FDot * FDot > 0.5)//back-right
				result = 3;
			else //right-back
				result = 2;
		}
		else //left
		{
			if (FDot * FDot > 0.5)//back-left
				result = 4;
			else //left-back
				result = 5;
		}
	}
	return result;
}

void UMontageComponent::PlayHitMontage(AActor* InCauser)
{
	ADungeonCharacterBase* owner = Cast<ADungeonCharacterBase>(GetOwner());
	CheckNull(owner);

	if (HitFronts.IsEmpty() && HitBacks.IsEmpty() && HitRights.IsEmpty() && HitLefts.IsEmpty())
	{
		CLog::Print(owner->GetName() + "'s Hit montages are empty", -1, 10, FColor::Red);
		return;
	}

	FVector look = InCauser->GetActorLocation() - owner->GetActorLocation();
	look.Z = 0;
	UKismetMathLibrary::Normal(look);

	FVector forward = owner->GetActorForwardVector();
	FVector right = owner->GetActorRightVector();

	float fdot = UKismetMathLibrary::Dot_VectorVector(forward, look);
	float rdot = UKismetMathLibrary::Dot_VectorVector(right, look);
	int32 direction = FindDirection(fdot,rdot);

	char arr[8] = { 'f','r','r','b','b','l','l','f' };
	int32 cnt[5] = { 2,2,2,2,0 };
	if (HitFronts.IsEmpty())
	{
		arr[0] = 'n';arr[7] = 'n';
		cnt[0] = 0; cnt[4] += 2;
	}
	if (HitRights.IsEmpty())
	{
		arr[1] = 'n';arr[2] = 'n';
		cnt[1] = 0; cnt[4] += 2;
	}
	if (HitBacks.IsEmpty())
	{
		arr[3] = 'n';arr[4] = 'n';
		cnt[2] = 0; cnt[4] += 2;
	}
	if (HitLefts.IsEmpty())
	{
		arr[5] = 'n';arr[6] = 'n';
		cnt[3] = 0; cnt[4] += 2;
	}

	while (cnt[4])
	{
		for (int32 i = 0; i < 8; ++i)
		{
			if (arr[i] == 'n')continue;
			if (arr[(i + 7) % 8] == 'n')
			{
				arr[(i + 7) % 8] = arr[i];
				--cnt[4];
			}
			else if (arr[(i + 1) % 8] == 'n')
			{
				arr[(i + 1) % 8] = arr[i];
				--cnt[4]; ++i;
			}
		}
	}

	TArray<UAnimMontage*>* montages=nullptr;

	switch (arr[direction])
	{
	case 'f':montages = &HitFronts;break;
	case 'r':montages = &HitRights;break;
	case 'b':montages = &HitBacks;break;
	case 'l':montages = &HitLefts;break;
	default:break;
	}
	CheckNull(montages);

	int32 idx = UKismetMathLibrary::RandomIntegerInRange(0, montages->Num() - 1);
	if (!montages->IsValidIndex(idx))
	{
		CLog::Print(arr[direction] + "'s montage, " + FString::FromInt(idx) + " not valid", -1, 10, FColor::Red);
		return;
	}
	owner->PlayAnimMontage((*montages)[idx]);
}

void UMontageComponent::PlayKnockDownMontage(FVector NewForce)
{
	Force = NewForce;

	ADungeonCharacterBase* owner = Cast<ADungeonCharacterBase>(GetOwner());
	CheckNull(owner);
	owner->PlayAnimMontage(KnockDownMontage);
}


//
// damagetype
// radial, normal
// normal, hit, knockback, knockdown
// 
// solve add force impulse network issue
//
