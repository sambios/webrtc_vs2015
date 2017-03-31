#include "WebSocketChannelClient.h"
#include "webrtc/base/logging.h"
#include <QtCore>


WebSocketChannelClient::WebSocketChannelClient(WebSocketChannelEvents &event):m_channelEvents(event)
{
	m_state = WSS_NEW;
	connect(&m_webSocket, SIGNAL(connected()), this, SLOT(onConnected()));
	connect(&m_webSocket, SIGNAL(disconnected()), this, SLOT(onDisconnect()));
	
}


WebSocketChannelClient::~WebSocketChannelClient()
{
}

void WebSocketChannelClient::onConnected()
{
	connect(&m_webSocket, &QWebSocket::textMessageReceived,
		this, &WebSocketChannelClient::onTextMessageReceived);
}

void WebSocketChannelClient::onDisconnect()
{

}

void WebSocketChannelClient::onTextMessageReceived(QString message)
{

}

void WebSocketChannelClient::wss_connect(const std::string& wsUrl, const std::string& postUrl)
{	
	if (m_state != WSS_NEW) {
		LOG(LS_ERROR) << "WebSocket is already connected.";
		return;
	}

	m_wsServerUrl.fromStdString(wsUrl);
	m_postServerUrl.fromStdString(postUrl);
	m_closeEvent = false;

	LOG(LS_INFO) << "Connecting WebSocket to: " << wsUrl << ". Post URL: " << postUrl;
		
	m_webSocket.open(QUrl(m_wsServerUrl));

}

void WebSocketChannelClient::wss_register(const std::string& roomID, const std::string& clientID) 
{	
	m_roomID = roomID;
	m_clientID = clientID;

	if (m_state != WSS_CONNECTED) {
		LOG(LS_ERROR) << "WebSocket register() in state " << m_state;
		return;
	}
	LOG(LS_ERROR) << "Registering WebSocket for room " << roomID << ". ClientID: " << clientID;

	QJsonObject json;
	try {
		json.insert("cmd", "register");
		json.insert("roomid", QString::fromStdString(roomID));
		json.insert("clientid", QString::fromStdString(clientID));

		QJsonDocument json_doc(json);
		QString json_message = QString(json_doc.toJson());
		
		LOG(LS_INFO) << "C->WSS: " << json_message.toStdString();
		
		m_webSocket.sendTextMessage(json_message);

		m_state = WSS_REGISTERED;
		// Send any previously accumulated messages.
		for (QString& sendMessage : m_wsSendQueue) {
			send_message(sendMessage);
		}
		m_wsSendQueue.clear();
	}
	catch (...) {
		reportError("WebSocket register JSON error: " + e.getMessage());
	}
}


void WebSocketChannelClient::wss_send_message(QString message)
{
	
	switch (m_state) {
	case WSS_NEW:
	case WSS_CONNECTED:
		// Store outgoing messages and send them after websocket client
		// is registered.
		LOG(LS_INFO) << "WS ACC: " << message.toStdString();
		m_wsSendQueue.push_back(message);
		return;
	case WSS_ERROR:
	case WSS_CLOSED:
		LOG(LS_INFO) << "WebSocket send() in error or closed state : " << message.toStdString();
		return;
	case WSS_REGISTERED:
		/*JSONObject json = new JSONObject();
		try {
			json.put("cmd", "send");
			json.put("msg", message);
			message = json.toString();
			Log.d(TAG, "C->WSS: " + message);
			ws.sendTextMessage(message);
		}
		catch (JSONException e) {
			reportError("WebSocket send JSON error: " + e.getMessage());
		}*/
		break;
	}
}

void WebSocketChannelClient::wss_post_message(QString message)
{

}