/* Modulador Percentual de Largura de Pulso -> MPLP 
 *  versão 1.0 - primeira revisão -              *       
 *  tem saída pwm controlável, porém, com        *
 *  frequências diferentes das previstas         *
**   12/12/19 - por jlnatalicio                ***/


/*-------------------- Biblioteca(s) ---------------------*/

#include <LiquidCrystal.h>

/*--------------------------------------------------------*/

/*---------------------- Hardware ------------------------*/

#define saida 3
#define up 14
#define down 15
#define select 16

const int rs = 6, en = 7, d4 = 8, d5 = 9, d6 = 10, d7 = 11;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

/*--------------------------------------------------------*/

/*----------------------------------------------------- variáveis iniciais ----------------------------------------------*/

int freq,                         // frequência do sinal modulado [Hz]
    freq_selecionada = 1,         // valor que define qual frequencia foi selecionada [1~6]
    ciclo_trabalho,               // valor do ciclo de trabalho do sinal [0~100%]
    duty_cycle,                   // valor do ciclo de trabalho, convertido para ser usado no registrador OCR2B [0~255]
    linha_seta;                   // valor da linha em qual estará a seta de seleção entre Freq. e Pulso

float duty_cycle_aux;             // variável auxiliar a duty_cycle
  
bool flag_up = false,             // flag para indicar que botão "up" foi pressionado
     flag_down = false,           // flag para indicar que botão "down" foi pressionado
     flag_select = false,         // flag para indicar que botão "select" foi pressionado
     seta_selecao = false;        // flag para indicar onde a seta está apontando no display -> [0: Freq; 1: Pulso]

byte seta[8] = {
  B11000,
  B11100,
  B11110,
  B11111,
  B11111,
  B11110,
  B11100,
  B11000,
};

/*------------------------------------------------------------------------------------------------------------------------*/

void setup() {
  // put your setup code here, to run once:

      //definindo entradas e saídas
      pinMode(up,INPUT_PULLUP);
      pinMode(down,INPUT_PULLUP);
      pinMode(select,INPUT_PULLUP);
      pinMode(saida,OUTPUT);

      //definindo configurações do Timer/Counter2 com fast PWM
      TCCR2A = B00100011;             // bits 7 e 6: COM2A1 e COM2A0, respectivamente: 00 -> OC2A disconectado
                                      // bits 5 e 4: COM2B1 e COM2B0, respectivamente: 10 -> Limpa OC2B quando há igualdade na comparação com OCR2 (modo não-inversor)
                                      // bits 3 e 2: bits reservados do microcontrolador (sempre manter em 00!)
                                      // bits 1 e 0: WGM21 e WGM20, respectivamente: 11 -> modo 3 - Fast PWM [0~255]
      TCCR2B = B00000001; 
      

      //inicializando display LCD
      lcd.begin(16, 4);
      lcd.createChar(0, seta);          // criando caracter especial - seta de seleção entre Freq. e Pulso
      lcd.setCursor(0,0);
      lcd.print("     Modulo     ");
      lcd.setCursor(0,1);
      lcd.print("   Percentual   ");
      lcd.setCursor(0,2);
      lcd.print("   de Largura   ");
      lcd.setCursor(0,3);
      lcd.print("    de Pulso    ");

      //inicializando comunicação serial
      Serial.begin(9600);
      Serial.println("Modulador Percentual de Largura de Pulso");
      Serial.println("         MPLP -> versão 1.0             ");
      Serial.println("****************************************");

      //iniciando sistema com valores iniciais (f = 1Hz - D = 50%):
      freq = 1;
      freq_selecionada = 1;
      ciclo_trabalho = 50;
      duty_cycle_aux = (ciclo_trabalho * 255) / 100;
      Serial.print("duty_cycle_aux = ");
      Serial.println(duty_cycle_aux);
      duty_cycle = arredondar(duty_cycle_aux);
      Serial.print("duty_cycle = ");
      Serial.println(duty_cycle);
      OCR2B = byte(duty_cycle);
      Serial.print("OCR2B = ");
      Serial.println(OCR2B);
      Serial.println("****************************************");
      
      delay(4900);
      lcd.clear();
      delay(100);
         
} // end void setup()

