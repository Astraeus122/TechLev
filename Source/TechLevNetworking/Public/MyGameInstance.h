#pragma once
#define SETTING_SERVERNAME FName("SERVER_NAME")

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemTypes.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MyGameInstance.generated.h"

UCLASS()
class TECHLEVNETWORKING_API UMyGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UMyGameInstance();

    virtual void Init() override;

    void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

    UFUNCTION(BlueprintCallable)
    void CreateSession();

    UFUNCTION(BlueprintCallable)
    void FindSessions();

    UFUNCTION(BlueprintCallable)
    void JoinSession(int32 SessionIndex);

    UFUNCTION(BlueprintCallable)
    TArray<FString> GetServerNames() const;

    UFUNCTION(BlueprintCallable)
    void DestroySession();

    UFUNCTION(BlueprintCallable)
    void SetServerName(const FString& InServerName);

private:
    IOnlineSessionPtr SessionInterface;
    TSharedPtr<FOnlineSessionSearch> SessionSearch;

    void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
    void OnFindSessionsComplete(bool bWasSuccessful);
    void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

    TArray<FString> ServerNames;
    TArray<FOnlineSessionSearchResult> SearchResults;

    FString CustomServerName; 
};
