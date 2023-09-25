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

Use the command line tool included in the sslclient package
```
git clone https://github.com/OPEnSLab-OSU/SSLClient

python3 pycert_bearssl.py convert --no-search ca.crt
```

Create a file called NetToolsEnv.h and include the following code
```
// this file alerts netTools to use custom trust anchors

// 0 for dev, 1 for prod

#define __NET_TOOLS_ENV__ 1

```

Create 2 more files called NetToolsCertsDev.h and NetToolsCertsProd.h and include there the trust anchors generated with bearssl package
