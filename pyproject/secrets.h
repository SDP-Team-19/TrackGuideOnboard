//Your WiFi credentials
const char ssid[] = "Payload";
const char password[] =  "mitch2004";

// Below infomation you can set after signing up with u-blox Thingstream portal 
// and after add a new New PointPerfect Thing
// https://portal.thingstream.io/app/location-services/things
// in the new PointPerfect Thing you go to the credentials page and copy paste the values and certificate into this.  

// <Your PointPerfect Thing> -> Credentials -> Hostname
const char AWS_IOT_ENDPOINT[]       = "pp.services.u-blox.com";
const unsigned short AWS_IOT_PORT   = 8883;
// <Your PointPerfect Thing> -> Credentials -> IP key distribution topic
const char MQTT_TOPIC_KEY[]        = "/pp/ubx/0236/ip"; // This topic provides the IP only dynamic keys in UBX format
//const char MQTT_TOPIC_KEY[]        = "/pp/ubx/0236/Lb"; // This topic provides the L-Band + IP dynamic keys in UBX format
// <Your PointPerfect Thing> -> Credentials -> IP correction topic for EU/US region
const char MQTT_TOPIC_SPARTN[]     = "/pp/ip/us"; // This topic provides the SPARTN corrections for IP only: choice of {eu, us}
//const char MQTT_TOPIC_SPARTN[]     = "/pp/Lb/us"; // This topic provides the SPARTN corrections for L-Band and L-Band + IP: choice of {eu, us}
// <Your PointPerfect Thing> -> Credentials -> AssistNow (MGA) topic
const char MQTT_TOPIC_ASSISTNOW[]     = "/pp/ubx/mga";

// <Your PointPerfect Thing> -> Credentials -> Client Id
static const char MQTT_CLIENT_ID[] = "ebf26281-22c2-462c-9bd7-66f2a60cb136";

// <Your PointPerfect Thing> -> Credentials -> Amazon Root Certificate
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

// <Your PointPerfect Thing> -> Credentials -> Client Certificate
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWjCCAkKgAwIBAgIVANySeahMti6bDS7TR558KYswOkj+MA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yNDExMTkyMTIy
NDFaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCtvbwhCaXWKR9WKwTl
qdqK7YZHyF3ocB0P6ZlcZ+SO1ih9vBHaE3dLZ5w6d7jfbxl2xYHfUEIP+61TpE5k
Up4gCOoDYabbvXORxSGS95JihjS8eW10svYGp95M+CPyxjyKvImbmkLnIiwuDpfD
sAc0+UlC7BzPOGbPVkhe1yG3Eo0ZB8VwVpP3l30O6sNxaf76eEY4ICcbtPpvSVJz
DsbGvdY0++2lw12KfNDEzNhUoHrwFkDANEQjvPpPvgQUoaoobA9GeJiBdf3MXgRZ
/DykuuomW3rtN8nsORfhnedz7xgVimkemB62S0vod2r51VZvMMv01yH9XHjXdfZd
gHPVAgMBAAGjYDBeMB8GA1UdIwQYMBaAFFvhvFQd2bCC4Wxjo9jjnOMMaP7GMB0G
A1UdDgQWBBTwVLgTaaeMl5ruCKvUhknT8DWsPDAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEADxZF6zGfo0mgdW8EcrpUtrwJ
Wb/IY/opOUDN7w6QvlBRbJjeBnQJGzbw10DtJzo21bjDUaPNdyBdQQ2DolWNL4Gu
UNHecD/UMPQXoNCm7EcxXpq3jxl3h8xlgf+T2NwAqcfdYwopgTeQjgSd5sRWfbe+
Uv9dM4h323KiChT4s+MGruelSE55hjXYZRgixbsQSM3Sy1jl2qyGkT9JRJhilpdK
/gWtH2X5kDY1yEerBmj/YxjVTW2SdnVpjRNDbgfFowf9dq/SNnQc90Rqa02EiEtK
k8KiK6hZTm034EzSvQYDHXcqAz0bkyn218H9fX+sKxBAY5Dg+qjUJNp7A5ohww==
-----END CERTIFICATE-----
)KEY";

