# MQTT

* binary headers
* publish/subscribe via broker
* https://en.wikipedia.org/wiki/MQTT

## Example MQTT sequence

| Client<br>A | MSG            | Broker | MSG            | Client<br>B |
| -------- | -----------------:|:------:|:----------------- | -------- |
|| CONNECT ->                  |        |                   |          |
|| <- CONNACK                  |        |                   |          |
|          |          | 25 | <- PUBLISH<br>temp/roof<br>25℃<br>✓Retain||
|| SUBSCRIBE -><br>temp/roof   |        |                   |          |
|| <- PUBLISH<br>temp/roof<br>25℃| 25   |                   |          |
|| PUBLISH -><br>temp/floor<br>20℃ | 20 |                   |          |
||<- PUBLISH<br>temp/floor<br>38℃ |38|<- PUBLISH<br>temp/floor<br>38℃ ||
|| DISCONNECT ->               |        |                   |          |
|          |                   |        |                   |          |


℃ = U2193, ✓ = U2713

## Specification

* http://docs.oasis-open.org/mqtt/mqtt/v5.0/cs01/mqtt-v5.0-cs01.pdf
* http://public.dhe.ibm.com/software/dw/webservices/ws-mqtt/MQTT_V3.1_Protocol_Specific.pdf

## Related links

* http://www.steves-internet-guide.com/mqtt-protocol-messages-overview/

