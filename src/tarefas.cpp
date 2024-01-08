/**
 * @file tarefas.cpp
 *  @author Eduardo José dos Santos (eduardo22santos@hotmail.com)
 * @brief Definições de funções que gerenciam todo o fluxo de controle do sistema.
 * Diria ser o arquivo mais importante para o entendimento do bom funcionamento do sistema.
 * @version 2.0
 * @date 2023-07-02
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <tarefas.h>

/**
 * @brief variável para criação do timer que fara a leitura dos sensores
 *
 */
TaskHandle_t leituraDosSensores = NULL;

/**
 * @brief variável para criação do timer irá salvar os dados no sd e enviar para o mqtt.
 *
 */
TaskHandle_t intervaloDasLeituras = NULL;

/**
 * @brief variável para identificar erros durante a depuração na tarefa mqtt_loop
 *
 */
TaskHandle_t mqtt_loop_ = NULL;

/**
 * @brief variável para identificar erros durante a depuração na tarefa led_loop
 *
 */
TaskHandle_t led_loop_ = NULL;

/**
 * @brief variável para identificar erros durante a depuração na tarefa leituraDeSensores
 *
 */
TaskHandle_t leituraDeSensores_;

/**
 * @brief variável para identificar erros durante a depuração na tarefa intervaloDeLeitura
 *
 */
TaskHandle_t intervaloDeLeitura_;

/**
 * @brief variável para criação do timer que irá reiniciar a placa em intervalos definidos.
 *
 */
TaskHandle_t reiniciarPlaca;

/**
 * @brief Semáforo para evitar o acesso simultâneo das variáveis de ambiente e índices calculados.
 *
 */
SemaphoreHandle_t semaforoDosIndices;

/**
 * @brief Indicador de atualização do horário no móduo rtc ds3231.
 * @attention variável armazenada na memória do rtc interno do esp32,
 * logo se mantém quando o sistema é reinicado.
 *
 */
RTC_DATA_ATTR bool atualizarRtcNoBoot = false;

Configuracao config;
DateTime tempoAtual;

void tarefasBegin()
{

    xTaskCreatePinnedToCore(led_loop, "led_loop", configMINIMAL_STACK_SIZE, NULL, 0, &led_loop_, 1);

    vSemaphoreCreateBinary(semaforoDosIndices);
    uint8_t cont;
    do
    {
        if (SD.begin())
        {
            config = carregarConfiguracao();
            config.ssd = true;
            // Verifica se existe o arquivo datalog no cartao de memoria, criando-o caso contrario
            if (!SD.exists("/datalog.csv")) // verifica ou cria o arquivo datalog.txt
            {
                File file = SD.open("/datalog.csv", FILE_WRITE);
                file.close();
                appendFile(SD, "/datalog.csv",
                           "data,hora,s0u,s0t,s1u,s1t,s2u,s2t,s3u,s3t,s4u,s4t,s5u,s5t,s6u,s6t,ldr1,ldr2");
            }
            if (SD.exists("/config.json"))
            {
                loadConfiguration("/config.json", config);
                SD.end();
            }
            if (!SD.exists("/exemplo.json"))
            {
                File file = SD.open("/exemplo.json", FILE_WRITE);
                if (!file)
                {
                    Serial.println(F("Failed to create file"));
                    return;
                }

                StaticJsonDocument<768> doc;

                doc["wifi_ssid"] = "ssid";
                doc["wifi_senha"] = "senha";
                doc["wifi_status"] = true;
                doc["eduroam_login"] = "login da rede eduroam";
                doc["eduroam_senha"] = "senha da rede eduroam";
                doc["eduroam_status"] = false;
                doc["mqtt_host"] = "mqtt3.thingspeak.com";
                doc["mqtt_porta"] = 1883;
                doc["mqtt_senha"] = "senha_do_broker_mqtt";
                doc["mqtt_usuario"] = "usuario_do_broker_mqtt";
                doc["mqtt_topico"] = "Canal_thingspeak";
                doc["plataforma_status"] = true;
                doc["intervalo_internet"] = 1;
                doc["intervalo_sd"] = 1;
                doc["fuso_horario"] = -3;
                doc["ntp_host"] = "0.br.pool.ntp.org";
                doc["host_teste_conexao"] = "https://ntp.br";

                serializeJsonPretty(doc, file);
                file.close();
                SD.end();
            }
            break;
        }
        else
        {
            config.ssd = false;
            cont++;
        }

    } while (cont < 3);

    if (!config.ssd)
    {
        config = carregarConfiguracao();
        config.ssd = false;
    }

    if (rtcBegin())
    {
        if (!verificarRtc() && config.internetStatus)
        {
            initWifi(config);
            atualizarRtc(config);
        }
        else if (atualizarRtcNoBoot && config.internetStatus)
        {
            initWifi(config);
            atualizarRtc(config);
        }
        else if (!verificarRtc())
        {
            config.ssd = false;
            config.rtc = false;
        }
        else
        {
            config.rtc = true;
        }
    }
    else
    {
        config.rtc = false;
        config.ssd = false;
    }
    Serial.println(returnHorario().timestamp().c_str());

    leituraDosSensores = xTimerCreate("leitura_dos_sensores", pdMS_TO_TICKS(config.intervaloSalvar * 6000), pdTRUE, 0, sensoresTimercallback);
    xTimerStart(leituraDosSensores, 0);

    intervaloDasLeituras = xTimerCreate("intervalo_de_leituras", pdMS_TO_TICKS(config.intervaloSalvar * 60000), pdTRUE, 0, intervaloDeLeituraTimerCallback);
    xTimerStart(intervaloDasLeituras, 0);

    if (config.mqttStatus)
    {
        xTaskCreatePinnedToCore(mqtt_loop, "mqtt_loop", 10000, &config, 1, &mqtt_loop_, 0);
    }

    reiniciarPlaca = xTimerCreate("reiniciar a placa", pdMS_TO_TICKS(21600000), pdFALSE, 0, reiniciarPlacaTimercallback);
    xTimerStart(reiniciarPlaca, 0);

    pinMode(0, INPUT_PULLUP);
    attachInterrupt(0, atualizarHorarioManualmente, FALLING);
}

