/*
----------------Trabalho 1 - Comunicação de Dados----------------

Alunos: Guilherme Lopes de Oliveira e Lucas Coelho Garbossa

Uso do protocolo de enlace HDLC sobre o protocolo físico RS485.

Código do Arduino UNO (Mestre)
*/

const unsigned long TIMEOUT_DURATION = 5000;
const char ACK_BYTE = 6;
const char NAK_BYTE = 7;
const byte FLAG = 0x7E;
const uint16_t SLAVE_ID_1 = 1;
const uint16_t SLAVE_ID_2 = 2;
const uint16_t BROADCAST = 3;
const uint16_t CONTROL_1 = 1; // LED verde
const uint16_t CONTROL_2 = 2; // LED amarelo
const uint16_t CONTROL_3 = 3; // LED vermelho
const uint16_t CONTROL_4 = 4; // Todos LEDs
uint16_t selectedID;
uint16_t selectedControl;
const char* messageToSend = "Hello ESP32!";
bool firstMessage = false;
bool secondMessage = false;


void setup() {
  Serial.begin(9600); // Começa a comuncação serial
}

void loop() {
    //makeSelection();
    //sendHDLCMessage(selectedID, selectedControl, messageToSend);
    //sendHDLCMessage(SLAVE_ID_2, CONTROL_1, messageToSend);
    if(!firstMessage){
      sendHDLCMessage (BROADCAST, CONTROL_4, messageToSend);
      firstMessage = true;
    }
    /*switch(waitForACK()) {
      case 1:
        Serial.println("Mensagem enviada com sucesso!");
      break;

      case 2:
        Serial.println("NAK recebido!"); // NAK recebido
        Serial.println("Enviando novamente:");
        sendHDLCMessage(BROADCAST, CONTROL_4, messageToSend);
      break;

      case 3:
        Serial.println("Timeout ao esperar ACK/NAK");
        Serial.println("Enviando novamente:");
        sendHDLCMessage(BROADCAST, CONTROL_4, messageToSend);
      break;

      default:
        Serial.println("Algo deu errado!");
      break;
    }*/
    if(waitForACK() == 1) { // ACK recebido
      Serial.println("Mensagem enviada com sucesso!");
      secondMessage = true;
    }
    if(waitForACK() == 2) { // NAK recebido
      if(!secondMessage) {
        Serial.println("NAK recebido!"); // NAK recebido
        Serial.println("Enviando novamente:");
        sendHDLCMessage(BROADCAST, CONTROL_4, messageToSend);
      }
    }
    if(waitForACK() == 3) { // Timeout
      if(!secondMessage) {
        Serial.println("Timeout ao esperar ACK/NAK");
        Serial.println("Enviando novamente:");
        sendHDLCMessage(BROADCAST, CONTROL_4, messageToSend);  
      }
    }
    delay(100);
}

void makeSelection() {
  // Limpa o buffer
  while (Serial.available() > 0) {
    Serial.read();
  }

  // Escolher a ID
  Serial.println("Escolha o ID:");
  Serial.println("1 - Escravo 1");
  Serial.println("2 - Escravo 2");
  Serial.println("3 - Broadcast");

  while (!Serial.available()) {
    // Aguarda interação do usuário
  }

  selectedID = Serial.parseInt();

  // Limpa o buffer novamente
  while (Serial.available() > 0) {
    Serial.read();
  }

  Serial.print("ID escolhida: ");
  Serial.println(selectedID);
  delay(1000);

  // Escolher o controle
  Serial.println("Escolha o valor de controle:");
  Serial.println("1 - Pisca LED Verde");
  Serial.println("2 - Pisca LED Amarelo");
  Serial.println("3 - Pisca LED Vermelho");
  Serial.println("4 - Pisca todos os LEDs");
  Serial.println("5 - Controle inválido para teste");

  while (!Serial.available()) {
    // Aguarda interação do usuário
  }

  selectedControl = Serial.parseInt();

  // Limpa o buffer novamente
  while (Serial.available() > 0) {
    Serial.read();
  }

  Serial.print("Controle escolhido: ");
  Serial.println(selectedControl);
  delay(1000);

  Serial.println("Confirma?");
  Serial.println("1 - Confirmar");
  Serial.println("2 - Cancelar");
  
  while (!Serial.available()) {
    // Aguarda interação do usuário
  }

  if (Serial.parseInt() == 2) {
    makeSelection();
    return;
  }
}

void sendHDLCMessage(uint16_t slaveID, uint16_t control, const char* message){
  Serial.write(FLAG); // Flag de inicio de mensagem
  Serial.write((byte)(slaveID >> 8)); // Endereço (byte mais significativo)
  Serial.write((byte)slaveID); // Endereço (byte menos significativo)
  Serial.write((byte)(control >> 8)); // Controle
  Serial.write(control); // Controle
  Serial.write(message); // Mensagem
  Serial.flush(); // Espera toda mensagem ser enviada

  uint16_t crc = calculateCRC(slaveID, control, message); // Calcula o FCS

  Serial.write((byte)(crc >> 8)); // FCS
  Serial.write((byte)crc);
  Serial.write(FLAG); // Flag de final de mensagem

  Serial.print("\nCRC calculado: ");
  Serial.println(crc);
}

int waitForACK() {
  unsigned long startTime = millis();
  bool receivedResponse = false;

  while (millis() - startTime < TIMEOUT_DURATION) { // Comparação para saber se deu o tempo de timeout
    while (Serial.available() > 0) {
      char receivedChar = Serial.read();

      //Serial.print("Recebido: ");
      //Serial.println(receivedChar);

      if (receivedChar == ACK_BYTE) { // ACK
        if(!receivedResponse) {
          //Serial.println("Recebi ACK");
          //Serial.println("Mensagem enviada com sucesso!");
          receivedResponse = true;
          return 1;
        }
      } else if (receivedChar == NAK_BYTE) { // NAK
          if(!receivedResponse) {
            //Serial.println("Recebi NAK");
            //Serial.println("NAK recebido!");
            receivedResponse = true;
            return 2;
          }
        }
    }
  }
  if (millis() - startTime >= TIMEOUT_DURATION) { // Timeout
    if(!receivedResponse) {
      //Serial.println("Timeout ao esperar ACK/NAK!");
      return 3;
    }
  }
}

uint16_t calculateCRC(uint16_t address, uint16_t control, const char* message) {
  uint16_t crc = 0;
  crc ^= address; // XOR do valor atual do fcs com address
  crc ^= control; // XOR do valor atual do fcs com control

  for (int i = 0; i < strlen(message); i++) {
    crc ^= message[i];
  }
  return crc;
}