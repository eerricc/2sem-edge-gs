# **2sem - Global Sulution Edge**

> Este projeto propõe uma solução baseada em IoT para monitoramento e otimização do consumo de energia e água em residências, promovendo sustentabilidade e redução de custos. A arquitetura do sistema integra dispositivos IoT, um backend robusto e aplicações interativas, utilizando tecnologias como ESP32, FIWARE, MQTT, Docker e MongoDB.

> Os dispositivos IoT, liderados pelo microcontrolador ESP32, coletam dados em tempo real de sensores que medem fluxo de água e consumo energético. Esses dados são enviados ao backend, que os processa utilizando o FIWARE e seus componentes, como o Orion Context Broker e o STH Comet, armazenando informações no MongoDB para análise histórica.

> A solução inclui um dashboard que apresenta os dados de forma clara, permitindo aos usuários acompanhar o consumo e identificar padrões para tomar decisões mais conscientes. Com a implementação do sistema, espera-se uma redução de até 30% nos consumos de água e energia, contribuindo para a preservação ambiental e um estilo de vida mais sustentável.

## 📡 🔍 Link do Projeto
Segue codigo do projeto : https://wokwi.com/projects/415265137895265281

## 🛠️ Construído com
+ Wokwi IDE
+ Azure Cloud
  - docker
  - [fiware](https://github.com/fabiocabrini/fiware)
  - python
+ Postman


## Linguagens Utilizadas
![Static Badge](https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&labelColor=black)

![Static Badge](https://img.shields.io/badge/C-A8B9CC?style=for-the-badge&logo=c&labelColor=black)

![Static Badge](https://img.shields.io/badge/python-%233776AB?style=for-the-badge&logo=python&logoColor=%233776AB&labelColor=black&color=%233776AB)




## Diagrama
![2sem-gs-edge-draw](https://github.com/user-attachments/assets/c6bd9b57-ac61-417d-893b-a69ea4f57123)
>O diagrama representa uma arquitetura de sistema IoT onde os dispositivos coletam dados e os enviam para um backend robusto, que processa, armazena e disponibiliza esses dados por meio de diferentes interfaces, como um dashboard e um aplicativo móvel. Essa estrutura permite a visualização e o gerenciamento eficiente das informações coletadas.

## 📖 Mini Guide
> Este guia não demonstra o passo a passo completo; o objetivo é apenas mostrar para o usuário a estrutura GERAL do projeto.

### 1) Criar uma Virtual Machine
- **Instalar**:
  - Docker
  - Docker Compose
  - FIWARE
  - Necessita de um código Python para dashboards

- **Abrir as portas do firewall**:
  - `1026/TCP` - Orion Context Broker
  - `1883/TCP` - Eclipse-Mosquito MQTT
  - `4041/TCP` - IoT-Agent MQTT
  - `8666/TCP` - STH-Comet

### 2) Postman
- **Instalar**:
  - Necessita de uma collection JSON
  - Adaptar as variáveis de acordo com as informações da sua máquina virtual

### 3) Simulação Wokwi ou Hands-On
- Utilize um código base, adaptando as variáveis de acordo com sua preferência e necessidade.

### 4) Guia Completo: https://github.com/fabiocabrini/fiware

#### 5) video no youtube: https://youtu.be/ESTKmMiho9E?si=7AEHoVp3D0QcQ683

## ✒️ Autor
+ Eric Segawa RM558224
