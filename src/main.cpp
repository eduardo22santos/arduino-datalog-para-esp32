/**
 * @file main.cpp
 * @author Eduardo José dos Santos (eduardo22santos@hotmail.com)
 * @brief Arquivo inicial com a primeira tarefa a ser execultada na inicialização
 * baseado no sistema ARDUINO CORE FOR ESP32.
 * @version 2.0
 * @date 2023-07-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <Arduino.h>
#include <tarefas.h>
#include <Wire.h>
void setup() {
    pinMode(LED_AZUL, OUTPUT);
    ledcAttachPin(LED_AZUL, 1);
    ledcSetup(1, 1000, 8);
	Serial.begin(9600);
    Wire.begin(21,22);
    sensoresBegin();
	tarefasBegin();
}
void loop() {
    vTaskDelete(NULL); 
}