void leituraDeSensores(void *pvParameters)
{
    Configuracao config = *(Configuracao *)pvParameters;

    if (xSemaphoreTake(semaforoDosIndices, portMAX_DELAY) == pdTRUE)
    {
        realizarLeitura();

        while (xSemaphoreGive(semaforoDosIndices) != pdTRUE)
            ;
    }

    vTaskDelete(NULL);
}
void intervaloDeLeitura(void *pvParameters)
{

    if (xSemaphoreTake(semaforoDosIndices, portMAX_DELAY) == pdTRUE)
    {
        // Salvando os dados no cartão SD
        Configuracao config = *(Configuracao *)pvParameters;
        if (config.ssd && config.rtc)
        {
            if (!SD.begin())
            {
                ESP.restart();
            }
            tempoAtual = returnHorario();
            if (!tempoAtual.isValid())
            {
                esp_restart();
            }
        }

        char horarioArquivo[9] = "hh:mm:ss";
        char dataArquivo[11] = "DD/MM/YYYY";

        //"data,hora,s0u,s0t,s1u,s1t,s2u,s2t,s3u,s3t,s4u,s4t,s5u,s5t,s6u,s6t,s7u,s7t,s8u,s8t,s9u,s9t,s10u,s10t,s11u,s11t"
        String datalog = String(tempoAtual.toString(dataArquivo)) + "," + String(tempoAtual.toString(horarioArquivo)) + getString();
        // Salva no arquivo datalog
        Serial.println(datalog);

        if (config.ssd && config.rtc)
        {
            appendFile(SD, "/datalog.csv", datalog.c_str());
            SD.end();
        }

        if (config.mqttStatus)
        {

            enviarMqttThingspeak(getTemperatura(0), getUmidade(0), getTemperatura(1), getUmidade(1),
                                 getTemperatura(2), getUmidade(2), getTemperatura(3), getUmidade(3),
                                 getTemperatura(4), getUmidade(4),  getTemperatura(5), String(datalog).c_str(), config.mqttTopico);
        }

        zeraVariaveis(); // Reinicia o somatório dos dados coletados dos sensores.

        while (xSemaphoreGive(semaforoDosIndices) != pdTRUE)
            ;
    }
    vTaskDelete(NULL);
}
void mqtt_loop(void *pvParameters)
{
    Configuracao config = *(Configuracao *)pvParameters;
    initWifi(config);
    mqttInit(config.eduroamStatus,
             config.eduroanLogin,
             config.eduroanSenha,
             config.wifiSsid,
             config.wifiSenha,
             config.mqttHostname,
             config.mqttPort,
             config.mqttUser,
             config.mqttUser,
             config.mqttSenha);
    while (true)
    {
        loopMqtt(config.eduroamStatus,
                 config.eduroanLogin,
                 config.eduroanSenha,
                 config.wifiSsid,
                 config.wifiSenha,
                 config.mqttHostname,
                 config.mqttPort,
                 config.mqttUser,
                 config.mqttUser,
                 config.mqttSenha);

        delay(10);
    }
}

void sensoresTimercallback(TimerHandle_t time)
{
    xTaskCreatePinnedToCore(leituraDeSensores, "leituraDeSensores", 10000, &config, 1, &leituraDeSensores_, 1);
}
void reiniciarPlacaTimercallback(TimerHandle_t time)
{
    esp_restart();
}
void intervaloDeLeituraTimerCallback(TimerHandle_t time)
{
    xTaskCreatePinnedToCore(intervaloDeLeitura, "intervaloDeleitura", 10000, &config, 1, &intervaloDeLeitura_, 1);
}
void atualizarHorarioManualmente()
{
    atualizarRtcNoBoot = true;
    esp_sleep_enable_timer_wakeup(1000000);
    esp_deep_sleep_start();
}
void led_loop(void *pvParameters)
{
    while (true)
    {

        for (size_t i = 0; i < 255; i++)
        {
            ledcWrite(1, i);
            delay(2);
        }
        for (size_t i = 254; i > 0; i--)
        {
            ledcWrite(1, i);
            delay(2);
        }
        delay(10);
    }
}