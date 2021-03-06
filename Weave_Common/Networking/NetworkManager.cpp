#include "stdafx.h"

#include "NetworkManager.h"

namespace bai = boost::asio::ip;

NetworkManager::NetworkManager( std::shared_ptr< boost::asio::io_service > aServce )
{
    // Create an IO service
    //io_service = std::make_shared< boost::asio::io_service >();
    io_service = aServce;
}

NetworkManager::~NetworkManager()
{
    io_service->stop();
    IsDone = true;

    if ( runningThread.joinable() )
    {
        runningThread.join();
    }

    LOG_TRACE( " --- NetworkManager DTOR --- " );
}

bool NetworkManager::Init( UINT16 aPort )
{
    try
    {
        ListenSocket = UDPSocketFactory(
            *io_service,
            boost::asio::ip::udp::endpoint( bai::udp::v4(), aPort )
        );

        LOG_TRACE( "Initalize Network manager at port {}", aPort );

        // Start the running thread
        runningThread = std::thread( &NetworkManager::Run, this );

        return true;
    }
    catch ( const std::exception& e )
    {
        ( void ) ( e );
        LOG_ERROR( "Error initalizing NetworkManager: {}", e.what() );
        return false;
    }
   
    // Set non-blocking mode (if not using boost)

    return true;
}

void NetworkManager::ProcessIncomingPackets()
{
    // If we were not using Boost asio, then we would do a recvfrom socket call here

    ProcessQueuedPackets();


}

void NetworkManager::SendPacket( const OutputMemoryBitStream & inOutputStream, const bai::udp::endpoint & inFromAddress )
{
    // Send the output stream to the given endpoint
    // returns the number of bytes send
    try 
    {
        ListenSocket->send_to(
            boost::asio::buffer( inOutputStream.GetBufferPtr(), inOutputStream.GetBitLength() ),
            inFromAddress
        );

        // #TODO Keep track of the number of bytes sent this frame
    }
    catch ( const std::exception& e )
    {
        ( void ) ( e );
        LOG_ERROR( "Error sending packet : {}", e.what() );
    }
}

void NetworkManager::Run()
{
    StartRecieve();

    while ( !IsDone || !io_service->stopped() )
    {
        // Run said server
        try
        {
            io_service->run();
        }
        catch ( const std::exception& e )
        {
            ( void ) ( e );
            LOG_ERROR( "Server: Network exception: {}", e.what() );
        }
        catch ( ... )
        {
            LOG_ERROR( "Server: Network exception: UNKNOWN" );
        }
    }
}

void NetworkManager::ProcessQueuedPackets()
{
    while ( !PacketQueue.empty() )
    {
        ReceivedPacket& nextPacket = PacketQueue.front();
        // #TODO Only process this packet if it was received later than the last one
        ProcessPacket( nextPacket.GetPacketBuffer(), nextPacket.GetEndpoint() );
        PacketQueue.pop();
    }
}

void NetworkManager::StartRecieve()
{
    // Start listening ASYNCRONOUSLY for incoming data
    // Set HandleRemoteRecieved to be called when there is data received
    // The endpoint information will be stored in 'remote_endpoint'
    try
    {
        ListenSocket->async_receive_from(
            boost::asio::buffer( recv_buf, DEF_BUF_SIZE ),
            remote_endpoint,
            boost::bind( &NetworkManager::HandleRemoteRecieved, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred )
        );
    }
    catch ( const std::exception& e )
    {
        ( void ) ( e );
        LOG_ERROR( "Error in StartRecieve: {}", e.what() );
    }
}

void NetworkManager::HandleRemoteRecieved( const std::error_code & error, std::size_t msgSize )
{
    // We have received some data, let's process it. 
    // The data is in the 'recv_buf' currently
    if ( !error )
    {
        // Put the data into an input stream so that we can manage it
        int packetSize = sizeof( recv_buf );
        InputMemoryBitStream inputStream( recv_buf, packetSize * 8 );

        float receivedTime = Timing::sInstance.GetTimef();

        // Put this packet memory into the buffer
        ReceivedPacket packet( receivedTime, inputStream, remote_endpoint );
        PacketQueue.emplace( packet );

        // Start another async request
        StartRecieve();
    }
    else
    {
        LOG_ERROR( "Network Man: Message error! {} ", error.message() );
    }
}

NetworkManager::ReceivedPacket::ReceivedPacket( float inRecievedtime, InputMemoryBitStream & inStream, boost::asio::ip::udp::endpoint & aInEndpoint ) :
    Recievedtime( inRecievedtime ),
    PacketBuffer( inStream ),
    InEndpoint( aInEndpoint )
{
}