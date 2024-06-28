#include "MyGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystemTypes.h"
#include "Interfaces/OnlineSessionInterface.h"
#include <Kismet/GameplayStatics.h>

#define SETTING_SERVERNAME FName("SERVER_NAME")

UMyGameInstance::UMyGameInstance()
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (Subsystem)
    {
        SessionInterface = Subsystem->GetSessionInterface();
    }
}

void UMyGameInstance::Init()
{
    Super::Init();

    if (SessionInterface.IsValid())
    {
        SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMyGameInstance::OnDestroySessionComplete);
    }
}

void UMyGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        UE_LOG(LogTemp, Log, TEXT("Session destroyed successfully: %s"), *SessionName.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to destroy session: %s"), *SessionName.ToString());
    }
}

void UMyGameInstance::SetServerName(const FString& InServerName)
{
    CustomServerName = InServerName;
}

void UMyGameInstance::CreateSession()
{
    if (SessionInterface.IsValid())
    {
        FName SessionName = FName(*CustomServerName);  // Use custom server name

        // Destroy any existing session
        FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
        if (ExistingSession)
        {
            SessionInterface->DestroySession(NAME_GameSession);
        }

        // Create a new session after destroying the old one
        FOnlineSessionSettings SessionSettings;
        SessionSettings.bIsLANMatch = true;
        SessionSettings.NumPublicConnections = 4;
        SessionSettings.bAllowJoinInProgress = true;
        SessionSettings.bShouldAdvertise = true;
        SessionSettings.bUsesPresence = true;
        SessionSettings.Set(SETTING_SERVERNAME, CustomServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

        SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMyGameInstance::OnCreateSessionComplete);
        SessionInterface->CreateSession(0, SessionName, SessionSettings);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("SessionInterface is not valid!"));
    }
}

void UMyGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        UE_LOG(LogTemp, Log, TEXT("Session created successfully: %s"), *SessionName.ToString());
        // Open the level for the session host with ?listen
        UE_LOG(LogTemp, Log, TEXT("Opening level as listen server"));
        UGameplayStatics::OpenLevel(GetWorld(), "ThirdPersonMap?listen", true, "listen");
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to create session: %s"), *SessionName.ToString());
    }
}

void UMyGameInstance::FindSessions()
{
    if (SessionInterface.IsValid())
    {
        SessionSearch = MakeShareable(new FOnlineSessionSearch());
        SessionSearch->bIsLanQuery = true;
        SessionSearch->MaxSearchResults = 20;
        SessionSearch->PingBucketSize = 50;

        SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMyGameInstance::OnFindSessionsComplete);
        SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("SessionInterface is not valid!"));
    }
}

void UMyGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
    ServerNames.Empty();
    SearchResults.Empty();

    if (bWasSuccessful && SessionSearch.IsValid())
    {
        UE_LOG(LogTemp, Log, TEXT("Found sessions: %d"), SessionSearch->SearchResults.Num());
        for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
        {
            if (SearchResult.IsValid() && !SearchResult.Session.SessionSettings.bIsDedicated) // Ensure the session result is valid and not dedicated
            {
                FString ServerName;
                if (SearchResult.Session.SessionSettings.Get(SETTING_SERVERNAME, ServerName) && !ServerName.IsEmpty()) // Retrieve and check custom server name
                {
                    ServerNames.Add(ServerName);
                    SearchResults.Add(SearchResult);
                    UE_LOG(LogTemp, Log, TEXT("Found session: %s, Ping: %d"), *ServerName, SearchResult.PingInMs);
                }
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to find sessions."));
    }
}

void UMyGameInstance::JoinSession(int32 SessionIndex)
{
    if (SessionInterface.IsValid() && SearchResults.IsValidIndex(SessionIndex))
    {
        // Check if a session already exists
        FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
        if (ExistingSession)
        {
            SessionInterface->DestroySession(NAME_GameSession);
            UE_LOG(LogTemp, Log, TEXT("Existing session destroyed before joining a new one"));
        }

        SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMyGameInstance::OnJoinSessionComplete);
        bool bSuccess = SessionInterface->JoinSession(0, NAME_GameSession, SearchResults[SessionIndex]);

        if (!bSuccess)
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to join session."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("SessionInterface is not valid or SessionIndex is out of range!"));
    }
}

void UMyGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    if (Result == EOnJoinSessionCompleteResult::Success)
    {
        UE_LOG(LogTemp, Log, TEXT("Successfully joined session: %s"), *SessionName.ToString());
        APlayerController* PlayerController = GetFirstLocalPlayerController();
        if (PlayerController)
        {
            FString ConnectString;
            if (SessionInterface->GetResolvedConnectString(SessionName, ConnectString))
            {
                UE_LOG(LogTemp, Log, TEXT("Resolved connect string: %s"), *ConnectString);

                // Ensure the map name is correct and the client can travel to it
                FString MapURL = ConnectString + "/Game/ThirdPerson/Maps/ThirdPersonMap";
                UE_LOG(LogTemp, Log, TEXT("Attempting to travel to: %s"), *MapURL);

                PlayerController->ClientTravel(MapURL, TRAVEL_Absolute);
                UE_LOG(LogTemp, Log, TEXT("ClientTravel called successfully"));
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Failed to get connect string."));
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("PlayerController is null."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to join session: %s, Result: %d"), *SessionName.ToString(), (int32)Result);
    }
}

void UMyGameInstance::DestroySession()
{
    if (SessionInterface.IsValid())
    {
        FName SessionName = NAME_GameSession;
        SessionInterface->DestroySession(SessionName);
    }
}

TArray<FString> UMyGameInstance::GetServerNames() const
{
    return ServerNames;
}
