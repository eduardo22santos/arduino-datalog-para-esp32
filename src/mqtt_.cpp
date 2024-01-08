/**
 * @file mqtt_.cpp
 * @author Eduardo José dos Santos (eduardo22santos@hotmail.com)
 * @brief Definições de funções que irão gerir a conexão do sistema embarcado com o broker mqtt.
 * @version 2.0
 * @date 2023-07-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <mqtt_.h>

WiFiClient espClient;
PubSubClient client(espClient);

void enviarMqttThingspeak(float field1,float field2,float field3,float field4,float field5,
                float field6, float field7,float field8,float lat,float longitude,float elevation,
                const char * status,const char * topico)
{
    if (client.connected())
    {    
        String enviar = String("field1="+String(field1) +
                        "&field2="+String(field2)+
                        "&status="+status);
        
        if(client.publish(String("channels/"+ String(topico) +"/publish").c_str(), enviar.c_str()))
        {
            Serial.println(enviar);
            Serial.println("Enviado ao topico com sucesso!");
        }else
        {
            Serial.println(enviar);
            Serial.println("Falha ao enviar o dado!");
        }
    }
}

void mqttInit(bool eduroamStatus, const char * eduroamLogin, const char * eduroamSenha,const char * wifiSsid,
                const char * wifiSenha,const char * mqttHostname, int & mqttPort, const char * mqttName,
                const char * mqttUser, const char * mqttSenha)
{
    client.setServer(mqttHostname,mqttPort);
    client.setCallback(callback);
    if(client.connect(mqttName,mqttUser,mqttSenha))
    {
        Serial.println("Conectado ao broker com sucesso!");
    }else
    {
        Serial.println("Falha ao conectar com o broker!");
    }
}

void loopMqtt(bool eduroamStatus, const char * eduroamLogin, const char * eduroamSenha,const char * wifiSsid,
                const char * wifiSenha,const char * mqttHostname, int & mqttPort, const char * mqttName,
                const char * mqttUser, const char * mqttSenha)
{
        if (!client.connected())
        {
            if (WiFi.isConnected())
            {
                client.setServer(mqttHostname,mqttPort);
                client.connect(mqttName,mqttUser, mqttSenha);
            }else
            {
                updateWifi();
            }
        }else
        {
            client.loop();            
        }
}

void callback(char* topic, byte* payload, unsigned int length)
{

}
bool getMqttStatus()
{
    return client.connected();
}

