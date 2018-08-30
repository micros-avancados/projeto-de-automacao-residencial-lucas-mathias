//Programa: Projeto 1 - Sistemas Microprocessados Avançados
//Autores: Lucas Eduardo Pandolfo // Mathias "Cavani" Trevisan
 
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <IRremoteESP8266.h>
//#include <IRsend.h>
//#include <IRutils.h>
//#include "raw_codes.h"                                                 // Include dos códigos do Ar Condicionado

//IRsend irsend(D5);                                                     // Cria instancia da biblioteca IR
#define BotaoModoFunc 13                                               //Botão para alternar modo Config/Opera pino D7

//------------------------------------------------ VARIÁVEIS SENS TEMP ----------------------------------------------------------

//const int LM35 = A0;
//float temperatura;                                                     // Variável que armazenará a temperatura medida

//------------------------------------------------ VARIÁVEIS AC -----------------------------------------------------------------

//bool state = 0;                                                        // Estado Atual do A/C
//bool l_state = 0;                                                      // Ultimo estado do A/C

//------------------------------------------------ WIFI ------------------------------------------------------------------------

const char* SSID = "iPhone de Lucas Eduardo";                          // SSID / nome da rede WI-FI que deseja se conectar
const char* PASSWORD = "detestesenha";                                 // Senha da rede WI-FI que deseja se conectar
WiFiClient espClient;                                                  // Cria o objeto espClient

//------------------------------------------------ MQTT ------------------------------------------------------------------------

const char* BROKER_MQTT = "iot.eclipse.org";                           //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883;                                                // Porta do Broker MQTT


//EDITAR AQUI PARA QUE OS TOPICOS DE SUBSCRIBE E PUBLISH SEJAM OS MESMOS, MAS QUE CADA DISPOSITIVO TENHA O SEU ID MQTT
//                        VVVVVVVVVV
#define TOPICO_SUBSCRIBE "MQTTLucMath"                                 //tópico MQTT de leitura de informações para Broker
#define TOPICO_PUBLISH   "MQTTLucMath"                                 //tópico MQTT de envio de informações para Broker
#define ID_MQTT  "CabeludoSmartHome"                                   //id mqtt (para identificação de sessão)
PubSubClient MQTT(espClient);                                          // Instancia o Cliente MQTT passando o objeto espClient

//------------------------------------------------ DECLARAÇÕES -----------------------------------------------------------------

void initSerial();                                                     //variável que inicia comunicação serial
void initWiFi();                                                       //variável que conecta o wifi
void initMQTT();                                                       //variável que conecta o broker MQTT
void reconectWiFi();                                                   //variável que em caso de perda de sinal, reconecta wifi
void mqtt_callback(char* topic, byte* payload, unsigned int length);   //variável que em caso de conexão, reconecta MQTT
void VerificaConexoesWiFIEMQTT(void);                                  //variável que monitora estado das conexões WIFI e MQTT
//DESCOBRIR QQ FAZ!!      void InitOutput(void);                                                 //variável que ...

//------------------------------------------------ SETUP ----------------------------------------------------------------------

void setup() 
{
    //DESCOBRIR QQ FAZ!!      InitOutput();
    initSerial();
    initWiFi();
    initMQTT();
//    leitura_da_temp();
//    irsend.begin();
//    pinMode(BotaoModoFunc, INPUT_PULLUP); 
}

//------------------------------------------------ LOOPING --------------------------------------------------------------------

void loop() 
{  
  VerificaConexoesWiFIEMQTT();                                         //garante funcionamento das conexões WiFi e ao broker MQTT
  EnviaEstadoOutputMQTT();                                             //envia o status de todos os outputs para o Broker no protocolo esperado
  MQTT.loop();                                                         //keep-alive da comunicação com broker MQTT
/*
  l_state = state;                                                     // iguala os valores

  if (state != l_state) 
    {  
       liga_ac();                                                      // Atualiza o estado
    }
*/

 /* if (temp < 25){
    irsend.sendRaw(desliga, sizeof(desliga) / sizeof(desliga[0]), freq);
    Serial.println("Comando enviado: Desliga");
    delay(2000);
  }
  else if (temp > 25){
    irsend.sendRaw(liga, sizeof(liga) / sizeof(liga[0]), freq);
    Serial.println("Comando enviado: Liga");
    delay(2000);
  }*/
}

