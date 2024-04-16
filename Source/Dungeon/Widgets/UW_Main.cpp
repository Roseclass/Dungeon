#include "Widgets/UW_Main.h"
#include "Global.h"

#include "Components/StatusComponent.h"

#include "Widgets/UW_Orb.h"

void UUW_Main::NativeConstruct()
{
	Super::NativeConstruct();

	APawn* pawn = GetOwningPlayer()->GetPawn();
	if (pawn)
	{
		UStatusComponent* status = CHelpers::GetComponent<UStatusComponent>(pawn);
		if (status)
		{
			status->OnMaxHealthChanged.AddUFunction(HealthOrb, "SetMax");
			status->OnCurrentHealthChanged.AddUFunction(HealthOrb, "SetCurrent");
			status->OnHealthRegenChanged.AddUFunction(HealthOrb, "SetRegen");

			status->OnMaxManaChanged.AddUFunction(ManaOrb, "SetMax");
			status->OnCurrentManaChanged.AddUFunction(ManaOrb, "SetCurrent");
			status->OnManaRegenChanged.AddUFunction(ManaOrb, "SetRegen");			
		}
	}

}
