# projeto01
Controlador para automação de produção de cerveja, construído com Arduino.

Este projeto é fruto da necessidade de automação do processo de brassagem de cervejas no âmbito doméstico.
				
   	No protótipo desenvolvido, utilizou-se os seguintes componentes:
    -> 1PCS 5V low level trigger One 1 Channel Relay Module interface Board Shield For PIC AVR DSP ARM MCU Arduino .
    -> 1PCS DS1820 Stainless steel package Waterproof DS18b20 temperature probe temperature sensor 18B20 For Arduino
    -> Uno R3 Case Enclosure Transparent Acrylic Box Clear Cover Compatible with Arduino UNO R3
    -> 1LOT Infrared IR Wireless Remote Control Module Kits DIY Kit HX1838 For Arduino Raspberry Pi
    -> High Quality Active BUZZER Module for Arduino New DIY Kit Active BUZZER low level modules
    -> 1PCS LCD module Blue screen IIC/I2C 1602 for arduino 1602 LCD UNO r3 mega2560
    -> WAVGAT high quality One set UNO R3 (CH340G) MEGA328P for Arduino UNO R3 + USB CABLE ATMEGA328P-AU Development board
    -> male to female dupont line 40pcs dupont cable jumper wire dupont line 2.54MM 20cm for arduino SKUGM
    -> WAVGAT Solid State Relay SSR-10DA SSR-25DA SSR-40DA 10A 25A 40A Actually 3-32V DC TO 24-380V AC SSR 10DA 25DA 40DA
    -> resistencia de baixa densidade de 2500W/220V
    -> I2C DS1307 AT24C32 Módulo de Relógio de Tempo Real RTC 51 AVR PIC ARM PARA arduino


          O objetivo do projeto é implementar um controle simples de temperatura, que se dará com:
          1 - recebimento dos sinais de temperatura
          2 - acionamento do rele de baixo nível para ativação da bomba de recirculação de 220V
          3 - acionamento do rele SSR para ativação da resistencia
          Todo sistema será programado por controle remoto(IR), com menu simples e direto.

          O codigo esta estruturado em um unico modulo, o que pode torna-lo confuso.
          Assim, o dividirei em grandes blocos, sempre que possível com os comentários, conforme preconiza a boa pratica de programacao em C.

          Esta versao ainda é embrionária, portanto, poderá haver erros de codigo.
          Baseado nos principios do software livre, este codigo é aberto, sob GNU General Public License.

          AUTOR: mvforce@gmail.com
          Brasilia, janeiro de 2019
