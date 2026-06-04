# TCC

# Hardwares utilizados

- Esp32-C3 super mini
- Sensor DHT22
- Raspberry Pi 4 8GB
- Cartão de memória 128GB

# Ferramentas utilizadas

- Arduino IDE
- Raspberry Pi Imager

# Bibliotecas utilizadas

- DHT sensor library
- PubSubClient
- ArduinoJson

# Conectando o Arduino IDE ao Esp32

- Adicionar a URL do ESP32 na IDE

    - Abra a Arduino IDE.
    - Vá no menu Arduino IDE > Ajustes....
    - No campo URLs Adicionais de Gerenciadores de Placas, cole o seguinte link:
        * `https://espressif.github.io/arduino-esp32/package_esp32_index.json`

- Instalar a placa no Gerenciador
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

- Testando o sensor
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
# Montando o servidor
 
- Montando o raspberry pi
    - Baixe o Raspberry Pi Imager
    - Conecte o cartão de memória no computador (OBS: ele será formatado)
    - Instale o Raspberry Pi OS Lite no cartão de memória utilizando o Raspberry Pi Imager (ative a opção SSH durante a instalação)



    Instalar o docker
   # 1. Atualiza a lista de pacotes do sistema
sudo apt update && sudo apt upgrade -y

# 2. Baixa o script oficial de instalação do Docker
curl -fsSL https://get.docker.com -o get-docker.sh

# 3. Executa o script para instalar o Docker Core
sudo sh get-docker.sh

# 4. Instala o plugin do Docker Compose (necessário para ler o arquivo docker-compose.yml)
sudo apt install docker-compose-plugin -y

sudo usermod -aG docker marvin

newgrp docker


criar pasta nodered
criar o arquivo docker-compose.yml

YAML

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


criar o arquivo mosquitto.conf

listener 1883 0.0.0.0
allow_anonymous true
persistence true
persistence_location /mosquitto/data/

docker compose up


🧩 Passo 1: Construindo o Fluxo no Node-RED
No Node-RED (que você acessa pelo navegador digitando http://192.168.1.135:1880), vamos montar uma sequência simples com 3 nós para capturar, tratar e visualizar esse JSON:

Nó mqtt in (Entrada):

Arraste o nó mqtt in (fica na paleta de rede/network) para o fluxo.

Dê dois cliques nele para configurar.

Em Server, clique no ícone de lápis para adicionar um novo broker:

Name: Mosquitto Local

Server: 192.168.1.135

Port: 1883

Clique em Add.

De volta à tela do nó, preencha o Topic exatamente como no código: quarto-dht22.

Output: Deixe configurado como a String (ou auto-detect).

Clique em Done.

Nó json (Função/Parser):

Seu ESP32 envia os dados empacotados como uma String de texto em formato JSON. O Node-RED precisa converter isso em um Objeto JavaScript manipulável.

Arraste o nó json (paleta parser) e conecte a saída do nó mqtt in na entrada dele. Ele faz a conversão de String para Objeto de forma automática.

Nó debug (Visualização):

Arraste o nó debug (paleta common, com o nome de msg.payload).

Conecte a saída do nó json na entrada do nó debug.

🚀 Passo 2: Validando o Recebimento dos Dados
Clique no botão vermelho Deploy no canto superior direito do Node-RED para ativar o fluxo.

Se a configuração com o Mosquitto estiver correta (e com as permissões de allow_anonymous true aplicadas no Raspberry Pi), você verá uma pequena etiqueta verde escrito "connected" logo abaixo do nó mqtt in.

Abra a aba de debug lateral (ícone de uma barata/inseto no canto direito).

Assim que o ESP32 rodar o loop() e publicar, você verá a estrutura printada na tela do Node-RED a cada 5 segundos assim:


🛠️ Passo 1: Instalar a Paleta do Dashboard 2.0
Se você ainda não tem os nós de gráfico instalados no seu Node-RED:

Abra o Node-RED no navegador (http://192.168.1.135:1880).

Clique no Menu (três linhas horizontais no canto superior direito) e selecione Manage palette (Gerenciar paleta).

Vá até a aba Install.

Digite na busca: @flowfuse/node-red-dashboard

Clique no botão Install ao lado dele e confirme. Uma nova coleção de nós azuis chamados "Dashboard" vai aparecer na sua barra lateral esquerda.

🧩 Passo 2: Estruturando o Fluxo (Mapeando o JSON)
Como o ESP32 envia um único objeto JSON contendo todas as variáveis, nós precisamos "separar" esses valores para que cada gráfico receba apenas o seu respectivo número.

O fluxo completo vai ficar assim:

Nó mqtt in: Configurado no tópico quarto-dht22 e apontando para o seu container do Mosquitto.

Nó json: Conectado na saída do MQTT para transformar o texto em objeto.

Nós change (Os Separadores): Arraste três nós do tipo Change (paleta function) para separar os dados:

Change 1 (Temperatura): Renomeie para "Filtra Temp" e configure para: Set msg.payload to msg.payload.temperatura

Change 2 (Umidade): Renomeie para "Filtra Umid" e configure para: Set msg.payload to msg.payload.umidade

Change 3 (Heap Livre): Renomeie para "Filtra Heap" e configure para: Set msg.payload to msg.payload.heap_livre

Conecte a saída do único nó json na entrada dos três nós change ao mesmo tempo.

📊 Passo 3: Adicionando os Componentes Visuais
Agora vamos ligar as saídas dos nós change nos componentes gráficos correspondentes:

Para a Temperatura (Gauge / Ponteiro):

Arraste um nó ui-gauge e conecte-o na saída do Change 1.

Dê dois cliques nele. Na primeira vez, ele vai pedir para criar um Group (Grupo) e uma Page (Página). Clique em adicionar de forma automática (pode chamar a página de "Home" e o grupo de "Quarto").

Configure o Label para Temperatura, o Min para 0 e o Max para 50. Mude as cores para verde/amarelo/vermelho se quiser.

Para a Umidade (Chart / Gráfico de Linha):

Arraste um nó ui-chart e conecte-o na saída do Change 2.

Configure o Group para o mesmo grupo "Quarto".

Mude o Label para Histórico de Umidade, o tipo para Line Chart e defina para guardar os dados das últimas 1 ou 2 horas.

Para o Heap Livre (Text / Informação Pura):

Arraste um nó ui-text e conecte-o na saída do Change 3.

Configure o Label para Memória RAM Livre (Bytes) e no campo Value deixe {{msg.payload}}. Isso vai exibir o número bruto para o seu monitoramento do TCC.

🚀 Passo 4: Dar o Deploy e Acessar a Interface
Clique no botão vermelho Deploy no canto superior direito do Node-RED.

Para abrir o seu painel bonito e limpo, abra uma nova aba no seu navegador e digite o IP do seu Raspberry seguido de /dashboard:

Plaintext
http://192.168.1.135:1880/dashboard