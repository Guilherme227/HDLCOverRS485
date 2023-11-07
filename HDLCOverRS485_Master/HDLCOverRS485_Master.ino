/*
----------------Trabalho 1 - Comunicação de Dados----------------

Alunos: Guilherme Lopes de Oliveira e Lucas

Uso do protocolo de enlace HDLC sobre o protocolo físico RS485.

Para o projeto estamos utilizando o Arduino UNO como mestre e dois ESP32 como escravos. Por uma limitação de hardware,
o Arduino só possui um par RX/TX, decidimos utilizar a biblioteca <ATtinySerialOut.h> que permite criar pinos RX/TX via software.

Código do Arduino UNO (Mestre)
*/

const unsigned long TIMEOUT_DURATION = 1000;
const byte ACK_BYTE = 0x06;
const byte NAK_BYTE = 0x15;

void setup() {
  Serial.begin(115200); // Começa a comuncação serial
}

void loop() {
  const char* messageToSend = "Hello, ESP32!";
  sendHDLCMessage(messageToSend);
}

void sendHDLCMessage(const char* message){
  Serial.write(0x7E); // Flag de inicio de mensagem
  Serial.write(message);
  Serial.flush();
  Serial.write(0x7E); // Flag de final de mensagem

  if (waitForACK()) {
    Serial.println("Mensagem enviada com sucesso!");
  } else {
    Serial.println("Falha ao enviar a mensagem!");
  }
}

bool waitForACK() {
  unsigned long startTime = millis();

  while (millis() - startTime < TIMEOUT_DURATION) {
    if (serial.available() > 0) {
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