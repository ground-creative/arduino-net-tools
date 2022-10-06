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
NetTools::MQTT mqtt(mqtt_server, mqtt_callback);
  
// Connect or reconnect to mqtt
if(mqtt.connect(mqtt_client_id, mqtt_username, mqtt_password))
{
  // do something
}
```