void loop() {
  // put your main code here, to run repeatedly:

       Serial.print("duty cycle: ");
       Serial.print(ciclo_trabalho);
       Serial.print("\t frequencia: ");
       Serial.println(freq);

       //tela principal com informações da saída
       lcd.setCursor(0,0);
       lcd.print("    MPLP v1.0   ");
       lcd.setCursor(0,2);
       lcd.print("  Freq.:");
       lcd.setCursor(8,2);
       lcd.print("    ");
       lcd.setCursor(8,2);
       lcd.print(freq);
       
            if (freq_selecionada == 7){
                lcd.setCursor(11,2);
                lcd.print("M");
            }
            else if (freq_selecionada >= 5){
                  lcd.setCursor(11,2);
                  lcd.print("k");
            }
            
       lcd.setCursor(12,2);
       lcd.print("Hz");
       lcd.setCursor(0,3);
       lcd.print("  Pulso:");
       lcd.setCursor(10,3);
       lcd.print("   ");
       lcd.setCursor(10,3);
       lcd.print(ciclo_trabalho);
       lcd.setCursor(13,3);
       lcd.print("%");

            if (!seta_selecao) linha_seta = 2;
            else if (seta_selecao) linha_seta = 3;
            
       lcd.setCursor(0,linha_seta);
       lcd.write(byte(0));

       //leitura dos botões
       if (!digitalRead(up)) flag_up = true;
       delay(21);         // delay para compensar deboucing do botão
       if (digitalRead(up) && flag_up){
       flag_up = false;
       
            if (!seta_selecao) {
                  freq_selecionada++;
                  if (freq_selecionada > 7) freq_selecionada = 7;
            }
            else if (seta_selecao) {
                  ciclo_trabalho++;
                  if (ciclo_trabalho > 100) ciclo_trabalho = 100;

                  duty_cycle_aux = (ciclo_trabalho * 255) / 100;
                  duty_cycle = arredondar(duty_cycle_aux);
                  if (duty_cycle > 255) duty_cycle = 255;
            }
            
       } //end if (digitalRead(up) && flag_up)

       if (!digitalRead(down)) flag_down = true;
       delay(21);         // delay para compensar deboucing do botão
       if (digitalRead(down) && flag_down){
       flag_down = false;
       
            if (!seta_selecao) {
                  freq_selecionada--;
                  if (freq_selecionada < 1) freq_selecionada = 1;
            }
            else if (seta_selecao) {
                  ciclo_trabalho--;
                  if (ciclo_trabalho < 0) ciclo_trabalho = 0;

                  duty_cycle_aux = (ciclo_trabalho * 255) / 100;
                  duty_cycle = arredondar(duty_cycle_aux);
                  if (duty_cycle < 0) duty_cycle = 0;
            }
            
       } //end if (digitalRead(up) && flag_up)

       if (!digitalRead(select)) flag_select = true;
       delay(21);         // delay para compensar deboucing do botão
       if (digitalRead(select) && flag_select){
       flag_select = false;
       
            if (seta_selecao) seta_selecao = false;
            else if (!seta_selecao) seta_selecao = true;
            Serial.print("seta_selecao = ");
            Serial.println(seta_selecao);
            
       } //end if (digitalRead(up) && flag_up)

       switch(freq_selecionada){
              
                  case 1:         // caso freq_selecionada = 1, freq = 1 Hz 
                      freq = 1;
                      //definindo configurações do clock do Timer/Counter2
                      TCCR2B = B00000001;             // bits 7 e 6: FOC2A e FOC2B, respectivamente: 00 -> sempre em 00 para modo PWM ou Fast PWM
                                                      // bits 5 e 4: bits reservados do microcontrolador (sempre manter em 00!)
                                                      // bit 3 : WGM22: 0 - modo 3 Fast PWM
                                                      // bits 2, 1 e 0: CS22, CS21 e CS20, respectivamente: 001 -> clkT2S/1 (sem prescaler)

                      //definindo configurações do duty cuycle do Timer/Counter2
                      OCR2B = duty_cycle;
                      break;               

                  case 2:         // caso freq_selecionada = 2, freq = 60 Hz
                      freq = 60;
                      //definindo configurações do clock do Timer/Counter2
                      TCCR2B = B00000010;             // bits 7 e 6: FOC2A e FOC2B, respectivamente: 00 -> sempre em 00 para modo PWM ou Fast PWM
                                                      // bits 5 e 4: bits reservados do microcontrolador (sempre manter em 00!)
                                                      // bit 3 : WGM22: 0 - modo 3 Fast PWM
                                                      // bits 2, 1 e 0: CS22, CS21 e CS20, respectivamente: 010 -> prescaler = clkT2S/8

                      //definindo configurações do duty cuycle do Timer/Counter2
                      OCR2B = duty_cycle;
                      break;              

                  case 3:         // caso freq_selecionada = 3, freq = 100 Hz
                      freq = 100;
                      //definindo configurações do clock do Timer/Counter2
                      TCCR2B = B00000011;             // bits 7 e 6: FOC2A e FOC2B, respectivamente: 00 -> sempre em 00 para modo PWM ou Fast PWM
                                                      // bits 5 e 4: bits reservados do microcontrolador (sempre manter em 00!)
                                                      // bit 3 : WGM22: 0 - modo 3 Fast PWM
                                                      // bits 2, 1 e 0: CS22, CS21 e CS20, respectivamente: 011 -> prescaler = clkT2S/32

                      //definindo configurações do duty cuycle do Timer/Counter2
                      OCR2B = duty_cycle;
                      break;             

                  case 4:         // caso freq_selecionada = 4, freq = 1 kHz
                      freq = 1000;
                      //definindo configurações do clock do Timer/Counter2
                      TCCR2B = B00000100;             // bits 7 e 6: FOC2A e FOC2B, respectivamente: 00 -> sempre em 00 para modo PWM ou Fast PWM
                                                      // bits 5 e 4: bits reservados do microcontrolador (sempre manter em 00!)
                                                      // bit 3 : WGM22: 0 - modo 3 Fast PWM
                                                      // bits 2, 1 e 0: CS22, CS21 e CS20, respectivamente: 100 -> prescaler = clkT2S/64

                      //definindo configurações do duty cuycle do Timer/Counter2
                      OCR2B = duty_cycle;
                      break;            

                  case 5:         // caso freq_selecionada = 5, freq = 10 kHz
                      freq = 10;
                      //definindo configurações do clock do Timer/Counter2
                      TCCR2B = B00000101;             // bits 7 e 6: FOC2A e FOC2B, respectivamente: 00 -> sempre em 00 para modo PWM ou Fast PWM
                                                      // bits 5 e 4: bits reservados do microcontrolador (sempre manter em 00!)
                                                      // bit 3 : WGM22: 0 - modo 3 Fast PWM
                                                      // bits 2, 1 e 0: CS22, CS21 e CS20, respectivamente: 101 -> prescaler = clkT2S/128

                      //definindo configurações do duty cuycle do Timer/Counter2
                      OCR2B = duty_cycle;
                      break;           

                  case 6:         // caso freq_selecionada = 6, freq = 100 kHz
                      freq = 100;
                      //definindo configurações do clock do Timer/Counter2
                      TCCR2B = B00000110;             // bits 7 e 6: FOC2A e FOC2B, respectivamente: 00 -> sempre em 00 para modo PWM ou Fast PWM
                                                      // bits 5 e 4: bits reservados do microcontrolador (sempre manter em 00!)
                                                      // bit 3 : WGM22: 0 - modo 3 Fast PWM
                                                      // bits 2, 1 e 0: CS22, CS21 e CS20, respectivamente: 110 -> prescaler = clkT2S/8

                      //definindo configurações do duty cuycle do Timer/Counter2
                      OCR2B = duty_cycle;
                      break;

                  case 7:         // caso freq_selecionada = 6, freq = x MHz
                      freq = 100;
                      //definindo configurações do clock do Timer/Counter2
                      TCCR2B = B00000111;             // bits 7 e 6: FOC2A e FOC2B, respectivamente: 00 -> sempre em 00 para modo PWM ou Fast PWM
                                                      // bits 5 e 4: bits reservados do microcontrolador (sempre manter em 00!)
                                                      // bit 3 : WGM22: 0 - modo 3 Fast PWM
                                                      // bits 2, 1 e 0: CS22, CS21 e CS20, respectivamente: 111 -> prescaler = clkT2S/1024

                      //definindo configurações do duty cuycle do Timer/Counter2
                      OCR2B = duty_cycle;
                      break;
                  
            } //end switch(freq_selecionada)
       
} // end void loop()

