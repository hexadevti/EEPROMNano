/* =========================================================================================================

   WR Kits vídeo aula de Engenharia Eletrônica (a clássica de sexta) número 291

   Gravador de EEPROM com Arduino

   Compilador: Arduino IDE 1.8.4

   Autor: Eng. Wagner Rambo
   Data: Setembro de 2019
   
   Adaptação do código original do autor Ben Eater
   Link: https://www.youtube.com/watch?v=K88pgWhEb1M 


========================================================================================================= */

 
// =========================================================================================================
// --- Mapeamento de Hardware ---
#define    shift_data     10    //dados do shift register 74HC595
#define    shift_latch    11    //latch do shift register 74HC595
#define    shift_clk      12    //clock do shift register 74HC595
#define    EEPROM_D0       2    //bit menos significativo de dados da EEPROM
#define    EEPROM_D7       9    //bit mais significativo de dados da EEPROM
#define    write_en       13    //sinal de escrita para EEPROM


// =========================================================================================================
// --- Protótipo das Funções ---
void setAddress(int address);               //função para seleção do endereço
byte readEEPROM(int address);               //função para leitura da EEPROM
byte writeEEPROM(int address, byte data);   //função para escrita da EEPROM
void printData();                           //função para imprimir os dados no monitor serial
void eraseEEPROM();                         //função para apagar EEPROM


// =========================================================================================================
// --- Preencha o vetor com os dados que deseja escrever na EEPROM ---
byte data[] = {0xAB, 0x5E, 0x6F, 0xB4, 0xEC, 0x88, 0x25, 0x3D, 
               0xC0, 0x12, 0x55, 0x79, 0xBC, 0xAA, 0x11, 0x99};
 

// =========================================================================================================
// --- Configurações Iniciais ---
void setup() 
{
   pinMode(shift_latch, OUTPUT);   //saída para latch
   pinMode(shift_data,  OUTPUT);   //saída para dados
   pinMode(shift_clk,   OUTPUT);   //saída para clock

   digitalWrite(write_en, HIGH);   //pullup interno em write_en
   pinMode(write_en, OUTPUT);      //saída para write_en

   Serial.begin(115200);           //inicializa Serial em 250000 bits por segundo


   //eraseEEPROM();                  //apaga EEPROM


   for(int address = 0; address <= 15; address += 1)
    writeEEPROM(address, data[address]);

   printData();                    //imprime o conteúdo da EEPROM no monitor serial
  

} //end setup


// =========================================================================================================
// --- Loop Infinito ---
void loop() 
{
  
   //nenhum processamento em loop infinito
   
} //end loop


// =========================================================================================================
// --- Desenvolvimento das Funções ---


void setAddress(int address, bool outEnable)
{
   // Seleciona os 3 bits mais significativos de endereço com bit outEnable
   // Serial.print("Address1: "); Serial.print(address, BIN); Serial.println();
   // Serial.print("Address2: "); Serial.print((address >> 8) , BIN); Serial.println();
   shiftOut(shift_data, shift_clk, MSBFIRST, (address >> 8) | (outEnable ? 0x00 : 0x80));
   
   //Seleciona os 8 bits menos significativos de endereço
   shiftOut(shift_data, shift_clk, MSBFIRST, address);

   //gera pulso de latch para escrever dados nas saídas dos shift registers
   digitalWrite(shift_latch,  LOW);
   digitalWrite(shift_latch, HIGH);
   digitalWrite(shift_latch,  LOW);
  
} //end setAddress


byte readEEPROM(int address)
{
   //configura pinos de dados como entrada
   for(int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1)
    pinMode(pin, INPUT);
     
   setAddress(address, true);   //seleciona endereço para leitura
   
   byte data = 0;               //variável local para armazenar dados
   
   //realiza a leitura dos dados
   for(int pin = EEPROM_D7; pin >= EEPROM_D0; pin -=1)
    data = (data << 1) + digitalRead(pin);
    
   return data;                 //retorna o dado lido
  
} //end readEEPROM


byte writeEEPROM(int address, byte data)
{
   //configura os pinos de dados como saída
   for(int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1)
    pinMode(pin, OUTPUT);
    
   setAddress(address, false);   //seleciona endereço para escrita

   //envia os dados para as saídas
   for(int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1)
   {
      digitalWrite(pin, data & 1);
      data = data >> 1;
    
   } //end for

   //gera o pulso de escrita
   digitalWrite(write_en, LOW);
   delayMicroseconds(1);
   digitalWrite(write_en, HIGH);
   delay(10);
  
} //end writeEEPROM


void printData()
{
   Serial.println("Reading Data ...");
   //imprime os primeiros 32752 endereços de dados
   for(unsigned int base = 0; base <= 32767; base += 16)
   {
     uint16_t data[16];
     
     for(int offset = 0; offset <= 15; offset += 1){
      data[offset] = readEEPROM(base + offset);
     }
  
   char buf[80];
     sprintf(buf, "%04x: %02x %02x %02x %02x %02x %02x %02x %02x  %02x %02x %02x %02x %02x %02x %02x %02x",
       base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], 
       data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

     Serial.println(buf);
 
   } //end for
  
} //end printData


void eraseEEPROM()
{
 
   for(unsigned int address = 0; address <= 32767; address += 1) {
      writeEEPROM(address, 0xFF);                          
      if ((address+1) % 128 == 0) {
        Serial.println((String)((float)(address+1) / 32767 * 100) + "%");
      }

   }
  
} 



















