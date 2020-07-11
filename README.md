# Arduino-Project-MPLP
repositório para projeto MPLP

O projeto consiste em um módulo gerador de sinal PWM, onde a frequência e largura do pulso (duty cycle) do sinal são configuráveis. Desenvolvido com Arduino IDE e placa Arduino UNO.

Inicialmente pensei em 7 frequências diferentes: - 1 Hz
                                                 - 60 Hz
                                                 - 100 Hz
                                                 - 1 kHz
                                                 - 10 kHz
                                                 - 100 kHz
                                                 - 16 MHz

Porém, esses valores podem ser alterados, dependendo da dificuldade em trabalhar com o timer para esses.

UPDATE 10/07/2020: revisão 2.1 lançada. Agora é possível alterar a frequência da saída, assim como o ciclo de trabalho. Frequências disponíveis: - 31,38 kHz
                                                                                                                                                 - 980,4  Hz
                                                                                                                                                 - 30,78  Hz
                   (é possível que os valores não sejam exatamente estes devido a alguns problemas intermitentes de funcionamento na simulação)

------------------------------------------------------------------------------------------------------------------------------------------

This project consists of a PWM signal generator module, where the frequency and duty cycle of the signal are configurable. Developed with Arduino IDE and Arduino UNO board.

Initially, I thought of 7 different frequencies to work with: - 1 Hz
                                                              - 60 Hz
                                                              - 100 Hz
                                                              - 1 kHz
                                                              - 10 kHz
                                                              - 100 kHz
                                                              - 16 MHz

However, these values may change depending on the difficulty of working with the timer for these.

UPDATE 10/07/2020: revision 2.1 released. It is now possible to change the output frequency as well as the duty cycle. Available frequencies: - 31,38 kHz
                                                                                                                                              - 980,4  Hz
                                                                                                                                              - 30,78  Hz
                  (it's possible that the values are not exactly these due to some intermittent problems of operation in the simulation)
