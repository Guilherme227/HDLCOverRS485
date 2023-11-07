/*
----------------Trabalho 1 - Comunicação de Dados----------------

Alunos: Guilherme Lopes de Oliveira e Lucas

Uso do protocolo de enlace HDLC sobre o protocolo físico RS485.

Código do ESP32-1 (Escravo)
*/

const byte ACK_BYTE = 0x06;
const byte NAK_BYTE = 0x15;

void setup() {
  Serial.begin(115200);
}

void loop() {
  receiveHDLCMessage();
}

void receiveHDLCMessage() {

  String receivedData = "";
  bool messageReceivedSuccessfully = false;

  while (Serial.available() > 0) {
    char receivedChar = Serial.read();
    receivedData += receivedChar;

    if (receivedChar == 0x7E) {
      if (!receivedData.isEmpty()) {
        messageReceivedSuccessfully = processReceivedData(receivedData);
      }
      receivedData = "";
    }
  }

  if (messageReceivedSuccessfully) {
    sendACK();
  } else {
    sendNAK();
  }
}

bool processReceivedData(const String& data) {
  Serial.println("Recebido: " + data);
  return true;
}

void sendACK () {
  Serial.write(ACK_BYTE);
}

void sendNAK() {
  Serial.write(NAK_BYTE);
}