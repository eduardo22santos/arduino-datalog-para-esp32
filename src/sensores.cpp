/**
 * @file sensores.cpp
 * @author Eduardo José dos Santos (eduardo22santos@hotmail.com)
 * @brief Definições de classes, funções e métodos para obter, calcular e gerenciar as variáveis térmicas.
 * @version 2.0
 * @date 2023-07-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <sensores.h>

DHT_Unified s0(pin0,DHT22);
DHT_Unified s1(pin1,DHT22);
DHT_Unified s2(pin2,DHT22);
DHT_Unified s3(pin3,DHT22);
DHT_Unified s4(pin4,DHT22);
DHT_Unified s5(pin5,DHT22);
DHT_Unified s6(pin6,DHT22);
DHT_Unified s7(pin7,DHT22);
DHT_Unified s8(pin8,DHT22);
DHT_Unified s9(pin9,DHT22);
DHT_Unified s10(pin10,DHT22);
DHT_Unified s11(pin11,DHT22);

Dados sensor0(&s0);
Dados sensor1(&s1);
Dados sensor2(&s2);
Dados sensor3(&s3);
Dados sensor4(&s4);
Dados sensor5(&s5);
Dados sensor6(&s6);
Dados sensor7(&s7);
Dados sensor8(&s8);
Dados sensor9(&s9);
Dados sensor10(&s10);
Dados sensor11(&s11);

Dados sensores[] = {sensor0,sensor1,sensor2,sensor3,sensor4,
                        sensor5,sensor6,sensor7,sensor8,sensor9,
                        sensor10,sensor11};


void realizarLeitura()
{

    for (size_t i = 0; i < 12; i++)
    {
        sensors_event_t event;
        Serial.print("\nsensor: ");Serial.println(i);
        sensores[i].sensor->temperature().getEvent(&event);
        if (isnan(event.temperature)) {
            Serial.println(F("Error reading temperature!"));
        }
        else {
            Serial.print(F("Temperature: "));
            Serial.print(event.temperature);
            Serial.println(F("°C"));
            sensores[i].temperatura += event.temperature;
        }
        // Get humidity event and print its value.
        sensores[i].sensor->humidity().getEvent(&event);
        if (isnan(event.relative_humidity)) {
            Serial.println(F("Error reading humidity!"));
        }
        else {
            Serial.print(F("Humidity: "));
            Serial.print(event.relative_humidity);
            Serial.println(F("%"));
            sensores[i].umidadeRelativa += event.relative_humidity;
        }
    }

    
}

void sensoresBegin()
{
    for (size_t i = 0; i < 12; i++)
    {
        sensores[i].sensor->begin();
    }
}

float Dados::getTemperatura()
{
    return temperatura/10;
}
float Dados::getUmidade()
{
    return umidadeRelativa/10;
}
void Dados::zeraVariaveis()
{
    temperatura = 0;
    umidadeRelativa = 0;
}
void zeraVariaveis()
{
    for (size_t i = 0; i < 12; i++)
    {
        sensores[i].zeraVariaveis();
    }
}
String getString()
{
    String datalog;
    for (size_t i = 0; i < 12; i++)
    {
        datalog.operator+=(String(","+String(sensores[i].getUmidade())+","+String(sensores[i].getTemperatura())));
    }
    return datalog;
}
float getUmidade(size_t i)
{
    return sensores[i].getUmidade();
}
float getTemperatura(size_t i)
{
    return sensores[i].getTemperatura();
}