# TCC

# Hardwares utilizados

- Esp32-C3 super mini
- Esp32-C3 super mini com display OLED 0.42
- Sensor DHT22
- Raspberry Pi 4 4GB
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
    - Cole o código do link abaixo no Arduino IDE:
    ```
    https://github.com/MarvinM7/tcc/blob/main/esp32/blinkTest.cc
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
    ```
    https://github.com/MarvinM7/tcc/blob/main/esp32/sensorTest.cc
    ```
    - Clique no botão de seta para a direita(Carregar/Upload) no topo da IDE
    - Veja o log no Serial Monitor do Arduino IDE 
- Adicionando a versão final do arquivo no esp32
    - Cole o código abaixo na sua IDE:
        - Versão com display OLED
        ```
        https://github.com/MarvinM7/tcc/blob/main/esp32/dhtWithOled.cc
        ```
        - Versão sem display
        ```
        https://github.com/MarvinM7/tcc/blob/main/esp32/dhtWithoutPrint.cc
        ```
    - Preencha os seguintes campos:
        - ssid: nome da rede Wi-Fi
        - password: senha da rede Wi-Fi
        - mqtt_server: ip do raspberry pi
        - mqtt_topic: identificador do esp32 (esse mesmo identificador vai ser necessário na configuração do dashboard)
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
    ```
    https://github.com/MarvinM7/tcc/blob/main/nodered/docker-compose.yml
    ```
    - Criar o arquivo mosquitto.conf
    ```
    https://github.com/MarvinM7/tcc/blob/main/nodered/mosquitto.conf
    ```
    - Criar e iniciar o container
    ```
    docker compose up
    ```
    - Criar o banco de dados
    ```
    docker exec -it influxdb_teste influx -execute "CREATE DATABASE tcc_sensores"
    ```

- Configurar o Node-RED
    - Acessar o endereço de um computador na mesma máquina que o raspberry pi
    ```
    http://IP_RASPBERRY:1880
    ```
    - Instalar o @flowfuse/node-red-dashboard
        - Clique no Menu (três linhas horizontais no canto superior direito) e selecione Manage palette (Gerenciar paleta).
        - Ir até a aba Instalar.
        - Digitar na busca: @flowfuse/node-red-dashboard
        - Clicar no botão Instalar ao lado dele. Uma nova coleção de nós chamados "Dashboard" vai aparecer na sua barra lateral esquerda.
    - Instalar o node-red-contrib-influxdb
        - Clique no Menu (três linhas horizontais no canto superior direito) e selecione Manage palette (Gerenciar paleta).
        - Ir até a aba Instalar.
        - Digitar na busca: @flowfuse/node-red-dashboard
        - Clicar no botão Instalar ao lado dele. Uma nova coleção de nós vai aparecer na sua barra lateral esquerda em "Armazenar".
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
- Instalar o ThingsBoard
    - Criar a pasta thingsboard
    - Acessar a pasta thingsboard e criar o arquivo docker-compose.yml
    ```
    https://github.com/MarvinM7/tcc/blob/main/thingsboard/docker-compose.yml
    ```
    - Criar e iniciar o container
    ```
    docker compose up
    ```
    OBS: é necessário garantir que as pastas ./tb-data ./tb-logs tenham permissão total de escrita e que pertençam ao usuário atual. Para isso, rode os comandos:
    ```
    sudo chown -R 799:799 ./tb-data ./tb-logs
    sudo chmod -R 777 ./tb-data ./tb-logs
    ```
    O processo de criação e inicialização do ThingsBoard costuma demorar um pouco.
- Configurar o ThingsBoard
    - Abra o ThingsBoard no navegador
    ```
    http://192.168.1.135:9090
    ```
    - Logue com o perfil de configuração
    ```
    Usuário: tenant@thingsboard.org
    Senha: tenant
    ```
    - No menu esquerdo, vá em Entidades > Dispositivos
    - Clique no botão + e depois em Adicionar Novo Dispositivo
        - Nome: preencha o nome com um de sua escolha
        - Device profile: mantenha default
    - Clique em Adicionar
    - Feche a janela que aparecer
    - Na lista de dispositivos, clique no penúltimo ícone (Gerenciar credenciais) do dispositivo que foi criado
    - Copie o token de acesso exibido na tela (é necessário no arquivo do Esp32 para realizar a conexão)
    - No menu esquerdo, vá em Dashboards
    - Clique no botão + e depois em Criar novo dashboard
        - Nome: preencha o nome com um de sua escolha
    - Clique em Adicionar
    - Clique em Adicionar novo widget e escolha o tipo que serve para seu objetivo
# Gerando relatório
- Criar o arquivo get_stats.sh
```
https://github.com/MarvinM7/tcc/blob/main/log/get_stats.sh
```
- Criar o arquivo generate_report.sh
```
https://github.com/MarvinM7/tcc/blob/main/report/generate_report.sh
```
- Configutar a execução dos scripts
    - Rodar crontab -e
    - Adicionar as linhas abaixo no final do arquivo
    ```
    */1 * * * * /caminho/para/get_stats.sh
    55 23 * * * /caminho/para/generate_report.sh
    ```
- Caso o uso de memória apareca como 0, é necessário ativar os contadores de memória do cgroups.
    - Rodar o comando
    ```
    sudo nano /boot/cmdline.txt
    ```
    - Adicionar a linha abaixo ao final da linha do arquivo. OBS: Não pode ter quebra de linha no arquivo, precisa ser linha única
    ```
    cgroup_enable=cpuset cgroup_enable=memory swapaccount=1
    ```
    - Salvar o arquivo
    - Reiniciar a máquina
    ```
    sudo reboot
    ```