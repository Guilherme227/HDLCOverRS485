/*
----------------Trabalho 1 - Comunicação de Dados----------------

Alunos: Guilherme Lopes de Oliveira e Lucas Coelho Garbossa

Uso do protocolo de enlace HDLC sobre o protocolo físico RS485.

Código do Arduino UNO (Mestre)
*/

const unsigned long TIMEOUT_DURATION = 10000;
const byte ACK_BYTE = 0x06;
const byte NAK_BYTE = 0x15;
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


void setup() {
  Serial.begin(9600); // Começa a comuncação serial
}

void loop() {
    //makeSelection();
    //sendHDLCMessage(selectedID, selectedControl, messageToSend);
    sendHDLCMessage(SLAVE_ID_1, CONTROL_4, messageToSend);

    /*if(!waitForACK) {
      sendHDLCMessage(BROADCAST, CONTOL_4, messageToSend);
    }*/

    delay(1000);
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

  uint16_t fcs = calculateFCS(slaveID, control, message); // Calcula o FCS

  Serial.write((byte)(fcs >> 8)); // FCS
  Serial.write((byte)fcs);
  Serial.write(FLAG); // Flag de final de mensagem

  Serial.print("\nFCS calculado: ");
  Serial.println(fcs);
}

void waitForACK() {
  unsigned long startTime = millis();

  while (millis() - startTime < TIMEOUT_DURATION) {
    while (Serial.available() > 0) {
      char receivedChar = Serial.read();

      if (receivedChar == ACK_BYTE) { // ACK
        Serial.println("Mensagem enviada com sucesso!");
      } else if (receivedChar == NAK_BYTE) { // NAK
        Serial.println("NAK recebido!");
      }
    }
  }
  Serial.println("Timeout ao esperar ACK/NAK!"); // Timeout
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