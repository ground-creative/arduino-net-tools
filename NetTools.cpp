/**
	Net tools helper
	Author: Carlo Pietrobattista
	Version: 1.1
*/

#include <Arduino.h>
#include "NetTools.h"
#include <PubSubClient.h>

WiFiClient mqttClient;
PubSubClient client(mqttClient);

int mqtt_max_reconnect_attemps = 20;

int wifi_max_reconnect_attemps = 120;

NetTools::WIFI::WIFI(const char* ssid, const char* password)
{
	_ssid = ssid;
	_password = password;
}

void NetTools::WIFI::connect() 
{
	WiFi.mode(WIFI_STA);
	WiFi.begin(_ssid , _password);
	Serial.print("Connecting to WiFi network");
	Serial.println("");
	Serial.println(_ssid);
	unsigned int current_attempt = 1;
	while(WiFi.status() != WL_CONNECTED) 
	{
		if (current_attempt >= wifi_max_reconnect_attemps)
		{
			
			Serial.println("Unable to connect MQTT server, restarting chip to try again!");
			ESP.restart();
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
	if ( ( WiFi.status( ) != WL_CONNECTED ) && ( current_time - _previous_time >=interval ) )  
	{
		Serial.println( "WiFI is disconnected, reconnecting" );
		WiFi.disconnect( );
		WiFi.reconnect( );
		_previous_time = current_time;
	}
	/*if (WiFi.status() != WL_CONNECTED)  
	{
		Serial.println("WiFI is disconnected, tried to reconnect too many times. Restarting chip");
		ESP.restart();
	}*/
}

NetTools::MQTT::MQTT(const char* server, std::function<void( char*, byte*, unsigned int )> callback, int port )
{
	_server = server;
	_port = port;
	client.setServer( _server, _port ).setCallback( callback );
}

PubSubClient NetTools::MQTT::getClient()
{
	return client;
}

boolean NetTools::MQTT::isConnected()
{
	return client.connected();
}

int NetTools::MQTT::connect(String mqttClientID, const char* username, const char* password, int interval)
{
	_clientID = mqttClientID;
	unsigned int current_attempt = 1;
	String clientID = _clientID + String( "-" + WiFi.macAddress( ) );
	while ( !client.connected( ) && WiFi.status( ) == WL_CONNECTED ) 
	{
		Serial.print( "Attempting MQTT connection..." );
		if ( client.connect( clientID.c_str( ), username, password , 
					String( "device-status/" + _clientID ).c_str( ), 1, true, "offline" ) ) 
		{
			Serial.println( "connected" );
			client.publish( String( "device-status/" + _clientID ).c_str( ), "online", true );
			return true;
		}
		/*else if ( current_attempt >= mqtt_max_reconnect_attemps )
		{
			
			Serial.println( "Unable to connect MQTT server, restarting chip to try again!" );
			ESP.restart( );
		}*/
		else 
		{
			Serial.print( "attempt ");
			Serial.print( current_attempt );
			Serial.print( " failed, rc=" );
			Serial.print( client.state( ) );
			Serial.print( " trying again in " );
			Serial.print( ( interval / 1000 ) );
			Serial.print( " seconds" );
			Serial.println( "" );
			current_attempt++;
			delay( interval );
		}
	}
	return false;
}

void NetTools::MQTT::publish( char* topic, char* value )
{
	Serial.println( "Publishing MQTT data" );
	Serial.print( "Topic: " );
	Serial.print( topic );
	Serial.print( " | Value: " );
	Serial.print( value );
	Serial.println( "" );
	client.publish( topic, value );
}

void NetTools::MQTT::subscribe( char* topic )
{
	Serial.print( "Subscribing to topic: " );
	Serial.print( topic );
	Serial.println( "" );
	client.subscribe( topic );
}

void NetTools::MQTT::loop( )
{
	client.loop( );
}