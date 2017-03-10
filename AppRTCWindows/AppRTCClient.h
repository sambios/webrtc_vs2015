#pragma once

#include <iostream>
#include <string>
#include <list>
#include <vector>

#include "webrtc/api/mediastreaminterface.h"
#include "webrtc/api/peerconnectioninterface.h"
#include "webrtc/examples/peerconnection/client/main_wnd.h"
#include "webrtc/examples/peerconnection/client/peer_connection_client.h"
#include "webrtc/base/scoped_ptr.h"

/**
* AppRTCClient is the interface representing an AppRTC client.
*/

/**
* Description of an RFC 4566 Session.
* SDPs are passed as serialized Strings in Java-land and are materialized
* to SessionDescriptionInterface as appropriate in the JNI layer.
*/
struct SessionDescription {	
	enum Type {
		OFFER,
		PRANSWER,
		ANSWER
	};

	Type type;
	std::string description;
	SessionDescription(){}

	SessionDescription(Type type, std::string& desc)
	{
		this->type = type;
		this->description = desc;
	}

	SessionDescription& operator=(const SessionDescription& o)
	{
		if (this == &o) return *this;

		this->type = o.type;
		this->description = o.description;
		return *this;
	}
};

class RoomConnectionParameters {
public:
	std::string roomUrl;
	std::string roomId;
	bool loopback;

	RoomConnectionParameters() {
		roomUrl = "https://webrtc.longzhu.com";
		roomId = 511211001;
		loopback = false;
	}

	RoomConnectionParameters(std::string& roomUrl, std::string& roomId, bool loopback) {
		roomUrl = roomUrl;
		roomId = roomId;
		loopback = loopback;
	}
};

/**
* Struct holding the signaling parameters of an AppRTC room.
*/
struct SignalingParameters {
	std::vector<webrtc::PeerConnectionInterface::IceServer> iceServers;
	bool initiator;
	std::string clientId;
	std::string wssUrl;
	std::string wssPostUrl;
	SessionDescription offerSdp;
	std::vector<webrtc::IceCandidateInterface*> iceCandidates;

	SignalingParameters(std::vector<webrtc::PeerConnectionInterface::IceServer>& iceServers, bool initiator,
		std::string& clientId, std::string& wssUrl, std::string& wssPostUrl, const SessionDescription& offerSdp,
		std::vector<webrtc::IceCandidateInterface*> iceCandidates) {
		this->iceServers = iceServers;
		this->initiator = initiator;
		this->clientId = clientId;
		this->wssUrl = wssUrl;
		this->wssPostUrl = wssPostUrl;
		this->offerSdp = offerSdp;
		this->iceCandidates = iceCandidates;
	}
};



/**
* Callback interface for messages delivered on signaling channel.
*
* <p>Methods are guaranteed to be invoked on the UI thread of |activity|.
*/
struct SignalingEvents {
	/**
	* Callback fired once the room's signaling parameters
	* SignalingParameters are extracted.
	*/
	virtual void onConnectedToRoom(const SignalingParameters& params) = 0;

	/**
	* Callback fired once remote SDP is received.
	*/
	virtual void onRemoteDescription(const SessionDescription& sdp) = 0;

	/**
	* Callback fired once remote Ice candidate is received.
	*/
	virtual void onRemoteIceCandidate(const webrtc::IceCandidateInterface& candidate) = 0;

	/**
	* Callback fired once remote Ice candidate removals are received.
	*/
	virtual void onRemoteIceCandidatesRemoved(const std::vector<webrtc::IceCandidateInterface*>& candidates) = 0;

	/**
	* Callback fired once channel is closed.
	*/
	virtual void onChannelClose() = 0;

	/**
	* Callback fired once channel error happened.
	*/
	virtual void onChannelError(std::string& description) = 0;
};

/**
* AppRTCClient is the interface representing an AppRTC client.
*/
struct AppRTCClient {
	
	/**
	* Asynchronously connect to an AppRTC room URL using supplied connection
	* parameters. Once connection is established onConnectedToRoom()
	* callback with room parameters is invoked.
	*/
	virtual void connectToRoom(const RoomConnectionParameters& connectionParameters) = 0;

	/**
	* Send offer SDP to the other participant.
	*/
	virtual void sendOfferSdp(const SessionDescription& sdp) = 0;

	/**
	* Send answer SDP to the other participant.
	*/
	virtual void sendAnswerSdp(const SessionDescription& sdp) = 0;

	/**
	* Send Ice candidate to the other participant.
	*/
	virtual void sendLocalIceCandidate(const webrtc::IceCandidateInterface& candidate) = 0;

	/**
	* Send removed ICE candidates to the other participant.
	*/
	virtual void sendLocalIceCandidateRemovals(std::vector<webrtc::IceCandidateInterface*>& candidates) = 0;

	/**
	* Disconnect from room.
	*/
	virtual void disconnectFromRoom() = 0;
		
};

