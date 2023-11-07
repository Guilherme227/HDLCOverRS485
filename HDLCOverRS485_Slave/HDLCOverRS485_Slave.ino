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
    String receivedData = Serial.readStringUntil('\n'); // Recebe os dados enviados pelo Mestre
    Serial.print("Recebido: ");
    Serial.println(receivedData); // Printa os dados recebidos
  }
}