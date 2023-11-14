/*
----------------Trabalho 1 - Comunicação de Dados----------------

Alunos: Guilherme Lopes de Oliveira e Lucas Coelho Garbossa

Uso do protocolo de enlace HDLC sobre o protocolo físico RS485.

Código do Arduino UNO (Mestre)
*/

const unsigned long TIMEOUT_DURATION = 1500;
const byte ACK_BYTE = 0x06;
const byte NAK_BYTE = 0x15;
const uint16_t SLAVE_ID_1 = 1;
const uint16_t SLAVE_ID_2 = 2;
const uint16_t BROADCAST = 3;
//uint16_t selectedSlaveID = 0;
const uint16_t CONTROL = 0x07;
const char* messageToSend = "Hello ESP32!";
const byte FLAG = 0x7E;

void setup() {
  Serial.begin(9600); // Começa a comuncação serial
  //selectedSlaveID = 0; // Inicialmente nenhum escravo selecionado
}

void loop() {
  //if (selectedSlaveID != 0){ // Verifica se o usuário deseja enviar a mensagem
    //sendHDLCMessage(selectedSlaveID, messageToSend);
    sendHDLCMessage(SLAVE_ID_2, CONTROL, messageToSend);
    //selectedSlaveID = 0; 

  /*if (Serial.available() > 0) {
    char userInput = Serial.read();
    if (userInput == '1') {
      selectedSlaveID = SLAVE_ID_1;
    } else if (userInput == '2') {
      selectedSlaveID = SLAVE_ID_2;
    } else if (userInput == 'c' || userInput == 'C') {
      selectedSlaveID = 0;
    }
  }*/
}

void sendHDLCMessage(uint16_t slaveID, uint16_t control, const char* message){
  Serial.write(FLAG); // Flag de inicio de mensagem
  Serial.write((byte)(slaveID >> 8)); // Endereço (byte mais significativo)
  Serial.write((byte)slaveID); // Endereço (byte menos significativo)
  Serial.write((byte)(control >> 8)); // Controle
  Serial.write(control); // Controle
  Serial.write(message); // Mensagem
  Serial.flush(); // Espera toda mensagem ser enviada

  uint16_t fcs = calculateFCS(slaveID, control, message); // Calcula o FCS

  Serial.write((byte)(fcs >> 8)); // FCS
  Serial.write((byte)fcs);
  Serial.write(FLAG); // Flag de final de mensagem

  Serial.print("FCS calculado: ");
  Serial.println(fcs);

  if (waitForACK()) {
    Serial.println("Mensagem enviada com sucesso!");
  } else {
    Serial.println("Falha ao enviar a mensagem!");
  }
}

bool waitForACK() {
  unsigned long startTime = millis();

  while (millis() - startTime < TIMEOUT_DURATION) {
    if (Serial.available() > 0) {
      char receivedChar = Serial.read();

      if (receivedChar == ACK_BYTE) {
        return true;
      } else if (receivedChar == NAK_BYTE) {
        return false;
      }
    }
  }
  return false;
}

uint16_t calculateFCS(uint16_t address, uint16_t control, const char* message) {
  uint16_t fcs = 0;
  fcs ^= address; // XOR do valor atual do fcs com address
  fcs ^= control; // XOR do valor atual do fcs com control

  for (int i = 0; i < strlen(message); i++) {
    fcs ^= message[i];
  }
  return fcs;
}