//------------------------------------------------ FUNÇÕES ---------------------------------------------------------------------

void initSerial() 
{
    Serial.begin(115200);
}


void initWiFi() 
{
    delay(10);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID);
    Serial.println("Aguarde");
     
    reconectWiFi();
}


void initMQTT() 
{
    MQTT.setServer(BROKER_MQTT, BROKER_PORT);                           //informa qual broker e porta deve ser conectado
    MQTT.setCallback(mqtt_callback);                                    //atribui função de callback (função chamada quando qualquer informação de um dos tópicos subescritos chega)
}


void mqtt_callback(char* topic, byte* payload, unsigned int length) 
{
    String msg;
 
    for(int i = 0; i < length; i++) 
    {
       char c = (char)payload[i];
       msg += c;
    }
   
    //toma ação dependendo da string recebida:
    //verifica se deve colocar nivel alto de tensão na saída D0:
    //IMPORTANTE: o Led já contido na placa é acionado com lógica invertida (ou seja,
    //enviar HIGH para o output faz o Led apagar / enviar LOW faz o Led acender)
    if (msg.equals("L"))
    {
        digitalWrite(D0, LOW);
        EstadoSaida = '1';
    }
 
    //verifica se deve colocar nivel alto de tensão na saída D0:
    if (msg.equals("D"))
    {
        digitalWrite(D0, HIGH);
        EstadoSaida = '0';
    }
     
}


void reconnectMQTT() 
{
    while (!MQTT.connected()) 
    {
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) 
        {
            Serial.println("Conectado com sucesso ao broker MQTT!");
            MQTT.subscribe(TOPICO_SUBSCRIBE); 
        } 
        else
        {
            Serial.println("Falha ao reconectar no broker.");
            Serial.println("Havera nova tentatica de conexao em 2s");
            delay(2000);
        }
    }
}


void reconectWiFi() 
{
    //se já está conectado a rede WI-FI, nada é feito. 
    //Caso contrário, são efetuadas tentativas de conexão
    if (WiFi.status() == WL_CONNECTED)
        return;
         
    WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI
     
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(100);
        Serial.print(".");
    }
   
    Serial.println();
    Serial.print("Conectado com sucesso na rede ");
    Serial.print(SSID);
    Serial.println("IP obtido: ");
    Serial.println(WiFi.localIP());
}
 

void VerificaConexoesWiFIEMQTT(void)
{
    if (!MQTT.connected()) 
        reconnectMQTT(); //se não há conexão com o Broker, a conexão é refeita
     
     reconectWiFi(); //se não há conexão com o WiFI, a conexão é refeita
}
 

void EnviaEstadoOutputMQTT(void)
{
    if (EstadoSaida == '0')
      MQTT.publish(TOPICO_PUBLISH, "D");
 
    if (EstadoSaida == '1')
      MQTT.publish(TOPICO_PUBLISH, "L");
 
    Serial.println("- Estado da saida D0 enviado ao broker!");
    delay(1000);
}
 

/*void InitOutput(void)
{
    //IMPORTANTE: o Led já contido na placa é acionado com lógica invertida (ou seja,
    //enviar HIGH para o output faz o Led apagar / enviar LOW faz o Led acender)
    pinMode(D0, OUTPUT);
    digitalWrite(D0, HIGH);          
}

*/
/*
void leitura_da_temp() 
{
  temperatura = (float(analogRead(LM35))*2.5/(1023.0))/0.01;
  Serial.print("Temperatura: ");
  Serial.println(temperatura);
  char ret[100];
}
*/

/*
void liga_ac() 
{                      
  if (state) 
  {
    irsend.sendRaw(liga, 199, 38);
  }
  else 
  {
    irsend.sendRaw(desliga, 199, 38);
  }
}
*/
