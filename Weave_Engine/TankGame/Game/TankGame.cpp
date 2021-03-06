#include "pch.h"
#include "TankGame.h"

using namespace Tanks;

TankGame::TankGame( HINSTANCE hInstance )
    : Game( hInstance )
{
}

TankGame::~TankGame()
{
    // Release network manager instance
    ClientNetworkManager::ReleaseInstance();
}

void TankGame::Init()
{
    Game::Init();

    PlayerMoves::Init();

    sceneManager->LoadScene( MainMenuSceneName );
}

void TankGame::Update( float deltaTime, float totalTime )
{
    Game::Update( deltaTime, totalTime );

    switch ( GameState )
    {
    case Tanks::EGameState::Playing:
    {
        NetMan->ProcessIncomingPackets();
        NetMan->SendOutgoingPackets( totalTime );
        PlayerMoves::Instance->Update( deltaTime );
    }
    break;
    default:
        break;
    }
}

void TankGame::Quit()
{
    GameState = EGameState::Quitting;

    // Tell the server that we quit

    Game::Quit();
}

void Tanks::TankGame::DrawUI()
{
    Game::DrawUI();

    switch ( GameState )
    {
    case Tanks::EGameState::MainMenu:
        DrawMainMenu();
        break;
    case Tanks::EGameState::Playing:
        DrawGameUI();
        break;
    case Tanks::EGameState::Quitting:
        break;
    default:
        break;
    }

    // Draw client state
    {
        ImGui::Begin( "Client State" );
        static std::string stateText = "Uninitalized";
        if ( NetMan != nullptr )
        {
            switch ( NetMan->GetClientState() )
            {
            case ClientNetworkManager::EClientState::Uninitalized:
                stateText = "Uninitalized";
                break;
            case ClientNetworkManager::EClientState::SayingHello:
                stateText = "Saying Hello...";
                break;
            case ClientNetworkManager::EClientState::Welcomed:
                stateText = "Welcomed!";
                break;
            default:
                break;
            }
        }

        ImGui::Text( "Client State: %s", stateText.c_str() );

        ImGui::End();
    }

}

void Tanks::TankGame::DrawMainMenu()
{
    ImGui::Begin( "Main Menu" );

    static char namebuf[ 64 ] = "Player 1\0";
    static char serverAddressBuf[ 64 ] = "127.0.0.1\0";
    static unsigned short serverPortBuf = 50001;
    ImGui::InputText( "Player Name", namebuf, IM_ARRAYSIZE( namebuf ) );
    ImGui::InputText( "Address", serverAddressBuf, IM_ARRAYSIZE( serverAddressBuf ) );
    ImGui::InputInt( "Port", ( int* ) &serverPortBuf );

    if ( ImGui::Button( "Connect", ImVec2( ImGui::GetWindowWidth(), 0.f ) ) )
    {
        // Attempt to connect to the server
        // The io_service object is what the network manager uses for async operations
        io_service.reset();
        io_service = std::make_shared< boost::asio::io_service >();
        NetMan = ClientNetworkManager::StaticInit( io_service, serverAddressBuf, serverPortBuf, namebuf );

        // Load the game scene file locally and set our state to playing
        sceneManager->LoadScene( GameSceneName );
        GameState = EGameState::Playing;
    }

    ImGui::Separator();

    if ( ImGui::Button( "Quit", ImVec2( ImGui::GetWindowWidth(), 0.f ) ) )
    {
        Quit();
    }
    ImGui::End();
}

void Tanks::TankGame::DrawGameUI()
{
    ImGui::Begin( "Game Menu" );

    ImGui::Text( "Currently Connected to:\n %s : %d",
        NetMan->GetServerEndpoint().address().to_string().c_str(),
        NetMan->GetServerEndpoint().port()
    );

    ImGui::Text( "Connected Players: %d", NetMan->GetNumConnectedPlayers() );

    if ( ImGui::Button( "Disconnect", ImVec2( ImGui::GetWindowWidth(), 0.f ) ) )
    {
        Disconnect();
    }

    ImGui::End();
}

void Tanks::TankGame::Disconnect()
{
    // Go back to the main menu when we disconnect
    GameState = EGameState::MainMenu;
    // Reset our network manager
    ClientNetworkManager::ReleaseInstance();

    // Reset the scene (clears the known replicated objects)
    sceneManager->GetActiveScene()->ResetScene();
    sceneManager->LoadScene( MainMenuSceneName );

    LOG_TRACE( "This client has disconnect from the server!" );
}