//algoritmo para arredondamento de valor float
int arredondar( float valor ) {

    int resultado, sobra;                                                     // variáveis usadas pela função
    
    resultado = valor * 100;                                                  // 1 - multiplicar valor da força medida por 100, guardar em 'resultado'
    Serial.print("resultado = ");
    Serial.println(resultado);
    
    sobra = resultado % 100;                                                  // 2 - dividir o valor da força medida por 100, guarda o resto da divisão em 'sobra'
    Serial.print("sobra = ");
    Serial.println(sobra);

      Serial.print("sobra e resultado final = ");
      if (sobra <= 59) {                                                      // 3.1 - se o valor da sobra for menor ou igual a 59 (ou seja, quando o valor fracionario for 0,59 ou menos)
          sobra = 0;                                                          // 3.1.1 - zera o resto, e não altera o resultado
          Serial.println(sobra);
          
      }
      else {                                                                  // 3.2 - se o valor da sobra for maior que 59 (ou seja, quando o valor fracionario for 0,60 ou mais) 
          resultado = resultado + 100;                                        // 3.2.1 - soma 100 ao resultado (ou seja, 1 no valor final)
          sobra = 0;                                                          // 3.2.2 - zera o resto
          Serial.println(sobra);
          Serial.print("resultado calculado = ");
          Serial.println(resultado);
      }

      resultado /= 100;                                                       // 4 - para voltar ao valor "normal", dividir o resultado por 100, e guardar nele mesmo
      Serial.print("resultado final = ");
      Serial.println(resultado);
      
      return resultado;                                                       // 5 - retornar ao código com o valor arredondado
}// end int arredondar( float valor )
