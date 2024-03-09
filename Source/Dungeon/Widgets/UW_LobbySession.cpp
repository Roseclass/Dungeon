#include "Widgets/UW_LobbySession.h"
#include "Global.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "GameFramework/PlayerState.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

void UUW_LobbySession::NativeConstruct()
{
	Super::NativeConstruct();
	Create->OnClicked.AddDynamic(this, &UUW_LobbySession::OnCreateButtonClicked);
	OnCreateSessionComplete.BindUFunction(this, "OnCreateSessionCompleted");
	OnStartSessionComplete.BindUFunction(this, "OnStartSessionCompleted");
}

void UUW_LobbySession::OnCreateButtonClicked()
{
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	APlayerState* state = GetOwningPlayer()->GetPlayerState<APlayerState>();

	if (OnlineSub && state)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		FUniqueNetIdRepl UserId = state->GetUniqueId();

		if (Sessions.IsValid()&& UserId.IsValid())
		{
			FOnlineSessionSettings SessionSettings;

			SessionSettings.bIsLANMatch = bIsLAN;
			SessionSettings.bUsesPresence = true;
			SessionSettings.NumPublicConnections = MaxNumPlayers;
			SessionSettings.NumPrivateConnections = 0;
			SessionSettings.bAllowInvites = true;
			SessionSettings.bAllowJoinInProgress = true;
			SessionSettings.bShouldAdvertise = true;
			SessionSettings.bAllowJoinViaPresence = true;
			SessionSettings.bAllowJoinViaPresenceFriendsOnly = false;
						   
			SessionSettings.Set(SETTING_MAPNAME, FString("NewMap"), EOnlineDataAdvertisementType::ViaOnlineService);

			CreateSessionCompleteHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionComplete);
			FName SessionName = FName(SessionNameBox->GetText().ToString());
			Sessions->CreateSession(*UserId.GetUniqueNetId(), SessionName, SessionSettings);
		}
	}
	else
		CLog::Print(__FUNCTION__, -1, 10.f, FColor::Red);
}

void UUW_LobbySession::OnCreateSessionCompleted(FName SessionName, bool bWasSuccessful)
{
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteHandle);
			if (bWasSuccessful)
			{
				StartSessionCompleteHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionComplete);
				Sessions->StartSession(SessionName);
				return;
			}
		}
	}

	if (!bWasSuccessful)
		CLog::Print(__FUNCTION__, -1, 10.f, FColor::Red);
}

void UUW_LobbySession::OnStartSessionCompleted(FName SessionName, bool bWasSuccessful)
{
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnStartSessionCompleteDelegate_Handle(StartSessionCompleteHandle);
		}
	}

	if (bWasSuccessful)
	{
		//·¹º§¿ÀÇÂ
		CLog::Print("Success", -1, 10.f, FColor::Green);
	}
	else
	{
		CLog::Print("Failed", -1, 10.f, FColor::Red);
	}
}

void UUW_LobbySession::OnFindButtonClicked()
{
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	APlayerState* state = GetOwningPlayer()->GetPlayerState<APlayerState>();

	if (OnlineSub && state)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		FUniqueNetIdRepl UserId = state->GetUniqueId();

		if (Sessions.IsValid() && UserId.IsValid())
		{
			FindSessionsHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessions);

			SessionSearch = MakeShareable(new FOnlineSessionSearch());

			SessionSearch->MaxSearchResults = 25;
			SessionSearch->bIsLanQuery = 1;
			SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

			Sessions->FindSessions(*UserId.GetUniqueNetId(), SessionSearch.ToSharedRef());

			return;
		}
	}
	else
		CLog::Print(__FUNCTION__, -1, 10.f, FColor::Red);
}

void UUW_LobbySession::OnFindSessionsCompleted(bool bSuccess)
{
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsHandle);
			for (auto i : SessionSearch->SearchResults)
			{
				//add join tab
			}
		}
	}

	if (bSuccess)
	{
		CLog::Print("Success", -1, 10.f, FColor::Green);
	}
	else
	{
		CLog::Print("Failed", -1, 10.f, FColor::Red);
	}
}

void UUW_LobbySession::OnJoinButtonClicked()
{
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	APlayerState* state = GetOwningPlayer()->GetPlayerState<APlayerState>();

	if (OnlineSub && state)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		FUniqueNetIdRepl UserId = state->GetUniqueId();

		if (Sessions.IsValid() && UserId.IsValid())
		{
			FindSessionsHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessions);

			FName SessionName;

			Sessions->JoinSession(*UserId.GetUniqueNetId(), SessionName, SessionSearch->SearchResults[0]);

			return;
		}
	}
	else
		CLog::Print(__FUNCTION__, -1, 10.f, FColor::Red);

}

void UUW_LobbySession::OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteHandle);

			if (Result == EOnJoinSessionCompleteResult::Success)
			{
				FString ConnectString;
				if (Sessions->GetResolvedConnectString(SessionName, ConnectString) && GetOwningPlayer())
				{
					UE_LOG_ONLINE_SESSION(Log, TEXT("Join session: traveling to %s"), *ConnectString);
					GetOwningPlayer()->ClientTravel(ConnectString, TRAVEL_Absolute);
					CLog::Print("Success", -1, 10.f, FColor::Green);
				}
			}

		}
	}
}