/*
----------------Trabalho 1 - Comunicação de Dados----------------

Alunos: Guilherme Lopes de Oliveira e Lucas

Uso do protocolo de enlace HDLC sobre o protocolo físico RS485.

Código do ESP32 (Escravo)
*/

const byte ACK_BYTE = 0x06;
const byte NAK_BYTE = 0x15;
const uint16_t SLAVE_ID = 2;
bool ackSent = false;
bool nakSent = false;
const uint FLAG = 0x7E;

#define LED 2

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
}

void loop() {
  
  delay(500);
  digitalWrite(LED,HIGH);
  delay(500);
  digitalWrite(LED,LOW);

  receiveHDLCMessage();
}

void receiveHDLCMessage() {
  String receivedData = "";
  uint16_t slaveID; // Endereço recebido
  uint16_t control; // Controle recebido
  String messageData = "";
  const int HEADER_LENGTH = 4; // Tamanho do cabeçalho (Endereço + Controle)
  bool inMessage = false; // Indicador se está dentro da mensagem
  uint16_t receivedFCS; // FCS recebido

  while (Serial.available()) {
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

          if (slaveID == SLAVE_ID) { // Verifica se é o mesmo ID do Escravo
          control = (receivedData[2] << 8) | receivedData[3];

          messageData = receivedData.substring(HEADER_LENGTH, receivedData.length() - 2); // Separa a mensagem recebida
          
          receivedFCS = (receivedData[receivedData.length() - 2] << 8) | receivedData[receivedData.length() - 1]; // Separa p FCS recebido

          if (validateFCS(slaveID, control, messageData, receivedFCS)) { // Calcula o FCS com os dados recebidos e compara
            processReceivedData(slaveID, control, messageData); // Processa a mensagem

            if(!ackSent) { // Envia a confirmação caso não tenha sido enviada anteriormente
              sendACK();
              delay(500);
            }
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
  }

  Serial.print("FCS Calculado: ");
  Serial.println(calculatedFCS);
  Serial.print("FCS Recebido: ");
  Serial.println(receivedFCS);

  return calculatedFCS == receivedFCS;
}

void processReceivedData(uint16_t slaveID, uint16_t control, const String& data) {
  Serial.print("ID do Escravo: ");
  Serial.println(slaveID);
  Serial.print("Controle: ");
  Serial.println(control);
  Serial.print("Dados Recebidos: ");
  Serial.println(data);
}

void sendACK () {
  Serial.print(ACK_BYTE);
}

void sendNAK() {
  Serial.print(NAK_BYTE);
}