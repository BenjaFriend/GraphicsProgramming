#pragma once

#include <iostream>
#include <thread>
#include <memory>
#include <unordered_map>
#include <boost/asio.hpp>

#include "ServerNetworkManager.h"

#include "ServerUtils.h"
#include "Config.h"
#include "Room.h"
#include "Timing.h"

/// <summary>
/// The core server logic that will handle the creation of rooms and 
/// manage data
/// </summary>
/// <author>Ben Hoffman</author>
class WeaveServer
{
public:

    WeaveServer( SERVER_INIT_DESC aDesc );

    /** Remove the copy ctor and operator */
    WeaveServer( const WeaveServer & ) = delete;
    WeaveServer & operator=( const WeaveServer & ) = delete;

    ~WeaveServer();

    /// <summary>
    /// Start running the server, this is a blocking function.
    /// </summary>
    void Run();

    /// <summary>
    /// Signal the server that we want to shut down
    /// </summary>
    void Shutdown();

private:

    /// <summary>
    /// Process any console input from the user, such as quit or help. 
    /// </summary>
    void ProcessConsoleInput();

    /** The network manager for this server */
    ServerNetworkManager* NetworkMan = nullptr;

    /** The max number of rooms that this server can have */
    unsigned int MaxRooms = 4;

    /** The default port to listen to data from the clients on  */
    unsigned short ListenPort = DEF_LISTEN_PORT;

    /** The default port to send data back to the clients on  */
    unsigned short ResponsePort = DEF_RESPONSE_PORT;

    /** Pointer to the room objects */
    Room* Rooms = nullptr;

    /// <summary>
    /// Command options for the console input
    /// </summary>
    enum class EUserCoptions : uint8_t
    {
        QUIT,
        HELP
    };

    std::unordered_map<std::string, EUserCoptions> UserOptions;

    /** A flag for if the user has entered the QUIT command */
    std::atomic<bool> ShouldQuit;

    /** The thread the user input will be checked on */
    std::thread userInputThread;

    /** Pointer to the io_service for the network manager 
    * This has to be here because otherwise there is an exception on exit
    * in release mode when the netMan io_service goes out of context
    */
    std::shared_ptr < boost::asio::io_service > io_service;

    /** Delta time of the server */
    float DeltaTime;

    /** Total time that the program has been running */
    float TotalTime;

    /** Time between input updates */
    float TimeOfLastStateUpdate;
    /** The amount of time between sending hello packets */
    const float TimeBetweenStateUpdates = 0.033f;

};