#ifndef WIFI_HOST_CONFIG_H_
#define WIFI_HOST_CONFIG_H_

#define MY_HOSTNAME "Bird"
#define USE_STATIC_IP 1

#if USE_STATIC_IP
IPAddress staticIP(192,168,2,10);
//IPAddress gateway(192,168,0,255);
IPAddress gateway(192,168,2,255);
IPAddress subnet(255,255,255,0);
#endif


#endif  // WIFI_HOST_CONFIG_H_
