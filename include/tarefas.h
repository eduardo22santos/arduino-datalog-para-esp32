/**
 * @file tarefas.h
 * @author Eduardo José dos Santos (eduardo22santos@hotmail.com)
 * @brief Definições de funções que gerenciam todo o fluxo de controle do sistema.
 * Diria ser o arquivo mais importante para o entendimento do bom funcionamento do sistema.
 * @version 2.0
 * @date 2023-07-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef TAREFAS_H
#define TAREFAS_H

#include <Arduino.h>
#include <sensores.h> // original do desenvolvedor
#include <configuracao.h> // original do desenvolvedor
#include <ssd_rtc.h> // original do desenvolvedor
#include <mqtt_.h> // original do desenvolvedor
#include <wifi_manager.h> // original do desenvolvedor

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>
#include <freertos/semphr.h>

/**
 * @brief  Pino para o led que irá retornar o status de funcionamento do sistema.
 * 
 */
#define LED_AZUL 2

/**
 * @brief Execulta uma tarefa que mantém a conexão com o servidor mqtt, e testa a conexão com a rede WiFi.
 * 
 * @attention Inicia um loop infinito.
 * @attention Ininializa a conexão com a rede WiFi.
 * @attention Inicializa a conexão com o broker mqtt.
 * @attention Faz piscar o led que indica o status da conexão com a internet.
 * 
 * @param pvParameters 
 */
void mqtt_loop(void * pvParameters);

/**
 * @brief Mantém o led de status piscando.
 * 
 * @attention Inicia um loop infinito.
 * 
 * 
 * @param pvParameters 
 */
void led_loop(void * pvParameters);

/**
 * @brief Execulta uma tarefa que atualiza as variaveis temporárias das leituras 
 * dos sensores.
 * 
 * @attention Sempre é encerrada no fim da tarefa.
 * @attention Faz o uso do semáforo semaforoDeIndices quando execultada.
 * @attention Faz o somatório dos valores a cada vez que é chamada
 * 
 * @param pvParameters 
 */
void leituraDeSensores(void * pvParameters);

/**
 * @brief Salva os dados no cartão e envia os dados para o mqtt toda vez que é chamada
 * 
 * @attention Sempre é encerrada no fim da tarefa.
 * @attention Faz o uso do semáforo semaforoDeIndices quando execultada.
 * @attention Ira zerar as variáveis dos sensores a fim de reiniciar o somatório
 * 
*/
void intervaloDeLeitura(void * pvParameters);

/**
 * @brief Reinicia o sistema da placa quando a função é chamada.
 * 
 * @param pvParameters 
 */
void reiniciarEsp(void * pvParameters);

/**
 * @brief Iniciliza os módulos de sd e rtc, define configurções iniciai, 
 * carrega o arquivo de configuração para a memória RAM e inicia os timers que 
 * iram funcionar em loop no sistema.
 * 
 * @attention Criará o arquivo de datalog.csv caso não exista no cartão de memória.
 * @attention Irá inicializar, verificar e atualizar o módulo rtc.
 * @attention Define o timer que faz a leitura dos sensores para intervalos de 6 segundos.
 * @attention Define o timer que faz a média das leituras e salva os dados para intervalos de 60 sengundos.
 * @attention Caso os módulos de sd e rtc não estejam ambos operando e reconhecidos pelo sistema, as etapas de salvar os 
 * dados no sd não serão realizadas.
 * @attention Se a variável platoformaStatus for verdadeira, o sistema irá executar as etapas para enviar os dados para o mqtt.
 * @attention Se a variável platoformaStatus for verdadeira, será iniciado a tarefa que execulta um loop infinito para manter 
 * a conexão com o broker mqtt.
 * @attention Se a variável platoformaStatus for falsa, uma tarefa com um loop infinito para piscar o led de status da placa 
 * será iniciado.
 * @attention Será iniciado um timer que renicia a placa a cada 6 horas quando a função é chamada.
 * @attention Será definido uma interrupção no pino zero, que será acionado quando for prescionado o botão de boot no esp32 devkit.
 * 
 * 
 */
void tarefasBegin();

/**
 * 
 * @brief É chamada pelo timer leituraDosSensores.
 * 
 * @attention Cria a tarefa qua leituraDosSensores, a qual atualiza as variáveis temporárias.
 * 
 * @param time 
 */
void sensoresTimercallback(TimerHandle_t time);

/**
 * @brief É chamada pelo timer reiniciarPlaca.
 * 
 * @attention Execulta uma função que reinica todo o sistema da placa.
 * 
 * @param time 
 */
void reiniciarPlacaTimercallback(TimerHandle_t time);

/**
 * @brief É chamada pelo timer intervaloDasLeitura.
 * 
 * @attention Cria a tarefa intervaloDeLeitura, a qual faz os cálculos das médias e índices das variáveis de ambiente.
 * 
 * @param time 
 */
void intervaloDeLeituraTimerCallback(TimerHandle_t time);

/**
 * @brief Hiberna o sistema da placa para forçar a atualização do horário no módulo rtc.
 * 
 * @attention utiliza métodos da função deep sleep do sistema para hibernar por 1 segundo.
 * @attention Atera o valor da variável atualizarRtcNoBoot para verdadeiro antes de hiberna a placa.
 * 
 */
void atualizarHorarioManualmente();

#endif // TAREFAS_H