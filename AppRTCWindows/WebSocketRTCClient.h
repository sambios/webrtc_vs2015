#pragma once

#include "AppRTCClient.h"
#include "WebSocketChannelClient.h"

class WebSocketRTCClient:public AppRTCClient
{
	const std::string TAG = "WSRTCClient";
	const std::string ROOM_JOIN = "join";
	const std::string ROOM_MESSAGE = "message";
	const std::string ROOM_LEAVE = "leave";

	enum ConnectionState { ConnectState_NEW, ConnectState_CONNECTED, ConnectState_CLOSED, ConnectState_ERROR };

	enum MessageType { MESSAGE, LEAVE };
		
	boolean m_initiator;
	SignalingEvents &m_signalingEvents;
	WebSocketChannelClient* m_wsClient;
	ConnectionState m_roomState;
	RoomConnectionParameters m_connectionParameters;
	std::string m_messageUrl;
	std::string m_leaveUrl;

public:
	WebSocketRTCClient(SignalingEvents &event);
	~WebSocketRTCClient();


	// Implement AppRTCClient Interfaces
	




};

