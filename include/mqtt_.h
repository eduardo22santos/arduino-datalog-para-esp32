/**
 * @file mqtt_.h
 * @author Eduardo José dos Santos (eduardo22santos@hotmail.com)
 * @brief Definições de funções que irão gerir a conexão do sistema embarcado com o broker mqtt.
 * @version 2.0
 * @date 2023-07-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef MQTT_H
#define MQTT_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <WiFi.h>
#include <esp_wpa2.h> //wpa2 library for connections to Enterprise networks
#include <esp_wifi.h>
#include <wifi_manager.h> // original do desenvolvedor


/**
 * @brief Função de retorno do mqtt, a qual sempre é chamada quando o tópico 
 * que está sendo ouvido pelo sistema é atualizado.
 * @attention Declarar a função é necessário para o pleno funcionamento da 
 * biblioteca PUBSUBCLIENT.H.
 * @attention Não Foram feitas implementações.
 * 
 * @param topic é o tópico mqtt sendo ouvido.
 * @param payload 
 * @param length 
 */
void callback(char* topic, byte* payload, unsigned int length);

/**
 * @brief Envia os dados de entrada para o broker mqtt.
 * @attention Necessário verificar a API do serviço THINGSPEAK.COM.
 * 
 * @param field1 field 1 da api do serviço thingspeak.
 * @param field2 field 2 da api do serviço thingspeak.
 * @param field3 field 3 da api do serviço thingspeak.
 * @param field4 field 4 da api do serviço thingspeak.
 * @param field5 field 5 da api do serviço thingspeak.
 * @param field6 field 6 da api do serviço thingspeak.
 * @param field7 field 7 da api do serviço thingspeak.
 * @param field8 field 8 da api do serviço thingspeak.
 * @param lat field lat da api do serviço thingspeak.
 * @param longitude field long da api do serviço thingspeak.
 * @param elevation field elevation da api do serviço thingspeak.
 * @param status Campo de status da api do serviço thingspeak.
 * @param topico 
 */
void enviarMqttThingspeak(float field1,float field2,float field3,float field4,float field5,
                float field6, float field7,float field8,float lat,float longitude,float elevation,
                const char * status,const char * topico);

/**
 * @brief Inicializa a conexão com o broker mqtt.
 * 
 * @param eduroamStatus 
 * @param eduroamLogin 
 * @param eduroamSenha 
 * @param wifiSsid 
 * @param wifiSenha 
 * @param mqttHostname 
 * @param mqttPort 
 * @param mqttName 
 * @param mqttUser 
 * @param mqttSenha
 *  @return * void  
 */
void mqttInit(bool eduroamStatus, const char * eduroamLogin, const char * eduroamSenha,const char * wifiSsid,
                const char * wifiSenha,const char * mqttHostname, int & mqttPort, const char * mqttName,
                const char * mqttUser, const char * mqttSenha);

/**
 * @brief Mantém a conexão com o broker mqtt.
 * 
 * @attention Irá piscar o led de status da internet conforme o status de conexão.
 * @attention Está sempre testando o status de conexão com o servidor do broker mqtt 
 * e também com o WiFi.
 * 
 * @param eduroamStatus 
 * @param eduroamLogin 
 * @param eduroamSenha 
 * @param wifiSsid 
 * @param wifiSenha 
 * @param mqttHostname 
 * @param mqttPort 
 * @param mqttName 
 * @param mqttUser 
 * @param mqttSenha 
 * @return * void 
 */
void loopMqtt(bool eduroamStatus, const char * eduroamLogin, const char * eduroamSenha,const char * wifiSsid,
                const char * wifiSenha,const char * mqttHostname, int & mqttPort, const char * mqttName,
                const char * mqttUser, const char * mqttSenha);

/**
 * @brief Retorna o status da conexão com o broker mqtt
 * 
 * @return true se conectado
 * @return false se desconectado
 */
bool getMqttStatus();
#endif // MQTT_H