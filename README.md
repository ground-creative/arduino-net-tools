# Arduino Network Tools

## Examples Wifi tools
```
// Init Wifi tools
NetTools::WIFI network(ssid, password);

// Connect to Wi-Fi
network.connect();


// Check Wifi Status
network.check();
```

## Examples mqtt tools
```
// Init mqtt tools
NetTools::MQTT mqtt(mqttClientID, mqtt_server, mqtt_callback);
  
// Connect or reconnect to mqtt
if(mqtt.connect(mqtt_username, mqtt_password))
{
  sendSerialData("info", "Connected to mqtt", "");
  // Subscribe to mqtt messages
  mqttSubscribe(roomID);
  sendSerialData("info", "Subscribed to mqtt", "");
  digitalWrite(MQTT_LED_PIN, HIGH);
  mqttConnected = true;
}
```
