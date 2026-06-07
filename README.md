# TCC

# Hardwares utilizados

- Esp32-C3 super mini
- Esp32-C3 super mini com display OLED 0.42
- Sensor DHT22
- Raspberry Pi 4 8GB
- Cartão de memória 128GB

# Ferramentas utilizadas

- Arduino IDE
- Raspberry Pi Imager

# Bibliotecas utilizadas

- DHT sensor library (por Adafruit) - v1.4.7
- Adafruit Unified Sensor (por Adafruit) - v.1.15
- PubSubClient (por Nick O'Leary) - v2.8
- ArduinoJson (por Benoit Blanchon) - v7.4.3
- Adafruit SSD1306 (por Adafruit) - v2.5.17
- Adafruit GFX Library (por Adafruit) - v1.12.6
- Adafruit BusIO (por Adafruit) - v1.17.4

# Conectando o Arduino IDE ao Esp32

- Adicionar a URL do ESP32 na IDE
    - Abra a Arduino IDE.
    - Vá no menu Arduino IDE > Ajustes....
    - No campo URLs Adicionais de Gerenciadores de Placas, cole o seguinte link:
    ```
    https://espressif.github.io/arduino-esp32/package_esp32_index.json
    ```

- Instalar a placa na IDE
    - Vá em Ferramentas > Placa > Gerenciador de Placas
    - Na barra de pesquisa, digite ESP32.
    - Encontre o pacote chamado esp32 (feito pela Espressif Systems) e clique em Instalar

- Selecionar a placa e a porta
    - Conecte o seu ESP32 ao computador usando um cabo USB que transmita dados, não apenas energia
    - Mantenha o botão boot pressionado, dê um clique no botão reset e solte o botão boot 
    - Vá em Ferramentas > Placa > ESP32
    - Selecione exatamente: "ESP32C3 Dev Module"
    - Selecione a porta correspondente (a nomeclatura muda de sistema para sistema, teste removendo e inserindo novamente o ESP32 e veja qual porta apareceu apenas com a placa conectada)
    - Vá em Ferramentas > USB CDC On Boot > Enabled

- Testando a placa com o blink
    - Cole o código abaixo na sua IDE:
    ```c++
    // O LED nativo da maioria das placas ESP32 está no pino 8
    #define LED_PIN 8

    void setup() {
        pinMode(LED_PIN, OUTPUT);
    }

    void loop() {
        digitalWrite(LED_PIN, HIGH); // Liga o LED
        delay(1000);                 // Aguarda 1 segundo
        digitalWrite(LED_PIN, LOW);  // Desliga o LED
        delay(1000);                 // Aguarda 1 segundo
    }
    ```
    - Clique no botão de seta para a direita (Carregar/Upload) no topo da IDE

- Instalando as libs
    - Vá em Ferramentas > Gerenciar Bibliotecas...
    - Busque por "DHT sensor library" (da Adafruit) e clique em instalar.
    - A IDE provavelmente vai pedir para instalar uma dependência chamada "Adafruit Unified Sensor". Clique em Instalar Todas (Install All).
    - Se estiver com a versão da esp32 sem o display OLED, pode avançar para a próxima etapa. Caso esteja com a versão da esp32 com o display OLED, busque por Adafruit SSD1306 e clique em instalar.
    - A IDE provavelmente vai pedir para instalar as dependências "Adafruit GFX" e "Adafruit BusIO". Clique em Instalar Todas (Install All).

- Testando o sensor (versão sem display) *OBS: na versão com display trocar o DHTPIN 5 por DHTPIN 3 
    - Cole o código abaixo na sua IDE:
    ```c++
    #include "DHT.h"

    #define DHTPIN 5

    #define DHTTYPE DHT22

    DHT dht(DHTPIN, DHTTYPE);

    unsigned long lastTime = 0;

    const unsigned long INTERVAL = 5000;

    void setup() {
        Serial.begin(115200);

        while (!Serial) {
            delay(10);
        }

        pinMode(DHTPIN, INPUT_PULLUP);
        dht.begin();
    }

    void loop() {
        unsigned long now = millis();

        if (now - lastTime >= INTERVAL) {
            lastTime = now; 

            float umidade = dht.readHumidity(true);
            float temperatura = dht.readTemperature(false, true);

            if (isnan(umidade) || isnan(temperatura)) {
            Serial.println(F("[Aviso] Falha de comunicação. Tentando novamente em breve..."));
            lastTime = now - (INTERVAL - 100);
            return;
            }

            Serial.print(F("Umidade: "));
            Serial.print(umidade, 1);
            Serial.print(F("%  |  Temperatura: "));
            Serial.print(temperatura, 1);
            Serial.println(F("°C"));

            Serial.flush(); 
        }
    }
    ```
    - Clique no botão de seta para a direita(Carregar/Upload) no topo da IDE
    - Veja o log no Serial Monitor do Arduino IDE 
# Montando o servidor
 
- Montando o raspberry pi
    - Baixe o Raspberry Pi Imager
    - Conecte o cartão de memória no computador (OBS: ele será formatado)
    - Instale o Raspberry Pi OS Lite no cartão de memória utilizando o Raspberry Pi Imager (ative a opção SSH durante a instalação)

-  Instalar o docker
   - Atualizar a lista de pacotes do sistema
    ```
    sudo apt update && sudo apt upgrade -y
    ```
    - Baixar o script oficial de instalação do Docker
    ```
    curl -fsSL https://get.docker.com -o get-docker.sh
    ```
    - Executa o script para instalar o Docker Core
    ```
    sudo sh get-docker.sh
    ```
    - Instalar o plugin do Docker Compose (necessário para ler o arquivo docker-compose.yml)
    ```
    sudo apt install docker-compose-plugin -y
    ```
    - Dar ao usuário local permissão para rodar os comandos do docker sem precisar do sudo
    ```
    sudo usermod -aG docker user
    ```
    - Aplicar as permissões sem precisar reiniciar o terminal
    ```
    newgrp docker
    ```

- Instalar o node-red
    - Criar a pasta nodered
    - Acessar a pasta nodered e criar o arquivo docker-compose.yml
    ```Yaml
    services:
        mosquitto:
            image: eclipse-mosquitto:latest
            container_name: broker_teste
            ports:
            - "1883:1883"
            volumes:
            - mosquitto_data:/mosquitto/data
            - ./mosquitto.conf:/mosquitto/config/mosquitto.conf
            restart: unless-stopped

        nodered:
            image: nodered/node-red:latest
            container_name: nodered_teste
            ports:
            - "1880:1880"
            volumes:
            - nodered_data:/data
            restart: unless-stopped

    volumes:
        mosquitto_data:
        nodered_data:
    ```
    - Criar o arquivo mosquitto.conf
    ```
    listener 1883 0.0.0.0
    allow_anonymous true
    persistence true
    persistence_location /mosquitto/data/
    ```
    - Criar e iniciar o container
    ```
    docker compose up
    ```

- Configurar o Node-RED
    - Acessar o endereço de um computador na mesma máquina que o raspberry pi
    ```
    http://IP_RASPBERRY:1880
    ```
    - Instalar o @flowfuse/node-red-dashboard
        - Clique no Menu (três linhas horizontais no canto superior direito) e selecione Manage palette (Gerenciar paleta).
        - Ir até a aba Install.
        - Digitar na busca: @flowfuse/node-red-dashboard
        - Clicar no botão Install ao lado dele. Uma nova coleção de nós azuis chamados "Dashboard" vai aparecer na sua barra lateral esquerda.
    - Configurar a entrada da informação recebida
        - Arrastar o nó mqtt in para a área da tela (para cada esp32 instalado, será adicionado 1 nó do tipo mqtt in)
        - Dê 2 cliques nele para configurar
        - Em Server, clique no ícone de lápis para adicionar um novo broker
            - Name: Mosquitto Local
            - Server: IP_RASPBERRY
            - Port: 1883
            - Clique em Adicionar
        - Preencher o Topic exatamente como topic enviado no código
        - Deixe o Output configurado como auto-detecção
        - Clique em Feito.
        - Arraste o nó json para o fluxo e conecte a entrada dele com a saída do nó mqtt in
        - Dê 2 cliques nele para configurar
            - Ação: Sempre converter para objeto JavaScript
            - Propriedade: msg.payload
            - Clique em Feito
        - Arraste o nó change para o fluxo e conecte a entrada dele com a saída do nó json (para cada campo enviado pelo mesmo esp32, será adicionado 1 nó do tipo change)
        - Dê 2 cliques nele para configurar
            - Definir: msg.payload
            - para o valor: msg.payload.<CAMPO_ENVIADO>
            - Clique em Feito
        - Arraste o nó chart para o fluxo e conecte a entrada dele com a saída do nó change
        - Dê 2 cliques nele para configurar
            - Aqui as alterações são escolhas pessoais, podendo configurar o gráfico de acordo com o desejado.
            - Clique em Feito
        - Clique no botão vermelho Implementar no canto superior direito do Node-RED.
        - Acesse o link abaixo para ver o gráfico gerado pelas informações
        ```
        http://IP_RASPBERRY:1880/dashboard
        ```
# Gerando relatório
- Criar o arquivo get_stats.sh
```sh
#!/bin/bash
LOG_FILE="/caminho/para/docker_stats.log"

# Formato: Data Hora | Nome | CPU% | Mem%
docker stats --no-stream --format "{{.Name}} {{.CPUPerc}} {{.MemPerc}}" | while read -r line; do
    echo "$(date '+%Y-%m-%d %H:%M:%S') $line" >> "$LOG_FILE"
done
```
- Criar o arquivo generate_report.sh
```sh
#!/bin/bash
LOG_FILE="/caminho/para/docker_stats.log"
RELATORIO_DIR="/caminho/para/pasta/relatorios"
DATA_ATUAL=$(date '+%Y-%m-%d')
ARQUIVO_FINAL="$RELATORIO_DIR/relatorio_$DATA_ATUAL.txt"

if [ ! -f "$LOG_FILE" ] || [ ! -s "$LOG_FILE" ]; then
    echo "Nenhum dado coletado para o relat  rio."
    exit 1
fi

echo "==================================================" > "$ARQUIVO_FINAL"
echo "  RELATARIO DIARIO DE CONSUMO DOCKER - $DATA_ATUAL" >> "$ARQUIVO_FINAL"
echo "==================================================" >> "$ARQUIVO_FINAL"
printf "%-25s %-15s %-15s\n" "Container" "Media CPU" "Media memoria" >> "$ARQUIVO_FINAL"
echo "--------------------------------------------------" >> "$ARQUIVO_FINAL"

# Processa o log eliminando o caractere '%' para fazer o calculo
cat "$LOG_FILE" | sed 's/%//g' | awk '{
    container=$3; cpu=$4; mem=$5;
    sum_cpu[container]+=cpu;
    sum_mem[container]+=mem;
    count[container]++;
}
END {
    for (c in sum_cpu) {
        printf "%-25s %-14.2f%% %-14.2f%%\n", c, sum_cpu[c]/count[c], sum_mem[c]/count[c]
    }
}' >> "$ARQUIVO_FINAL"

echo "==================================================" >> "$ARQUIVO_FINAL"

# Limpa o arquivo de logs
> "$LOG_FILE"
```
- Configutar a execução dos scripts
    - Rodar crontab -e
    - Adicionar as linhas abaixo no final do arquivo
    ```
    */1 * * * * /caminho/para/get_stats.sh
    55 23 * * * /caminho/para/generate_report.sh
    ```