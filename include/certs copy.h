#ifndef CERTS
#define CONFIG

//https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/BearSSL_Server/BearSSL_Server.ino

// The server's private key which must be kept secret
const char CA_CERT_PROG[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFtTCCA52gAwIBAgIUN5soqMYnDxw4Cw8uH7Ic/OT093cwDQYJKoZIhvcNAQEN
BQAwajEXMBUGA1UEAwwOQW4gTVFUVCBicm9rZXIxFjAUBgNVBAoMDU93blRyYWNr
cy5vcmcxFDASBgNVBAsMC2dlbmVyYXRlLUNBMSEwHwYJKoZIhvcNAQkBFhJub2Jv
ZHlAZXhhbXBsZS5uZXQwHhcNMjIwMTE3MjAyODUyWhcNMzIwMTE1MjAyODUyWjBq
MRcwFQYDVQQDDA5BbiBNUVRUIGJyb2tlcjEWMBQGA1UECgwNT3duVHJhY2tzLm9y
ZzEUMBIGA1UECwwLZ2VuZXJhdGUtQ0ExITAfBgkqhkiG9w0BCQEWEm5vYm9keUBl
eGFtcGxlLm5ldDCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAKvWhQgo
2c0RBAU9fZB6j/Skh+00t9mnAX0xQquGvuHPvQCEQRfLpBGUCKuAVHl3wtNjvnLE
johInOd8eaohYP+5Bi305fY4MypUoVbpNBkhO3UoOm3MGL+nUg2MN9Nu2d9s41DQ
arNmcj2wszMwMx94E1jUXsKXfG5uS9fxjF/wK9WYQLjUbz9JqonbTk/C0DwhoSkg
AO7qdb17WB++cvJxX7SsB5vuBuQx/KuVMt5IFrU3zYdyJI7jE7Bw23CrcLpwY4+P
6jkIXBIwrqG5C8twbrwNiNmFg2qLVZahq1vUSbVM+B5dBjB2ufnWxm3RS1CjrZRK
q6YBpHq4qJEwRGdCcD03zqQGXjTsY5InO2K/rDWvzVtoxaj5t4L/IS+IlJT3KmOV
+WOgQkwfG4EAqboHVnyat6k2yUQLySh11nbU61gO8ia3ll+/UJinpF9ubbz7Y6uz
HGfN9DcP3d9Z0Dtk/beDjGPyiaM2nk7/vbnuDtCNV+4YNWIltCjoxZqUYv6xj//2
cS3PNlBVoJ4U5UPP5ib3qlDtNY8+J3jS3mA+/SMk36CuOQYErjkgcfoQrN7lkbh8
O3LGArUU1Ah4fOm+jfsQ3aYDa8cU0YB4nKf8Cvl35EikWzOd9OAaOGXzK0AdTrA3
+tNskSiSRTqM1jNW6n9przxWpihritIfI/shAgMBAAGjUzBRMB0GA1UdDgQWBBSS
9aPUpLGKGDjCNlrRQ6HAz1eUpTAfBgNVHSMEGDAWgBSS9aPUpLGKGDjCNlrRQ6HA
z1eUpTAPBgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3DQEBDQUAA4ICAQBTKTpLX0Bz
FZf4Z4UHg2X1Z36Am0byHPX5LrtZAs0e2rD0wMMMDHyn/o0ERYZrGa5fE8a0qfNT
VJNKURkuf+Qx9DSA+p7rM0uIn/g73LGl2/2FbzrqDUqyosSv7n/MrTgc8YXMLPIQ
cEnfU79HuruccB/I/9GPmCXWUVNHIhz74m17UASV5KffkFA98Mx9FSTB2q/CE+ML
Ml2gsOHem/Y5Shhr0rxR6c9HMhG51T7L4o84CKZMcAO3gRNZOVdGNNXdYwnkjtN0
vueSwtORkIMLFIfpaEvfXg3ONLwQLXAnRf+PKsraHfIlHOSNwoXTrmoXNaOEhZ6T
RCpEKJfjnSGuhYSs0xrbsybjNpjhZnebY1VHlvwrQqCntDb+5lJqxnAUfhik9yOx
3IGc4G623FbJ94s1CHITR5EuNfbpbym37gXL6o/KhS2AvgVugH4sIAtbTdY86K0e
gSBU2rFf5jqG3TipIawhsfM/qzu6X34TwumczuODGlmiuRA5eP60YpBPiC2YhdLo
SyNlw9Q4CAXmVcofREVDBaZnTD9dHkLVq9DIm/1m0X1aVFiuToq30aP2veQQBmUR
mHxmE0cV7MEm5ktjDLQSuCUO2MtpxpqMcRv233etkAdJ+ZmqjnU7G6Y+xrTSnrhN
b4vSZ+z4cNtVGq9JF3RlNaTsVp5n0v+NTQ==
-----END CERTIFICATE-----
)EOF";

// The server's public certificate which must be shared
const char CLIENT_CERT_PROG[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
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
CBJC3chlXAdfOc6Lfaa+bQ==
-----END CERTIFICATE-----    
)EOF";

// The server's public certificate which must be shared
// const char client_key[] PROGMEM = R"EOF(
const char CLIENT_KEY_PROG[] PROGMEM = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
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
-----END RSA PRIVATE KEY-----  
)EOF";

#endif