// Get this from Thingstream Portal 
// <Your PointPerfect Thing> -> Credentials -> Client Key
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEowIBAAKCAQEArb28IQml1ikfVisE5anaiu2GR8hd6HAdD+mZXGfkjtYofbwR
2hN3S2ecOne4328ZdsWB31BCD/utU6ROZFKeIAjqA2Gm271zkcUhkveSYoY0vHlt
dLL2BqfeTPgj8sY8iryJm5pC5yIsLg6Xw7AHNPlJQuwczzhmz1ZIXtchtxKNGQfF
cFaT95d9DurDcWn++nhGOCAnG7T6b0lScw7Gxr3WNPvtpcNdinzQxMzYVKB68BZA
wDREI7z6T74EFKGqKGwPRniYgXX9zF4EWfw8pLrqJlt67TfJ7DkX4Z3nc+8YFYpp
HpgetktL6Hdq+dVWbzDL9Nch/Vx413X2XYBz1QIDAQABAoIBAF+au442f+IUKOFC
tOQoDlFKmQyhDkPchbF+6tw1c0jCQe/GcNBey9NbhVWLEdkpioqX7npS8LdY16zi
Ch0cdVTNcvA/aREb9B9vqaT0xMGgdh6MdxWzcLyC3UnfdfCe8yuoydQtE8maPVNu
xkZxc9XLo2vG/6drsu8+3H4Lxcq164HfQU3YqwUPBWt0iw1IPfchioQscFo2xanw
xzKlK+zCwJw00UlyPM5Vexb7/wz6j9QD7S+ZauKqMk6qHtBdcjoC+C6PBYFdB608
79FoY6FxMDmy3pJt+ox5fMSNKZygez5qGk2Hu4SRu1bYpJxj57YbZLLedvFJ3Woj
ocK7ogECgYEA00cyYilS7bTDqW+i5buqkkRDkN0Eb7mXu+r4S3DgaDq0cZF33xWE
IkTaFiT9oBDMX3KNvLb0gJWitdlU8SdiL4grd15XxeJYxkXe9hkogoU76JAO4zpY
XsDGs9H7R4QMYC0MytIA/dafXP3I+bDrlZI+2wupOoNptV4Tz6u3LFUCgYEA0oR5
RiL10KgGq9EDr8hdkxd5woam5Yb2b8FiJf+WZYS2kCAmzhWOgDQM2CMmFhBLeSOm
wwiDfTE62cGMCeaff68hORCEr8dIJHMFL1vr2R36cMghnX03iOj11BXPxXDgDqtI
iU+bZseKmTXMw86Moc5kW+3HMdKa7S+e38EzqYECgYAQ7Mkqd+j+xdZOwnSoNqpp
YTKQbkQmtxVipmYRlQ9FCj7eQir6If0dyao2z+vJYM8ebkzQa0+W5WXNb4WQc/k3
mY7x1wrs/T7jU0GTMVy/Z/Dupmuq5Eej/4lrakzY48kiAzndHYmazBCkIGhzKCsX
JkoGvh5mY5SaRNkNb9ISqQKBgQDKb4NjM4koLfvES0j9R3LcWI3Ixs1FGMRv8Qld
uuq4Eqev7osCT+5pUCQtz5b3TpIVGnhpQg2XXLjX32u3taMZGKKdCkpd8gdorYaF
5SHl9hL3UywoEtpNeJ+0Y4kZtxJuaL1m9Ttd2XJpJDgU7tGDY0JUuFF9kqVSgiGH
z3TLgQKBgCYgswnqxoOk4rR1PaScsVEn/L0iJs/+2j/BmcpsxYZcNkXYz+H1IokD
HFp8544VV+LNm8pgwWwnaQGHnTnmJ+xcK0AVymyIePKDQMHzYycAjM+i2yYSQADX
i59RMPuyS7OFb0AAhuOc7mhapr1zLBpFv2PLnFaYyU2ry4w/dtIr
-----END RSA PRIVATE KEY-----
)KEY";
