#ifndef _NET_TOOLS_H
#define _NET_TOOLS_H
#include <Arduino.h>
#ifdef ESP32
	#include <WiFi.h>
#else
	#include <ESP8266WiFi.h>
#endif
#include <SPI.h>
#include <Ethernet.h>
#include "WiFiClientSecure.h"
#include <SSLClient.h> 
#include <PubSubClient.h>
#if defined __has_include
	#if __has_include ("NetToolsEnvDev.h")
		#include "NetToolsCertsDev.h"
	#elif __has_include ("NetToolsEnvProd.h")
		#include "NetToolsCertsProd.h"
	#else
		#include "trust_anchors.h"
	#endif;
#else
	#include "trust_anchors.h"
#endif;

namespace NetTools
{
	class WIFI
	{
		public:
			WIFI(const char* ssid = NULL, const char* password = NULL);
			void connect();
			IPAddress localAddress();
			void check(int interval = 20000);
			uint8_t status();
			bool softAP(const char* ssid, const char* passphrase = NULL, int channel = 1, int ssid_hidden = 0, int max_connection = 4, bool ftm_responder = false);
			bool softAP(const String& ssid, const String& passphrase = emptyString, int channel = 1, int ssid_hidden = 0, int max_connection = 4, bool ftm_responder = false) 
			{
				return WiFi.softAP(ssid.c_str(), passphrase.c_str(), channel, ssid_hidden, max_connection, ftm_responder);
			};
			WiFiAPClass getSoftAP();
			WiFiClass getObject();
		private:
			const char* _ssid;
			const char* _password;
			unsigned long _previous_time = 0;
	};
	
	class MQTT
	{
		public:
			MQTT(String clientType = "wifi");
			MQTT(const char* server, std::function<void(char*, byte*, unsigned int)> callback = NULL, int port = 1883);
			int connect(String mqttClientID, const char* username = NULL, const char* password = NULL, int interval = 5000);
			void publish(char* topic, char* value);
			void subscribe(char* topic);
			void setCert(const char* cert);
			void loop();
			void setServer(const char* server, std::function<void(char*, byte*, unsigned int)> callback = NULL, int port = 1883);
			void disconnect();
			PubSubClient getClient();
			boolean isConnected();
		private:
			const char* _server;
			const char* _username;
			const char* _password;
			PubSubClient _client;
			String _clientID;
			int _port = 1883;
	};
}
#endif