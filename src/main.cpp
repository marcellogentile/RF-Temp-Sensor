#include <Arduino.h>
#include <DHT.h>

#define MY_DEBUG
#define MY_RADIO_RF24
#define MY_NODE_ID 1

#define CHILD_ID_TEMP 0
#define CHILD_ID_HUM 1
#define DHT_DATA_PIN 3
#define SENSOR_TEMP_OFFSET 0

#include <MySensors.h>

/**************************************************/
/****************** CONSTANTS *********************/
/**************************************************/

static const uint64_t UPDATE_INTERVAL = 10000;
static const uint8_t FORCE_UPDATE_N_READS = 10;

/**************************************************/
/****************** VARIABLES *********************/
/**************************************************/
float lastTemp;
float lastHum;
float temperature;
float humidity;
uint8_t nNoUpdatesTemp;
uint8_t nNoUpdatesHum;

/**************************************************/
/****************** MESSAGES **********************/
/**************************************************/

MyMessage msgHum(CHILD_ID_HUM, V_HUM);
MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP);

DHT dht;

void presentation() 
{ 
present(CHILD_ID_HUM, S_HUM);
present(CHILD_ID_TEMP, S_TEMP);
}


void setup()
{
delay(2000); //Wait 2 seconds before starting sequence
Serial.begin(115200);
if (UPDATE_INTERVAL <= dht.getMinimumSamplingPeriod()) 
{
Serial.println("Warning: UPDATE_INTERVAL is smaller than supported by the sensor!");
}

sleep(dht.getMinimumSamplingPeriod()); 
dht.setup(DHT_DATA_PIN);
}

/**************************************************/
/**************** AUX. FUNCTIONS ******************/
/**************************************************/

void sendTemperatureHumiditySensor()
{
dht.readSensor(true);
temperature = dht.getTemperature();
humidity = dht.getHumidity();

if (isnan(temperature)) 
{
Serial.println("Failed reading temperature from DHT!");
} else if (temperature != lastTemp || nNoUpdatesTemp == FORCE_UPDATE_N_READS) 
{
lastTemp = temperature;
nNoUpdatesTemp = 0;
temperature += SENSOR_TEMP_OFFSET;
send(msgTemp.set(temperature, 1));

#ifdef MY_DEBUG
Serial.print("T: ");
Serial.println(temperature);
#endif
} else 
{
nNoUpdatesTemp++;
}

if (isnan(humidity)) 
{
Serial.println("Failed reading humidity from DHT");
} else if (humidity != lastHum || nNoUpdatesHum == FORCE_UPDATE_N_READS)
{
lastHum = humidity;
nNoUpdatesHum = 0;
send(msgHum.set(humidity, 1));

#ifdef MY_DEBUG
Serial.print("H: ");
Serial.println(humidity);
#endif
} else 
{
nNoUpdatesHum++;
} 
}

void loop() 
{ 
sendTemperatureHumiditySensor(); 
wait(UPDATE_INTERVAL);
}