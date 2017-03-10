#pragma once
#include <QtWebSockets/QtWebSockets>

/**
* Callback interface for messages delivered on WebSocket.
* All events are dispatched from a looper executor thread.
*/
struct WebSocketChannelEvents {
	virtual void onWebSocketMessage(std::string message) = 0;
	virtual void onWebSocketClose() = 0;
	virtual void onWebSocketError(std::string description) = 0;
};

class WebSocketChannelClient:public QObject
{
	const int CLOSE_TIMEOUT = 1000;
	
	/**
	* Possible WebSocket connection states.
	*/

	enum WebSocketConnectionState { WSS_NEW, WSS_CONNECTED, WSS_REGISTERED, WSS_CLOSED, WSS_ERROR };
	
	QString m_wsServerUrl;
	QString m_postServerUrl;
	QString m_roomID;
	QString m_clientID;

	WebSocketConnectionState m_state;
	bool m_closeEvent;
	// WebSocket send queue. Messages are added to the queue when WebSocket
	// client is not registered and are consumed in register() call.
	std::list<QString> m_wsSendQueue;

	QWebSocket m_webSocket;
	WebSocketChannelEvents &m_channelEvents;
		
public:
	WebSocketChannelClient(WebSocketChannelEvents &event);
	~WebSocketChannelClient();
	void wss_connect(const std::string& wsUrl, const std::string& postUrl);
	void wss_register(const std::string& roomID, const std::string& clientID);
	void wss_send_message(QString message);
	void wss_post_message(QString message);

private Q_SLOTS:
	void onConnected();
	void onTextMessageReceived(QString message);
	void onDisconnect();

};

