/**
 * @file wifi_manager.cpp
 * @author Eduardo José dos Santos (eduardo22santos@hotmail.com)
 * @brief Classe e funções para gerenciar o funcionamento da conexão rede WiFi.
 * @version 2.0
 * @date 2023-07-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <wifi_manager.h>

unsigned long wifiTime = 0;

WifiManager conexao;

void initWifi(Configuracao config)
{
    //Atribuimos ao canal 0 a frequencia de 1000Hz com resolucao de 10bits.
    WiFi.disconnect(true);  //disconnect form wifi to set new wifi connection
    WiFi.mode(WIFI_STA); //init wifi mode
    if(config.eduroamStatus)
    {
        esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)config.eduroanLogin, strlen(config.eduroanLogin)); //provide identity
        esp_wifi_sta_wpa2_ent_set_username((uint8_t *)config.eduroanLogin, strlen(config.eduroanLogin)); //provide username --> identity and username is same
        esp_wifi_sta_wpa2_ent_set_password((uint8_t *)config.eduroanSenha, strlen(config.eduroanSenha)); //provide password
        esp_wifi_sta_wpa2_ent_enable();
        WiFi.begin(config.wifiSsid);
        delay(5000);
    }else
    {
        WiFi.begin(config.wifiSsid, config.wifiSenha);
        delay(5000);
    }
    wifiTime = millis();
    updateWifi();
}
void reconnectWifi()
{    
    if ((millis() - wifiTime) >= 5000)
    {
        WiFi.reconnect();
        wifiTime = millis();
    }
}
void updateWifi()
{
    if (!WiFi.isConnected())
    {
        Serial.println("Tentando conectar ao wifi!");
        conexao.wifiStatus = false;
        reconnectWifi();

    }else
    {
        Serial.println("Conectado ao wifi!");
        conexao.wifiStatus = true;
    }
}
bool getWifiStatus()
{
    return conexao.wifiStatus;
}