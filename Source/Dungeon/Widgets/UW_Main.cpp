#include "Widgets/UW_Main.h"
#include "Global.h"

#include "Components/StatusComponent.h"

#include "Widgets/UW_Orb.h"

void UUW_Main::NativeOnInitialized()
{
	Super::NativeOnInitialized();

}

void UUW_Main::Init(APawn* NewPawn)
{
	if (NewPawn)
	{
		CLog::Print("IN");
		UStatusComponent* status = CHelpers::GetComponent<UStatusComponent>(NewPawn);
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
	else CLog::Print("UUW_Main::Init NewPawn is nullptr", -1, 10, FColor::Red);
}