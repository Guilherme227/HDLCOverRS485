/*
----------------Trabalho 1 - Comunicação de Dados----------------

Alunos: Guilherme Lopes de Oliveira e Lucas

Uso do protocolo de enlace HDLC sobre o protocolo físico RS485.

Código do ESP32-1 (Escravo)
*/
void setup() {
  Serial.begin(115200);
}

void loop() {
  //Serial.println("Hello, World!");
  //delay(1000);

  if (Serial.available()) {
    char receivedChar = Serial.read(); // Lê um caracter
    if (receivedChar == '>') { // Verifica se o caracter é o de inicio de mensagem
      String receivedData = "";
      while (Serial.available()) {
        char dataChar = Serial.read();
        if (dataChar == '<') { // Verifica caracter a caracter se é o de fim de mensagem
          Serial.print("Recebido: ");
          Serial.println(receivedData);
          break;
        }
        receivedData += dataChar;
      }
    }
  }
}