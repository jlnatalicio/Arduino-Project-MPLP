/* Modulador Percentual de Largura de Pulso -> MPLP 
 *  versão 2.1 - segunda revisão -              *       
**   10/07/20 - por jlnatalicio                ***/

/*
 * Obs: f(ocr2pwm) = f(clock)      16000000
 *                   -------- = --------------  
 *                   N * 256    [1~1024] * 256
 *                   
 * TABELA DE FREQUÊNCIAS DISPONÍVEIS
 * |-------------------|
 * | N    | f(ocr2pwm) |
 * |-------------------|
 * | 1    | 62,5 khz   |
 * | 8    | 7,81 khz   |
 * | 32   | 1,95 khz   |
 * | 64   | 976,5 hz   |
 * | 128  | 488,3 hz   |
 * | 256  | 244,1 hz   |
 * | 1024 | 61,03 hz   |
 * |-------------------|
*/


/*-------------------- Biblioteca(s) ---------------------*/

#include <LiquidCrystal.h>

/*--------------------------------------------------------*/

/* ------------------------ Protótipo de Funções -------------------------- */
int lerBotoes();
void selFreq(char opcao);
void selDuty(float dutyCycle);
/*--------------------------------------------------------------------------*/

/*---------------------- Hardware ------------------------*/
#define up A0
#define down A1
#define select A2
#define saida 3

const int rs = 6, en = 7, d4 = 8, d5 = 9, d6 = 10, d7 = 11;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
/*--------------------------------------------------------*/

/*----------------------------------------------------- variáveis globais ----------------------------------------------*/

int freq = 1,
    duty = 50;

char   botao,
       linha = 2,
       linhaAtual = 2,
       linhaAnterior = 0,
       upFlag = 0,
       downFlag = 0,
       selFlag = 0,
       limSup = 2,
       limInf = 3,
       toggle = 0;
/*------------------------------------------------------------------------------------------------------------------------*/

void setup() {
  // put your setup code here, to run once:

      // inicializando botões em 1 e saída em 0
      pinMode(up, INPUT_PULLUP);
      pinMode(down, INPUT_PULLUP);
      pinMode(select, INPUT_PULLUP);
      pinMode(saida, OUTPUT);
      digitalWrite(saida, LOW);
  
      // inicializando display LCD
      lcd.begin(16, 4);
      lcd.setCursor(0,0);
      lcd.print("     Modulo     ");
      lcd.setCursor(0,1);
      lcd.print("   Percentual   ");
      lcd.setCursor(0,2);
      lcd.print("   de Largura   ");
      lcd.setCursor(0,3);
      lcd.print("    de Pulso    ");

      // inicializando comunicação serial
      Serial.begin(9600);
      Serial.println("Modulador Percentual de Largura de Pulso");
      Serial.println("         MPLP -> versão 2.1             ");
      Serial.println("****************************************");

      cli();
      
      TCCR2A = 0;
      TCCR2B = 0;
      TCNT2 = 0;
      
      OCR2B = 128;
      TCCR2A |= (1<<COM2A1) | (1<<COM2B1) | (1<<WGM21) | (1<<WGM20);
      TCCR2B |= (1<<CS20);
      //TCCR2A = B10100011;
      //selFreq(3);
      
      sei();
      
      delay(4000);
      lcd.clear();
      delay(50);
         
} // end void setup()

void loop() {
  // put your main code here, to run repeatedly:

       selDuty(duty);

       //tela principal com informações da saída
       lcd.setCursor(0,0);
       lcd.print("    MPLP v2.1   ");
       lcd.setCursor(2,2);
       lcd.print("Freq.:");
       lcd.setCursor(8,2);
       lcd.print(freq);
       lcd.setCursor(0,3);
       lcd.print("  Pulso:");
       lcd.setCursor(8,3);
       lcd.print(duty);
       lcd.setCursor(0,linhaAtual);
       lcd.write(">");

       botao = lerBotoes();

       switch (botao) {
            case 1:
                  linhaAnterior = linha;
                  lcd.setCursor(0,linhaAnterior);
                  lcd.print(" ");
                  delay(50);
                  linha--;
                  if (linha < limSup) linha = limInf;
                  linhaAtual = linha;
            break;
            
            case 2:
                  linhaAnterior = linha;
                  lcd.setCursor(0,linhaAnterior);
                  lcd.print(" ");
                  delay(50);
                  linha++;
                  if (linha > limInf) linha = limSup;
                  linhaAtual = linha;
            break;
            
            case 3:
                  if (linhaAtual == 2) {
                        freq++;
                        if (freq > 7) freq = 1;
                        selFreq(freq);
                  }
                  else if (linhaAtual == 3) {
                            duty++;
                            if (duty > 100) duty = 1;
                            selDuty(duty);
                  }
            break;

            default:
            break;
       } // end switch(botao)
}

