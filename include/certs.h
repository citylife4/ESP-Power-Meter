#ifndef CERTS
#define CONFIG

//https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/BearSSL_Server/BearSSL_Server.ino

// The server's private key which must be kept secret
const char CA_CERT_PROG[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----
)EOF";

// The server's public certificate which must be shared
const char CLIENT_CERT_PROG[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----    
)EOF";

// The server's public certificate which must be shared
// const char client_key[] PROGMEM = R"EOF(
const char CLIENT_KEY_PROG[] PROGMEM = R"EOF(
-----BEGIN RSA PRIVATE KEY-----

-----END RSA PRIVATE KEY-----  
)EOF";

#endif