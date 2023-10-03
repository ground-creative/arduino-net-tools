/**
	Net tools helper
	Author: Ground Creative
*/

#define _VERSION_ 1.1.1
#include "NetTools.h"

WiFiClientSecure mqttWiFiClientSecure;
PubSubClient wifiClientSecure(mqttWiFiClientSecure);
WiFiClient mqttWiFiClient;
PubSubClient wifiClient(mqttWiFiClient);
EthernetClient mqttEthClient;
PubSubClient ethClient(mqttEthClient);
SSLClient ethClientSSL(mqttEthClient, TAs, (size_t)TAs_NUM, A5, SSLClient::SSL_DUMP);
PubSubClient ethClientSecure(ethClientSSL);

//int mqtt_max_reconnect_attemps = 20;

int wifi_max_reconnect_attemps = 120;

int wifi_pause_time = 10000;

NetTools::WIFI::WIFI(const char* ssid, const char* password)
{
	_ssid = ssid;
	_password = password;
}

void NetTools::WIFI::connect() 
{
	WiFi.mode(WIFI_STA);
	WiFi.begin(_ssid, _password);
	Serial.print("Connecting to WiFi network");
	Serial.println("");
	Serial.println(_ssid);
	unsigned int current_attempt = 1;
	while (WiFi.status() != WL_CONNECTED) 
	{
		if (Ethernet.hardwareStatus() != EthernetNoHardware && Ethernet.linkStatus() == LinkON)
		{
			Serial.println("Ethernet cable has been connected.");
			ESP.restart();
		}
		if (current_attempt >= wifi_max_reconnect_attemps)
		{
			//Serial.println("Unable to connect WiFi network, restarting chip to try again!");
			//ESP.restart();
			Serial.println("Unable to connect WiFi network, switching radio off and on!");
			WiFi.mode(WIFI_MODE_NULL);
			delay(wifi_pause_time);
			WiFi.mode(WIFI_STA);
			WiFi.begin(_ssid, _password);
			current_attempt = 1;
		}
		else 
		{
			Serial.print('.');
			delay(1000);
			current_attempt++;
		}
	}
	Serial.println("");
	Serial.println("WiFi connected!");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
}

IPAddress NetTools::WIFI::localAddress()
{
	return WiFi.localIP();
}

uint8_t NetTools::WIFI::status()
{
	return WiFi.status();
}

void NetTools::WIFI::check(int interval)
{
	unsigned long current_time = millis();
	if ((WiFi.status() != WL_CONNECTED) && (current_time - _previous_time >=interval))  
	{
		Serial.println("WiFI is disconnected, reconnecting");
		WiFi.disconnect();
		WiFi.reconnect();
		_previous_time = current_time;
	}
	/*if (WiFi.status() != WL_CONNECTED)  
	{
		Serial.println("WiFI is disconnected, tried to reconnect too many times. Restarting chip");
		ESP.restart();
	}*/
}

bool NetTools::WIFI::softAP(const char* ssid, const char* passphrase, int channel, int ssid_hidden, int max_connection, bool ftm_responder)
{
	return WiFi.softAP(ssid, passphrase, channel, ssid_hidden, max_connection, ftm_responder);
}

WiFiAPClass NetTools::WIFI::getSoftAP()
{
	return WiFiAPClass();
}

WiFiClass NetTools::WIFI::getObject()
{
	return WiFi;
}
NetTools::MQTT::MQTT(String clientType)
{
	//_client = (clientType == "ethernet") ? ethClient : wifiClient;
	_clientType = clientType;
	if (clientType == "ethernet"){ _client = ethClient; }
	else if (clientType == "wifi"){ _client = wifiClient; }
	else if (clientType == "wifiSecure"){ _client = wifiClientSecure; }
	else if (clientType == "ethernetSecure"){ _client = ethClientSecure; }
}

NetTools::MQTT::MQTT(const char* server, std::function<void(char*, byte*, unsigned int)> callback, int port)
{
	_server = server;
	_port = port;
	//client.setServer(_server, _port).setCallback(callback);
}

void NetTools::MQTT::setCert(const char *cert)
{
	mqttWiFiClientSecure.setCACert(cert);
}

void NetTools::MQTT::setServer(const char* server, std::function<void(char*, byte*, unsigned int)> callback, int port)
{
	_server = server;
	_port = port;
	_client.setServer(_server, _port).setCallback(callback);
}

PubSubClient& NetTools::MQTT::getClient()
{
	return _client;
}

boolean NetTools::MQTT::isConnected()
{
	return _client.connected();
}

void NetTools::MQTT::disconnect()
{
	Serial.print("Disconnecting from MQTT server ");
	Serial.print(String(_server) + ":" + String(_port) + " ");
	_client.disconnect();
}

void NetTools::MQTT::setSocketTimeout(int timeout)
{
	Serial.print("Setting mqtt socket timeout to ");
	Serial.println(timeout);
	_client.setSocketTimeout(timeout);
}

void NetTools::MQTT::setKeepAlive(int value)
{
	Serial.print("Setting mqtt keep alive value ");
	Serial.println(value);
	_client.setKeepAlive(value);
}

void NetTools::MQTT::setBufferSize(int value)
{
	Serial.print("Setting mqtt buffer size to ");
	Serial.println(value);
	_client.setBufferSize(value);
}

int NetTools::MQTT::connect(String mqttClientID, const char* username, const char* password, int interval)
{
	_clientID = mqttClientID;
	unsigned int current_attempt = 1;
	String clientID = _clientID + String("-" + WiFi.macAddress());
	while (!_client.connected() && (WiFi.status() == WL_CONNECTED || Ethernet.linkStatus() == LinkON)) 
	{
		if (WiFi.status() == WL_CONNECTED && Ethernet.linkStatus() == LinkON)
		{
			Serial.println("Ethernet cable has been connected.");
			ESP.restart();	
		}
		Serial.print("Attempting MQTT connection with server ");
		Serial.print(String(_server) + ":" + String(_port) + " ");
		if (_client.connect(clientID.c_str(), username, password , 
			String("device-status/" + _clientID).c_str(), 1, true, "offline")) 
		{
			Serial.println("connected!");
			_client.publish(String("device-status/" + _clientID).c_str(), "online", true);
			return true;
		}
		/*else if (current_attempt >= mqtt_max_reconnect_attemps)
		{
			
			Serial.println("Unable to connect MQTT server, restarting chip to try again!");
			ESP.restart();
		}*/
		else 
		{
			Serial.print("attempt ");
			Serial.print(current_attempt);
			Serial.print(" failed, rc=");
			Serial.print(_client.state());
			Serial.print(" trying again in ");
			Serial.print((interval/1000));
			Serial.print(" seconds");
			Serial.println("");
			current_attempt++;
			delay(interval);
		}
	}
	return false;
}

void NetTools::MQTT::publish(char* topic, char* value)
{
	Serial.println("Publishing MQTT data");
	Serial.print("Topic: "); Serial.print(topic); Serial.print(" | Value: "); Serial.print(value); Serial.println("");
	Serial.println("");
	_client.publish(topic, value);
	/*if (_clientType == "ethernetSecure")
	{
		ethClientSSL.flush();
	}*/
}

void NetTools::MQTT::subscribe(char* topic)
{
	Serial.print("Subscribing to topic: ");
	Serial.print(topic);
	Serial.println("");
	_client.subscribe(topic);
}

void NetTools::MQTT::loop()
{
	_client.loop();
}