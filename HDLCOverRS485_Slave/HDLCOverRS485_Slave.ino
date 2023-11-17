/*
----------------Trabalho 1 - Comunicação de Dados----------------

Alunos: Guilherme Lopes de Oliveira e Lucas Coelho Garbossa

Uso do protocolo de enlace HDLC sobre o protocolo físico RS485.

Código do ESP32 (Escravo)
*/

#define GREEN_LED_PIN 26
#define YELLOW_LED_PIN 25
#define RED_LED_PIN 27

const char ACK_BYTE = 6;
const char NAK_BYTE = 7;
const byte FLAG = 0x7E;
const uint16_t SLAVE_ID = 1;
const uint16_t BROADCAST = 3;
bool ackSent = false;
bool nakSent = false;


void setup() {
  Serial.begin(9600);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
}

void loop() { 
  receiveHDLCMessage();
  delay(1000);
}

void receiveHDLCMessage() {
  String receivedData = "";
  uint16_t slaveID; // Endereço recebido
  uint16_t control; // Controle recebido
  String messageData = "";
  const int HEADER_LENGTH = 4; // Tamanho do cabeçalho (Endereço + Controle)
  bool inMessage = false; // Indicador se está dentro da mensagem
  uint16_t receivedFCS; // FCS recebido
  ackSent = false;
  nakSent = false;

  while (Serial.available() > 0) {
    char receivedChar = Serial.read(); // Lê caracter a caracter do pacote recebido
    //Serial.print("Caracter lido: " + receivedChar);

    if (receivedChar == FLAG){ // Verifica se o que recebeu é igual a Flag
      //Serial.print("Flag recebida!");

      if(!inMessage) {
        receivedData = ""; // Caso seja o inicio de um pacote, limpa o buffer
        inMessage = true; // Atualiza a variável para indicar que iniciou a leitura de um pacote
      } else {
        inMessage = false; // Caso seja o fim de um pacote, atualiza a variável de indicação

        if (receivedData.length() >= HEADER_LENGTH) { // Verifica se o tamanho é o suficiente para formar o cabeçalho (Endereço + Controle)
          slaveID = (receivedData[0] << 8) | receivedData[1]; // Separa o endereço do cabeçalho

          if (slaveID == SLAVE_ID || slaveID == BROADCAST) { // Verifica se é o mesmo ID do Escravo ou se é um broadcast
            control = (receivedData[2] << 8) | receivedData[3];

            messageData = receivedData.substring(HEADER_LENGTH, receivedData.length() - 2); // Separa a mensagem recebida
          
            receivedFCS = (receivedData[receivedData.length() - 2] << 8) | receivedData[receivedData.length() - 1]; // Separa p FCS recebido

            if (validateFCS(slaveID, control, messageData, receivedFCS)) { // Calcula o FCS com os dados recebidos e compara
              processReceivedData(slaveID, control, messageData); // Processa a mensagem

              if(!ackSent && !nakSent) { // Envia a confirmação caso não tenha sido enviada anteriormente
                sendACK();
              }
            } else if(!nakSent && !ackSent) {
              sendNAK();
            }
          }
        }
      }
    } else if (inMessage) {
      receivedData += receivedChar; // Armazena os bytes lidos para formar o pacote
    }
  }
}

bool validateFCS(uint16_t address, uint16_t control, const String& data, uint16_t receivedFCS) {
  uint16_t calculatedFCS = 0;
  calculatedFCS ^= address;
  calculatedFCS ^= control;

  for (int i = 0; i < data.length(); i++) {
    calculatedFCS ^= data[i];
    //calculatedFCS += 1; // Força NAK
  }

  Serial.print("\nFCS Calculado: ");
  Serial.println(calculatedFCS);
  Serial.print("FCS Recebido: ");
  Serial.println(receivedFCS);

  return calculatedFCS == receivedFCS;
}

void executeControlCode(uint16_t control) {
  switch (control) {
    case 1:
      for (int i = 1; i <= 3; i++){
        digitalWrite(GREEN_LED_PIN, HIGH);
        delay(500);
        digitalWrite(GREEN_LED_PIN, LOW);
        delay(500);
      }
      break;
    
    case 2:
      for (int i = 1; i <= 3; i++){
        digitalWrite(YELLOW_LED_PIN, HIGH);
        delay(500);
        digitalWrite(YELLOW_LED_PIN, LOW);
        delay(500);
      }
      break;

    case 3:
      for (int i = 1; i <= 3; i++){
        digitalWrite(RED_LED_PIN, HIGH);
        delay(500);
        digitalWrite(RED_LED_PIN, LOW);
        delay(500);
      }
      break;
    
    case 4:      
      digitalWrite(GREEN_LED_PIN, HIGH);
      delay(500);
      digitalWrite(GREEN_LED_PIN, LOW);

      digitalWrite(YELLOW_LED_PIN, HIGH);
      delay(500);
      digitalWrite(YELLOW_LED_PIN, LOW);

      digitalWrite(RED_LED_PIN, HIGH);
      delay(500);
      digitalWrite(RED_LED_PIN, LOW);
      break;

    default:
      Serial.println("Código de controle inválido. Enviando NAK.");
      if(!nakSent && !ackSent) {
        sendNAK();
      }
      break;
  }
}

void processReceivedData(uint16_t slaveID, uint16_t control, const String& data) {
  Serial.print("ID do Escravo: ");
  Serial.println(slaveID);
  Serial.print("Controle: ");
  Serial.println(control);
  Serial.print("Dados Recebidos: ");
  Serial.println(data);

  executeControlCode(control);
}

void sendACK () {
  Serial.println("Enviando ACK");
  Serial.print(ACK_BYTE);
  ackSent = true;
  delay(500);
}

void sendNAK() {
  Serial.println("Enviando NAK");
  Serial.print(NAK_BYTE);
  nakSent = true;
  delay(500);
}