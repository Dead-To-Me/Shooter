// Shooter27


#include "SGameSession.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"

void ASGameSession::RegisterServer() 
{
    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (OnlineSubsystem == nullptr) {
        return;
    }
    IOnlineSessionPtr Session = OnlineSubsystem->GetSessionInterface();
    FOnlineSessionSettings Settings;

    Settings.bIsDedicated = true;
    Settings.bAllowInvites = true;
    Settings.bAllowJoinInProgress = true;
    Settings.bIsLANMatch = false;
    Settings.bShouldAdvertise = true;
    Settings.bUsesPresence = true;
    Settings.bAllowJoinViaPresence = true;
    Settings.NumPublicConnections = 32;

    bool bSessionCreated = Session->CreateSession(0, NAME_GameSession, Settings);
    UE_LOG(LogTemp, Log, TEXT("Session Started: %d =================="), bSessionCreated);
}
