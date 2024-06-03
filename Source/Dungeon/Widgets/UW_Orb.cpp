#include "Widgets/UW_Orb.h"
#include "Global.h"
#include "Components/ProgressBar.h"

void UUW_Orb::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (Max > 0 && bUpdate)// and player is alive
	{
		float cur = Orb->Percent;
		cur += InDeltaTime * Regen / Max;
		if (cur > 1)cur = 1;
		Orb->SetPercent(cur);
	}
}

void UUW_Orb::SetMax(float NewMax)
{
	Max = NewMax;
}

void UUW_Orb::SetPercent(float NewPercent)
{
	Orb->SetPercent(NewPercent);
}

void UUW_Orb::SetRegen(float NewRegen)
{
	Regen = NewRegen;
}

void UUW_Orb::Dead()
{
	bUpdate = 0;
	SetPercent(0);
}