int lerBotoes() {
  
    int button;
    
    if (!digitalRead(up)) upFlag = 1;
    else if (!digitalRead(down)) downFlag = 1;
    else if (!digitalRead(select)) selFlag = 1;
    else return 0;
    delay(50); 

    if (digitalRead(up) && upFlag) {
        upFlag = 0;
        button = 1;
    }
    else if (digitalRead(down) && downFlag) {
             downFlag = 0;
             button = 2;
    }
    else if (digitalRead(select) && selFlag) {
             selFlag = 0;
             button = 3;
    } else return 0;

    return button;
    
}// end lerBotoes()

void selDuty(float dutyCycle) {
  
      int cicloTrabalho;
      float val;

      dutyCycle /= 100;

      val = (dutyCycle * 256) - 1;
      cicloTrabalho = arredondar(val);
      Serial.print("cicloTrabalho = ");
      Serial.println(cicloTrabalho);

      OCR2B = cicloTrabalho;
      Serial.print("OCR2B = ");
      Serial.println(OCR2B);
} // end void selDuty(float dutyCycle)

void selFreq(char opcao) {

      switch (opcao) {
        
            case 1: // tccr2b = valor em bits de N
                  TCCR2B |= (1<<CS20);
            break;

            case 2:
                  TCCR2B |= (1<<CS21);
            break;

            case 3:
                  TCCR2B |= (1<<CS20) | (1<<CS21);
            break;

            case 4:
                  TCCR2B |= (1<<CS22);
            break;

            case 5:
                  TCCR2B |= (1<<CS20) | (1<<CS22);
            break;

            case 6:
                  TCCR2B |= (1<<CS21) | (1<<CS22);
            break;

            case 7:
                  TCCR2B |= (1<<CS20) | (1<<CS21) | (1<<CS22);
            break;

            default:
                  TCCR2B |= (1<<CS20);
            break;
      }
      Serial.print("TCCR2B = ");
      Serial.println(TCCR2B);
} // end void selFreq(char opcao)  

int arredondar( float valor ) {                       // algoritmo para arredondamento de valor float

    int resultado, sobra;                          // variáveis usadas pela função
    
    resultado = valor * 100;                          // 1 - multiplicar valor por 100, guardar em 'resultado'
    Serial.print("resultado = ");
    Serial.println(resultado);
    
    sobra = resultado % 100;                          // 2 - dividir o valor por 100, guarda o resto da divisão em 'sobra'
    Serial.print("sobra = ");
    Serial.println(sobra);

      Serial.print("sobra e resultado final = ");
      if (sobra <= 59) {                              // 3.1 - se o valor da sobra for menor ou igual a 59 (ou seja, quando o valor fracionario for 0,59 ou menos)
          sobra = 0;                                  // 3.1.1 - zera o resto, e não altera o resultado
          Serial.println(sobra);
          
      } else {                                        // 3.2 - se o valor da sobra for maior que 59 (ou seja, quando o valor fracionario for 0,60 ou mais) 
          resultado = resultado + 100;                // 3.2.1 - soma 100 ao resultado (ou seja, 1 no valor final)
          sobra = 0;                                  // 3.2.2 - zera o resto
          Serial.println(sobra);
          Serial.print("resultado calculado = ");
          Serial.println(resultado);
      }

      resultado /= 100;                               // 4 - para voltar ao valor "normal", dividir o resultado por 100, e guardar nele mesmo
      Serial.print("resultado final = ");
      Serial.println(resultado);
      
      return resultado;                               // 5 - retornar ao código com o valor arredondado
}// end int arredondar( float valor )
