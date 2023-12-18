# ESTRUTURAÇÃO DO PROJETO
> ### **projeto_bee**
>> #### **include**
>> > ##### configuracao.h
>> > ##### mqtt_.h
>> > ##### sensores.h
>> > ##### ssd_rtc.h
>> > ##### tarefas.h
>> > ##### wifi_manager.h
>> #### **lib**
>> > #### **NTPClient**
>> #### **src**
>> > #### main.cpp
>> > #### mqtt_.cpp
>> > #### sensores.cpp
>> > #### ssd_rtc.cpp
>> > #### tarefas.cpp
>> > #### wifi_manager.cpp
>> ### platformio.ini
>> ##### README.md

# FUNCIONAMENTO GERAL DO SISTEMA
O sistema é caracterizado como um sistema de tempo real não crítico o qual utiliza a biblioteca de comandos do FREERTOS junto ao ARDUINO CORE TO THE ESP32.

Note no arquivo main.cpp que toda configuração do sistema está descrita nas funções **sensoresBegin()** e **tarefasBegin**, as quais são chamadas na função **setup()** do ARDUINO CORE. A função loop() não foi utilizada e é encerrada assim que chamada.

## DESCRIÇÃO RESUMIDA DA LIB TAREFAS
A biblioteca tarefas reune os métodos de responsáveis por gerenciar a lógica de funcionamento do sistema.
Os métodos principais são:
1. tarefasBegin();
2. leituraDeSensores();
3. intervaloDeLeitura();

### tarefasBegin()
Método principal do sistema. Verifica as configurações no cartão sd. Cria os arquivos **datalog.csv** e **exemplo.json**.
É nesta função que deve ser configurada a primeira linha do arquivo **datalog.csv**, o nome das colunas.
A função inicializa dois temporizadores **leituraDosSensores** e **intervaloDasLeituras**, os quais são responsáveis por coletar os dados dos sensores e salvar no sd/mqtt.
Também é inicializado um semáforo - **semaforoDosIndices** - o qual impede acesso simutáneo de recursos entre as funções **leituraDeSensores()** e **intervaloDeLeitura()**.

### leituraDeSensores()
É uma tarefa execultada pelo temporizador **leituraDosSensores**, a qual é execultada 10 vezes a cada período de coleta de dados, executando a funão **realizarLeitura()** da lib **sensores.h**, a qual coleta os dados de cada sensor realizando um somatório.

### intervaloDeLeitura()
É uma tarefa execultada pelo temporizador **intervaloDasLeituras**, a qual é execultada conforme descrito no arquivo de configuração, salvando os dados no cartão de memoria e enviando para o broker mqtt.
Éla utiliza os metódos da lib **sensores.h** para obter os dados de cada sensor e após salvar e enviar os dados, zera as variváveis de cada sensor, a fim de recomeçar o somatório de cada sensor.