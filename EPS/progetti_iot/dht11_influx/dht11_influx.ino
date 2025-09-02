
#include <WiFi.h>             /* per protocollo wifi */
#include <DHT.h>              /* per libreria DHT  */
#include <InfluxDbClient.h>   /* per libreria di comunicazione con InfluxDB */
#include <InfluxDbCloud.h>    /* per gestire il token di sicurezza */

#include "esp_task_wdt.h"

/* costanti per le connesioni */

#define WIFI_SSID       "PosteMobile-79159793"
#define WIFI_PASSWORD   "4PF9yedTc2FdN5kzfu4EuSQk"

#define INFLUXDB_URL    "http://informatica-iot.freeddns.org:8086/"
#define INFLUXDB_ORG    "uniurb"
#define INFLUXDB_BUCKET "esercitazioni"
#define INFLUXDB_TOKEN  "qt5kHYb2Wwg19lbGEOe3BmVJhlMf6ZxfDu_Z7Lrhiiiv9FTEGKiD_pJzkDa_qSlUPLPm0zI-1yE6THb4kg-0kA=="

/* costanti per l'accesso a DH11 */

#define DHTPIN  14
#define DHTTYPE DHT11

/* un'istanza di InlfuxDbClient */
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN,
                      InfluxDbCloud2CACert);

/* definisco la misura cioè la tabella in cui inserire i dati */
Point sensors("corso_IoT");

/* creo l'istanza di DHT11 */
DHT dht(DHTPIN, DHTTYPE); 

/* misure provenienti dal sensore */
double t = 0.0;
double h = 0.0;
 

void setup() {
  /* inizializzo la comuincazione seriale */
  Serial.begin(115200);
  /* inizializzo il DHT */
  dht.begin();
  
  // configuro il watchdog per riavviare la CPU se non viene 
  // resettato entro 20 secondi

  esp_task_wdt_config_t config = {
    .timeout_ms = 20* 1000,    //  20 secondi
    .trigger_panic = true,     // Trigger panic == reset della CPU (reboot)
  };
  // configuro il WDT
  esp_task_wdt_reconfigure(&config);

  // abilito il WDT nel loop di Arduino (ogni invocazione del loop resetta il WDT)
  enableLoopWDT();

  // connessione alla rete WiFI
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // controllo se la connessione è avvenuta
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // se supero il loop significa che sono connesso
  // stampo info su connessione
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());

  /* preparo la connessione su influxDB */
  /* aggiungo i tag nel Point per salvare in modo ordinato i dati sul server */
  sensors.addTag("host", "ESP_LEOPIZZI");
  sensors.addTag("location", "Lecce");
  sensors.addTag("room", "Stanza");

  /* testiamo la connessione  su influxDB */
  if(client.validateConnection()){
    Serial.println("Connected to influxDB");
  }else {
    Serial.println("Connection failed!");
    Serial.println(client.getLastErrorMessage());    
  }  

} /* end funzione setup */

void loop() {
  // leggo i sensori
  readSensors();
  // scrivo sul server
  writeToInfluxDB();
  // attendo 10 secondi (non può essere superiore al timer del WDT)
  delay(10000);
}


/* funzione che legge dal sensore DHT11 */
void readSensors(){
  t = dht.readTemperature();
  h = dht.readHumidity();
}

/* funzione per scrivere su influxDB */
void writeToInfluxDB(){
  sensors.clearFields();
  sensors.addField("temperature", t);
  sensors.addField("humidity", h);
  Serial.print("Writing: ");
  Serial.println(sensors.toLineProtocol());

  /* ora scriviamo sul server */
  if(!client.writePoint(sensors)){
    Serial.print("InfluxDB write failed ");
    Serial.println(client.getLastErrorMessage());
    while(1); // se ho attivato il WDT posso mettere in loop la CPU così il WDT riavvierà l'ESP
    // posso anche mettere un delay e poi esp_reastart()
  }  
}
