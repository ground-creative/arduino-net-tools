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
## Creating and including trust anchors for ssl

Use the command line tool included in bearssl package
```
git clone https://www.bearssl.org/git/BearSSL
```

Create 2 files called NetToolsEnvDev.h and NetToolsCertsProd.h <br>
Include there the trust anchors generated with bearssl package
