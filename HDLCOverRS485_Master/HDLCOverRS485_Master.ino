/*
----------------Trabalho 1 - Comunicação de Dados----------------

Alunos: Guilherme Lopes de Oliveira e Lucas

Uso do protocolo de enlace HDLC sobre o protocolo físico RS485.

Para o projeto estamos utilizando o Arduino UNO como mestre e dois ESP32 como escravos. Por uma limitação de hardware,
o Arduino só possui um par RX/TX, decidimos utilizar a biblioteca <ATtinySerialOut.h> que permite criar pinos RX/TX via software.

Código do Arduino UNO (Mestre)
*/
//#include <ATtinySerialOut.h>

//SoftwareSerial esp32Serial1(2,3); // RX, TX via software para o primeiro ESP32
//SoftwareSerial esp32Serial2(4,5); // RX, TX via software para o segundo ESP32

void setup() {
  Serial.begin(115200); // Começa a comuncação serial com o Mestre

  //esp32Serial1.begin(115200); // Começa a comunicação serial com o Escravo 1
}

void loop() {
  //esp32Serial1.print("Olá, ESP32-1!"); // Envio de dados do Mestre ao Escravo 1
  char dataToSend[] = "Olá, ESP32!";
  Serial.println(dataToSend);

  /*while (esp32Serial1.available()) {
    char receivedChar = esp32Serial1.read(); // Recebe os dados do Escravo 1
    Serial.print(receivedChar); // Printa os dados do Escravo 1
  }*/
  while (Serial.available()) {
    char receivedChar = Serial.read();
    Serial.print(receivedChar);
  }
}