#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <stdlib.h>
#include <stdio.h>
const int LM35 = A0; // Define o pino que lera a saída do LM35
float temperatura; // Variável que armazenará a temperatura medida
 

/* Set these to your desired credentials. */
const char *ssid = "projetouno";
const char *password = "huehuehuebr";

char mensagem[100];

ESP8266WebServer server(80);

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */
void handleRoot() {
	server.send(200, "text/html",
	  "<h1>You are connected</h1>"
	  "<br>"
    "<a href=\"leitura_temp\">leitura_temp</a>"
    "<br><br>"
    );
}

void leitura_da_temp()
{
  temperatura = (float(analogRead(LM35))*2.5/(1023.0))/0.01;
  Serial.print("Temperatura: ");
  Serial.println(temperatura);

  char ret[100];

  sprintf(ret, "<p>temperatura = <b>%0.2f</b></p>", temperatura);

  server.send(200, "text/html", ret);
}

void setup() 
{
	delay(1000);
	Serial.begin(115200);
	Serial.println();
	Serial.print("Configuring access point...");
	/* You can remove the password parameter if you want the AP to be open. */
	WiFi.softAP(ssid, password);

	IPAddress myIP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(myIP);
	server.on("/", handleRoot);
  server.on("/leitura_temp", leitura_da_temp);
	server.begin();
	Serial.println("HTTP server started");

  pinMode(D0, OUTPUT);
}



void loop() 
{
  //Serial.println(mensagem);
  
	server.handleClient();
}



