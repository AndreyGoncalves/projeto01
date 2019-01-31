/*
				Este projeto é fruto da necessidade de automação do processo de brassagem de cervejas, no âmbito doméstico.
				No protótipo desenvolvido, utilizou-se os seguintes componentes:
				 1PCS 5V low level trigger One 1 Channel Relay Module interface Board Shield For PIC AVR DSP ARM MCU Arduino .
				 1PCS DS1820 Stainless steel package Waterproof DS18b20 temperature probe temperature sensor 18B20 For Arduino
				 Uno R3 Case Enclosure Transparent Acrylic Box Clear Cover Compatible with Arduino UNO R3
				 1LOT Infrared IR Wireless Remote Control Module Kits DIY Kit HX1838 For Arduino Raspberry Pi
				 High Quality Active BUZZER Module for Arduino New DIY Kit Active BUZZER low level modules
				 1PCS LCD module Blue screen IIC/I2C 1602 for arduino 1602 LCD UNO r3 mega2560
				 WAVGAT high quality One set UNO R3 (CH340G) MEGA328P for Arduino UNO R3 + USB CABLE ATMEGA328P-AU Development board
				 male to female dupont line 40pcs dupont cable jumper wire dupont line 2.54MM 20cm for arduino SKUGM
				 WAVGAT Solid State Relay SSR-10DA SSR-25DA SSR-40DA 10A 25A 40A Actually 3-32V DC TO 24-380V AC SSR 10DA 25DA 40DA
				 resistencia de baixa densidade de 2500W/220V

				O objetivo do projeto é implementar um controle simples de temperatura, que se dará com:
				1 - recebimento dos sinais de temperatura
				2 - acionamento do rele de baixo nível para ativação da bomba de recirculação de 220V
				3 - acionamento do rele SSR para ativação da resistencia
				Todo sistema será programado por controle remoto(IR), com menu simples e direto.

				O codigo esta estruturado em um unico modulo, o que pode torna-lo confuso.
				Assim, o dividirei em grandes blocos, sempre que possível com os comentários, conforme preconiza as boas praticas de programacao.

				Esta versao ainda é embrionária, portanto, haverá erros de codigo.
				Baseado nos principios do software livre, este codigo é aberto, sob GNU General Public License.

				AUTOR: mvforce@gmail.com
				Brasilia, janeiro de 2019
				*/




// ----------------------------------- bloco de inclusão de bibliotecas -------------------//

// bibliotecas do sensor de temperatura
#include <OneWire.h>		   // versao 2.3.4
#include <DallasTemperature.h> //versao 3.8

// bibliotecas do LCD
#include <Wire.h> //https://github.com/PaulStoffregen/Wire
#include <LiquidCrystal_I2C.h> //https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads/


// bibliotecas do controle remoto
#include <IRremote.h> // http://z3t0.github.io/Arduino-IRremote/

//Arduino Time library - Timers e manipulacao de tempo
#include <DS1307.h> //https://github.com/filipeflop/DS1307


// ----------------------------------- bloco de definições e endereçamento -------------------//

// -------------- PÍNOS -----------//


#define PINO_BUZZER 2
#define PINO_SENSOR_TEMP 3
#define PINO_IR 4
#define PINO_RELE_BAIXO 5
#define PINO_RELE_ALTO 6

// SENSOR DE TEMPERATURA
OneWire temperatura_pino(PINO_SENSOR_TEMP); // pino que esta conectado o sensor
DallasTemperature  temperatura_barramento (&temperatura_pino);
DeviceAddress temperatura_sensor;


// CONTROLE REMOTO
IRrecv receptor(PINO_IR);  
decode_results resultado;

// Display LCD  
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3, POSITIVE); // declaracao padronizada | PINOS A5(SCL) A4(SDA)


#define MENU_PRINCIPAL 0
#define MENU_RAMPAS 1
#define MENU_TEMPERATURA 2
#define MENU_TEMPO 3
#define MENU_MASHOUT 4
#define MENU_MOSTURA 5
#define MENU_FERVURA 6
#define ETAPA_MOSTURA 7
#define ETAPA_MASHOUT 8
#define ETAPA_FERVURA 9
#define MENU_INICIAR_FERVURA 10
#define MENU_FINAL_FERVURA 11
#define ETAPA_RESFRIAMENTO 12
#define ETAPA_ADICAO_MALTE 13
#define MENU_MALTE_ADICIONADO 14
#define MENU_TEMPERATURA_CONSTANTE 15
#define ETAPA_TEMPERATURA_CONSTANTE 16

#define EXEC_RAMPA1 17
#define EXEC_RAMPA2 18
#define EXEC_RAMPA3 19
#define SUBIR_TEMP_RAMPA2 20
#define SUBIR_TEMP_RAMPA3 21
#define SUBIR_TEMP_MASHOUT 22
#define SUBIR_TEMP_FERVURA 23
#define EXEC_MASHOUT 24
#define EXEC_FERVURA 25



#define RAMPA1 0
#define RAMPA2 1
#define RAMPA3 2
#define MASHOUT 3
#define FERVURA 4
#define RESFRIAMENTO 5


#define MAX_RAMPAS 3
#define FATOR_CORRECAO_TEMPERATURA 2 // ADICIONA X GRAUS A TEMPERATURA PARA ADICAO DE MALTE


#define ATIVO 1
#define INATIVO 0

//Modulo RTC DS1307 ligado as portas A3 e A2 do Arduino 
DS1307 rtc(A3, A2); //SDA=A3 SCL=A2



// -------------------------- VARIAVEIS E STRUCTS GLOBAIS -------------------// 
byte menu_item =MENU_PRINCIPAL; //define a posicao do menu
byte sons=ATIVO;
byte etapa_da_mostura = EXEC_RAMPA1;

unsigned int qtd_rampas=1;
unsigned int temperatura_rampa[MAX_RAMPAS+3]; //numero de rampas de mostura + mashout + fervura + resfriamento
unsigned int tempo[MAX_RAMPAS+3]; //numero de rampas de mostura + mashout + fervura + resfriamento
unsigned int hora_rampa[MAX_RAMPAS+3]; //numero de rampas de mostura + mashout + fervura + resfriamento
unsigned int minuto_rampa[MAX_RAMPAS+3]; //numero de rampas de mostura + mashout + fervura + resfriamento





// =================== FUNÇÃO SETUP =============//

void setup() {
 
temperatura_rampa[MASHOUT]=32; //78
temperatura_rampa[FERVURA]=32; //94
temperatura_rampa[RESFRIAMENTO]=30; //30
tempo[RESFRIAMENTO]=30; //30
 
// SENSOR TEMPERATURA 
temperatura_barramento.begin();
temperatura_barramento.getAddress(temperatura_sensor, 1);

//  DISPLAY LCD
lcd.begin(16,2); // 16 colunas x 2 linhas 
lcd.blink();

//  CONTROLE REMOTO 
receptor.enableIRIn(); // Inicializa o receptor IR

#define BT_0 16750695
#define BT_1 16753245
#define BT_2 16736925
#define BT_3 16769565
#define BT_4 16720605
#define BT_5 16712445
#define BT_6 16761405
#define BT_7 16769055
#define BT_8 16754775
#define BT_9 16748655
#define BT_ASTERISTICO 16738455
#define BT_TRALHA 16756815
#define BT_CIMA 16718055
#define BT_BAIXO 16730805
#define BT_ESQUERDA 16716015
#define BT_DIREITA 16734885
#define BT_OK 16726215


// RELOGIO
rtc.halt(false); //Aciona o relogio

//Definicoes do pino SQW/Out
rtc.setSQWRate(SQW_RATE_1);
rtc.enableSQW(true);
   
//  BUZZER 
pinMode(PINO_BUZZER,OUTPUT);
digitalWrite(PINO_BUZZER, HIGH);


//  RELE BAIXO 
pinMode  (PINO_RELE_BAIXO,OUTPUT); //define o pino do rele como saída
digitalWrite(PINO_RELE_BAIXO,HIGH); // coloca em estado ALTO=desligado

// RELE ALTO	 
pinMode  (PINO_RELE_ALTO,OUTPUT); //define o pino do rele como saída
digitalWrite(PINO_RELE_ALTO,LOW); // coloca em estado BAIXO=desligado

} // FIM DA FUNCAO SETUP()




// =================== FUNÇÃO LOOP ===================//
void loop() {

	if (menu_item==MENU_PRINCIPAL)  menu_principal();
	if (menu_item==MENU_RAMPAS) menu_recebe_rampas();
	if (menu_item==MENU_TEMPERATURA) menu_recebe_temperatura();
	if (menu_item==MENU_TEMPO) menu_recebe_tempo();
	if (menu_item==MENU_MASHOUT) menu_mashout();
	if (menu_item==MENU_MOSTURA) menu_mostura();
	if (menu_item==MENU_FERVURA) menu_fervura();
	if (menu_item==ETAPA_MOSTURA)realiza_mostura();
	if (menu_item==ETAPA_MASHOUT) realiza_mashout();
	if (menu_item==MENU_INICIAR_FERVURA) menu_iniciar_fervura();
	if (menu_item==ETAPA_FERVURA) realiza_fervura();
	if (menu_item==MENU_FINAL_FERVURA) menu_final_fervura();
	if (menu_item==ETAPA_RESFRIAMENTO) realiza_resfriamento();
	if (menu_item==ETAPA_ADICAO_MALTE) realiza_adicao_malte();
	if (menu_item==MENU_MALTE_ADICIONADO) menu_malte_adicionado();
	if (menu_item==MENU_TEMPERATURA_CONSTANTE) menu_temperatura_constante();
	if (menu_item==ETAPA_TEMPERATURA_CONSTANTE) realiza_temperatura_constante();
	if (menu_item==SUBIR_TEMP_MASHOUT) ajusta_temperatura_mashout();
	if (menu_item==EXEC_MASHOUT) realiza_mashout();
	if (menu_item==SUBIR_TEMP_FERVURA) ajusta_temperatura_fervura();
	if (menu_item==EXEC_FERVURA) realiza_fervura();
	
	

delay(100);

} // FIM DA FUNCAO LOOP()

void menu_principal()
{
	if(menu_item==MENU_PRINCIPAL) {

	byte retorno=0;

	exibe_sim_nao("BRASSAGEM:");
	controle_comandos(&retorno);
	
		if(retorno==1){
		menu_item=MENU_RAMPAS;
		lcd.clear();	
		}
		if(retorno==2){
		menu_item=MENU_TEMPERATURA_CONSTANTE;
		if(sons==ATIVO) beep_pipi();
		}
	delay(100);
	}
}


void menu_recebe_rampas()
{
	if(menu_item==MENU_RAMPAS) {
	
	qtd_rampas=controle_numeros("QTD DE RAMPAS:",1);
	
	if(qtd_rampas<1 || qtd_rampas>MAX_RAMPAS){
	popup_msg("QTD INCORRETA",2);
	popup_msg("SETANDO PADRAO",2);
	popup_msg("RAMPAS = 1",2);
	qtd_rampas=1;
	}
	menu_item=MENU_TEMPERATURA;
	lcd.clear();
	}
}



void menu_recebe_temperatura()
{
	if(menu_item==MENU_TEMPERATURA){
		
	String buffer PROGMEM;


		for(unsigned int i=0;i<qtd_rampas;i++){
		buffer="TEMPERATURA";
		buffer+="("+String(i+1)+"):";
		temperatura_rampa[i]=controle_numeros(buffer,2);
		}
	 
	menu_item=MENU_TEMPO;
	}
}
  
void menu_recebe_tempo()
{
	if(menu_item==MENU_TEMPO){
	  
	String buffer PROGMEM;
	  
		for(unsigned int i=0;i<qtd_rampas;i++){
		buffer="TEMPO(min)";
		buffer+="("+ String(i+1)+"):";
		tempo[i]=controle_numeros(buffer,3);
	 	}

	menu_item=MENU_MASHOUT;
	}
}


void menu_mashout()
{
	if(menu_item==MENU_MASHOUT){
	tempo[MASHOUT]=controle_numeros("MASHOUT(min):",2);
	menu_item=MENU_FERVURA;
	}

}

void menu_fervura()
{
	if(menu_item==MENU_FERVURA){
	tempo[FERVURA]=controle_numeros("FERVURA(min):",3);
	menu_item=MENU_MOSTURA;
	}
	
}
 
void menu_mostura()
{
	if(menu_item==MENU_MOSTURA) {
	byte retorno=0;
	
	exibe_sim_nao("INICIAR MOSTURA");
	controle_comandos(&retorno);
	
		if(retorno==1){
		menu_item=ETAPA_ADICAO_MALTE;
		if(sons==ATIVO) beep_pi_longo;
		}
	delay(100);
	}
}


void menu_iniciar_fervura()
{
	if(menu_item==MENU_INICIAR_FERVURA) {
	byte retorno=0;
	
	exibe_sim_nao("INICIAR FERVURA");
	controle_comandos(&retorno);
	
		if(retorno==1){
		menu_item=SUBIR_TEMP_FERVURA;
		if(sons=ATIVO) beep_pi_longo();
		}
	delay(200);
	}
}




void menu_final_fervura()
{
	if(menu_item==MENU_FINAL_FERVURA) {
	byte retorno=0;
	exibe_sim_nao("RESFRIAMENTO:");
	controle_comandos(&retorno);
	if(sons=ATIVO) beep_pipi();
	
		if(retorno==1){
		menu_item=ETAPA_RESFRIAMENTO;
		}
    delay(200);
	}
  
}

void menu_malte_adicionado()
{
	if(menu_item==MENU_MALTE_ADICIONADO) {
	byte retorno=0;
	
	if(sons==ATIVO) beep_pipi();
	exibe_sim_nao("MALTE ADICIONADO");
	controle_comandos(&retorno);
	controla_temperatura(temperatura_rampa[RAMPA1]+FATOR_CORRECAO_TEMPERATURA); // mantem temperatura controlada até confirmar a adicao de malte

		if(retorno==1){
		menu_item=ETAPA_MOSTURA;
		Time t2=calcula_horario(tempo[RAMPA1]);
		hora_rampa[RAMPA1]=t2.hour;
		minuto_rampa[RAMPA1]=t2.min;
		if(sons==ATIVO) beep_pi_longo();

		}
	delay(200);
	}
}

void menu_temperatura_constante()
{
	if(menu_item==MENU_TEMPERATURA_CONSTANTE){
	String buffer PROGMEM;
	
	temperatura_rampa[RAMPA1]=controle_numeros("TEMPERATURA FIXA",2);
	menu_item=ETAPA_TEMPERATURA_CONSTANTE;
	if(sons==ATIVO) beep_pi_longo();
	lcd.clear();
	lcd.print("CONSTANTE >>>>");
	delay(5000);
	}
			
}


void exibe_sim_nao(char titulo[16])
{
lcd.clear();
lcd.print(titulo); //titulo
lcd.setCursor(2,1); 
lcd.print("1=SIM | 2=NAO");//sim ou não	
delay(150);
}

unsigned int controle_numeros(String titulo, unsigned int qtd_digitos)
{

static unsigned int i=0;
static char interno[4];
for(int j=0;j<=4;j++) interno[j]=' ';

	while(resultado.value!=BT_OK){
		if(i>qtd_digitos){
			i=0;
			lcd.setCursor(0,1);
			for(int j=0;j<=qtd_digitos;j++) interno[j]=' ';
			}
		lcd.clear();
		lcd.print(titulo); // imprime titulo do input
		lcd.setCursor(0,1);
		for(int j=0;j<=qtd_digitos;j++) if(interno[j]!=' ') lcd.print(interno[j]);

		if(receptor.decode(&resultado) && i<=qtd_digitos){
	
			if(resultado.value==BT_0) {
			if(sons==ATIVO) beep_pi();
			
			interno[i]='0';
			lcd.setCursor(i,1);
			i++;	
			}
		
			if(resultado.value==BT_1){
				if(sons==ATIVO) beep_pi();
			
				interno[i]='1';
				lcd.setCursor(i,1);
				i++;	
				}
			if(resultado.value==BT_2){
				if(sons==ATIVO) beep_pi();
			
				interno[i]='2';
				lcd.setCursor(i,1);
				i++;	
				}
				
			if(resultado.value==BT_3){
				if(sons==ATIVO) beep_pi();
			
				interno[i]='3';
				lcd.setCursor(i,1);
				i++;
				
				}
			if(resultado.value==BT_4){
				if(sons==ATIVO) beep_pi();
			
				interno[i]='4';
				lcd.setCursor(i,1);
				i++;
				
				}
			if(resultado.value==BT_5){
				if(sons==ATIVO) beep_pi();
			
				interno[i]='5';
				lcd.setCursor(i,1);
				i++;
			
				}
			if(resultado.value==BT_6){
				if(sons==ATIVO) beep_pi();
		
				interno[i]='6';
				lcd.setCursor(i,1);
				i++;
			
				}
			if(resultado.value==BT_7){
				if(sons==ATIVO) beep_pi();
			
				interno[i]='7';
				lcd.setCursor(i,1);
				i++;
			
				}
			if(resultado.value==BT_8){
				if(sons==ATIVO) beep_pi();
			
				interno[i]='8';
				lcd.setCursor(i,1);
				i++;
		
				}
			if(resultado.value==BT_9){
				if(sons==ATIVO) beep_pi();
			
				interno[i]='9';
				lcd.setCursor(i,1);
				i++;
		
				}
			if(resultado.value==BT_ASTERISTICO){  // apaga valores digitados
				if(sons==ATIVO) beep_pi();
				
				for(int j=0;j<=qtd_digitos;j++) interno[j]=' ';
		        int valor=atoi(&interno[0]);
		        i=0;
		        receptor.resume();
		        resultado.value=NULL;
		  	
				}
					
			if(resultado.value==BT_OK){
				if(sons==ATIVO) beep_pi();
		
				unsigned int valor=atoi(interno);
				for(int j=0;j<=qtd_digitos;j++) interno[j]=' ';
				i=0;
				receptor.resume();
				resultado.value=NULL;
				return valor;		
				}
	
	delay(100);
	receptor.resume(); //Le o próximo valor 
	
	} //FIM DO IF
delay(250);
} // FIM DO WHILE
return 0;
}




void controle_comandos(byte *valor)
{
   if (receptor.decode(&resultado)){  
	 
		  if(resultado.value==BT_0) {
		
			  if(sons==ATIVO) beep_pi();
			  *valor=0;
		  }
		 
		 if(resultado.value==BT_1){
			 if(sons==ATIVO) beep_pi();
			 *valor=1;
			 }
		if(resultado.value==BT_2){
			if(sons==ATIVO) beep_pi();
			*valor=2;
		}
		if(resultado.value==BT_3){
			if(sons==ATIVO) beep_pi();
			*valor=3;
		}
		if(resultado.value==BT_4){
			if(sons==ATIVO) beep_pi();
			*valor=4;
		}
		if(resultado.value==BT_5){
			if(sons==ATIVO) beep_pi();
			*valor=5;
		}
		if(resultado.value==BT_6){
			if(sons==ATIVO) beep_pi();
			*valor=6;
		}
		if(resultado.value==BT_7){
			if(sons==ATIVO) beep_pi();
			*valor=7;
		}
		if(resultado.value==BT_8){
			if(sons==ATIVO) beep_pi();
			*valor=8;
		}
		if(resultado.value==BT_9){
			if(sons==ATIVO) beep_pi();
			*valor=9;
		}
		if(resultado.value==BT_ASTERISTICO){
			if(sons==ATIVO) beep_pi();
			*valor=10;
		}
		if(resultado.value==BT_TRALHA){
			if(sons==ATIVO) beep_pi();
			*valor=11;
		}
		if(resultado.value==BT_CIMA){
			if(sons==ATIVO) beep_pi();
			*valor=12;
		}
		if(resultado.value==BT_BAIXO){
			if(sons==ATIVO) beep_pi();
			*valor=13;
		}
		if(resultado.value==BT_ESQUERDA){
			if(sons==ATIVO) beep_pi();
			*valor=14;
		}
		if(resultado.value==BT_DIREITA){
			if(sons==ATIVO) beep_pi();
			*valor=15;
		}
		if(resultado.value==BT_OK){
			if(sons==ATIVO) beep_pi();
			*valor=16;
		 }
      
       }
      delay(100);
      receptor.resume(); //Le o próximo valor 
    
}



void controla_temperatura(unsigned int temperatura)
{
unsigned int temperatura_atual=le_temperatura();
if(temperatura_atual<temperatura) ativa_resistencia();
else desativa_resistencia();
}

void realiza_adicao_malte()
{
	if(menu_item==ETAPA_ADICAO_MALTE){
		
		ativa_bomba();
		unsigned int temperatura_alvo= temperatura_rampa[RAMPA1]+FATOR_CORRECAO_TEMPERATURA; //SOBE A TEMPERATURA X GRAUS ACIMA DA PRIMEIRA RAMPA
		controla_temperatura(temperatura_alvo);
		exibe_subindo_temperatura_alternados(RAMPA1);
		
		unsigned int temperatura_atual=le_temperatura();
			
			if(temperatura_atual>=temperatura_alvo) menu_item=MENU_MALTE_ADICIONADO;
	delay(250);
	}

}

void ajusta_temperatura_mashout()
{
	if(menu_item==SUBIR_TEMP_MASHOUT){
		unsigned int temperatura_atual=le_temperatura();
		controla_temperatura(temperatura_rampa[MASHOUT]);
		exibe_subindo_temperatura_alternados(MASHOUT);
		ativa_bomba();

			if(temperatura_atual>=temperatura_rampa[MASHOUT]) {
				Time t2=calcula_horario(tempo[MASHOUT]);
				hora_rampa[MASHOUT]=t2.hour;
				minuto_rampa[MASHOUT]=t2.min;
				if(sons==ATIVO) beep_pi_longo();	
				menu_item=EXEC_MASHOUT;
			}
		
	}	
	
}

void realiza_mashout()
{
	
	if(menu_item==EXEC_MASHOUT){
		if(realiza_rampa(MASHOUT)==0){ 
			if(sons==ATIVO) beep_pi_longo();
			desativa_bomba();
			menu_item=MENU_INICIAR_FERVURA;
			
		}	
		
	}
}
	
void ajusta_temperatura_fervura()
{
		if(menu_item==SUBIR_TEMP_FERVURA){
		unsigned int temperatura_atual=le_temperatura();
		controla_temperatura(temperatura_rampa[FERVURA]);
		exibe_subindo_temperatura_alternados(FERVURA);
		//ativa_bomba();

			if(temperatura_atual>=temperatura_rampa[FERVURA]) {
				Time t2=calcula_horario(tempo[FERVURA]);
				hora_rampa[FERVURA]=t2.hour;
				minuto_rampa[FERVURA]=t2.min;
				if(sons==ATIVO) beep_pi_longo();	
				menu_item=EXEC_FERVURA;
			}
		
	}
	
	
}
	
void realiza_fervura()
{
	if(menu_item==EXEC_FERVURA){
		
	if(tempo_restante(FERVURA)>0){
	ativa_resistencia();
	exibe_dados_alternados(FERVURA);
	}
	else{
			if(sons==ATIVO) beep_pipi();
			menu_item=MENU_FINAL_FERVURA;
			desativa_resistencia();
		}
		
	}
}
	
		


void realiza_resfriamento()
{
	if(menu_item==ETAPA_RESFRIAMENTO){
	desativa_resistencia();
	lcd.clear();
    lcd.print("RESFRIAMENTO >>");
    delay(5000);
    if(sons==ATIVO) beep_pi_longo();	
    
	}
}



void exibe_temperatura_constante()
{
Time t1=rtc.getTime();
String buffer PROGMEM;
static byte temperatura_atingida=0;

	if((t1.sec % 10)<=5){
	lcd.clear();
	lcd.print("Temp(c) | Alvo:");
	lcd.setCursor(2,1);
	buffer=String(le_temperatura())+"    |   "+String(temperatura_rampa[RAMPA1]);
	lcd.print(buffer);  
	delay(250);
	}
	else{
	lcd.clear();
	lcd.print("Transcorrido:");
	lcd.setCursor(2,1);
	int dif, r1,r2;
	
	r1=minuto_total(t1.hour,t1.min);
	r2=minuto_total(hora_rampa[RAMPA1],minuto_rampa[RAMPA1]);
	dif=(r1-r2);
	
	buffer=String(dif)+" minutos";
	lcd.print(buffer);
	delay(250);
	}
}
void exibe_subindo_temperatura_constante()
{
Time t1=rtc.getTime();
String buffer PROGMEM;
	
	if((t1.sec % 10)<=5){
	lcd.clear();
	lcd.print("Temp(c) | Alvo:");
	lcd.setCursor(2,1);
	buffer=String(le_temperatura())+"    |   "+String(temperatura_rampa[RAMPA1]);
	lcd.print(buffer);	
	delay(250);
	}
	else{
	lcd.clear();
	lcd.print("CONSTANTE >>>>");
	lcd.setCursor(0,1);
	lcd.print("Subindo temp...");
	delay(250);
	}
}



void exibe_dados_alternados(unsigned int RAMPA)
{
Time t1=rtc.getTime();
String buffer PROGMEM;

	if((t1.sec % 10)<=5){
	lcd.clear();
	lcd.print("Temp(c) | Alvo:");
	lcd.setCursor(2,1);
	buffer=String(le_temperatura())+"    |   "+String(temperatura_rampa[RAMPA]);
	lcd.print(buffer);  
	delay(250);
	}
	else{
	lcd.clear();
	lcd.print("Tempo restante:");
	lcd.setCursor(2,1);
	int dif=tempo_restante(RAMPA);
	
	buffer=String(dif)+" minutos";
	lcd.print(buffer);
	delay(250);
	}
}




// FUNÇÃO QUE LE A TEMPERATURA

unsigned int le_temperatura()
{ 
temperatura_barramento.requestTemperatures(); 
unsigned int temperatura = temperatura_barramento.getTempC(temperatura_sensor);
if(temperatura>100 || temperatura<0) temperatura=20; //evita estouro de buffer por erro de leitura no sensor

return temperatura;
delay(50); //espera antes de ler de novo
}



void exibe_subindo_temperatura_alternados(unsigned int RAMPA)
{
Time t1=rtc.getTime();
String buffer PROGMEM;
	
	if((t1.sec % 10)<=5){
	lcd.clear();
	lcd.print("Temp(c) | Alvo:");
	lcd.setCursor(2,1);
	
	if(RAMPA==RAMPA1) buffer=String(le_temperatura())+"    |   "+String(temperatura_rampa[RAMPA1]+FATOR_CORRECAO_TEMPERATURA);
	else buffer=String(le_temperatura())+"    |   "+String(temperatura_rampa[RAMPA]);
	
	lcd.print(buffer);	
	delay(250);
	}
	else{
	lcd.clear();
	if(RAMPA==MASHOUT) lcd.print("MASHOUT> Subindo");
	if(RAMPA==FERVURA) lcd.print("FERVURA> Subindo");
	if(RAMPA==RAMPA1) lcd.print("RAMPA N1> Subindo");
	if(RAMPA==RAMPA2) lcd.print("RAMPA N2> Subindo");
	if(RAMPA==RAMPA3) lcd.print("RAMPA N3> Subindo");
	
	lcd.setCursor(0,1);
	lcd.print("temperatura...");
	delay(250);
	}

}



// ------------- FUNÇÕES BUZZER ------------------- ///

void beep_pipi()
{
digitalWrite(PINO_BUZZER, LOW);
delay(200);
digitalWrite(PINO_BUZZER, HIGH);
delay(50);
digitalWrite(PINO_BUZZER, LOW);
delay(200);
digitalWrite(PINO_BUZZER, HIGH);
delay(50);

}


void beep_pi()
{
digitalWrite(PINO_BUZZER, LOW);
delay(200);
digitalWrite(PINO_BUZZER, HIGH);
delay(50);
}


void beep_pi_longo()
{
digitalWrite(PINO_BUZZER, LOW);
delay(400);
digitalWrite(PINO_BUZZER, HIGH);
delay(50);
}

  
void popup_msg(String msg, byte tempo) //tempo de exibicao em segundos
{
lcd.clear();
lcd.print(msg);
delay(500*tempo);
}


Time calcula_horario(unsigned int minutos)
{
Time t1=rtc.getTime();
Time t2;
int r1=(t1.min+minutos+1) / 60;

t2.hour=t1.hour+r1;
t2.hour=t2.hour % 24;
t2.min=(t1.min+minutos-1) % 60;
	
return t2;
}


void ativa_resistencia()
{
digitalWrite(PINO_RELE_ALTO, HIGH);
}

void desativa_resistencia()
{
digitalWrite(PINO_RELE_ALTO, LOW);
}

void ativa_bomba()
{
digitalWrite(PINO_RELE_BAIXO, LOW);
}

void desativa_bomba()
{
digitalWrite(PINO_RELE_BAIXO, HIGH);
}


int tempo_restante(unsigned int num_rampa)
{
Time t1=rtc.getTime();
int r1 = minuto_total(t1.hour,t1.min);
int r2 = minuto_total(hora_rampa[num_rampa],minuto_rampa[num_rampa]);

int dif=(r2-r1);
return dif+1;
}

int minuto_total(unsigned int hora, unsigned int minuto)
{
	int r = (hora*60) + minuto;
	return r;
}


void realiza_temperatura_constante()
{
	static byte atingida=0;
	
	if(menu_item==ETAPA_TEMPERATURA_CONSTANTE){
	controla_temperatura(temperatura_rampa[RAMPA1]);
	//ativa_bomba();
	
	if (le_temperatura()<temperatura_rampa[RAMPA1] && atingida==0) exibe_subindo_temperatura_constante(); 
	
	if (le_temperatura()==temperatura_rampa[RAMPA1] && atingida==0){
		 atingida=1;
		 Time t1=rtc.getTime();
		 hora_rampa[RAMPA1]=t1.hour;
		 minuto_rampa[RAMPA1]=t1.min;
	}
	
	if (atingida==1) exibe_temperatura_constante();
	
	delay(250);
	}
}





void realiza_mostura()
{
	if(menu_item==ETAPA_MOSTURA){
	
	unsigned int temperatura_atual=0;
		
	switch(etapa_da_mostura){
	
	case EXEC_RAMPA1:
	if(realiza_rampa(RAMPA1)==0){
		
		if(qtd_rampas>=2){
		popup_msg("INDO PARA...",2);
		popup_msg("RAMPA N2",2);
		if(sons=ATIVO) beep_pi_longo();
		etapa_da_mostura=SUBIR_TEMP_RAMPA2;
		}
		else {
			if(sons=ATIVO) beep_pi_longo();
			menu_item=SUBIR_TEMP_MASHOUT;
			break;
		}
	}
	break;
		
	case SUBIR_TEMP_RAMPA2:
	controla_temperatura(temperatura_rampa[RAMPA2]);
	exibe_subindo_temperatura_alternados(RAMPA2);
	
	temperatura_atual = le_temperatura();
	
	if(temperatura_atual>=temperatura_rampa[RAMPA2]){
		if(sons=ATIVO) beep_pi_longo();
		
		Time t2=calcula_horario(tempo[RAMPA2]);
		hora_rampa[RAMPA2]=t2.hour;
		minuto_rampa[RAMPA2]=t2.min;
		etapa_da_mostura=EXEC_RAMPA2;

	}
	break;	
	
	case EXEC_RAMPA2:
	if(realiza_rampa(RAMPA2)==0){
		if(qtd_rampas>=3){
		popup_msg("INDO PARA...",2);
		popup_msg("RAMPA N3",2);
		if(sons=ATIVO) beep_pi_longo();
		etapa_da_mostura=SUBIR_TEMP_RAMPA3;
		}
		else{
			menu_item=SUBIR_TEMP_MASHOUT;
			if(sons=ATIVO) beep_pi_longo();
		}
	}
	break;	
	
	case SUBIR_TEMP_RAMPA3:
	controla_temperatura(temperatura_rampa[RAMPA3]);
	exibe_subindo_temperatura_alternados(RAMPA2);
	
	temperatura_atual = le_temperatura();
	
	if(temperatura_atual>=temperatura_rampa[RAMPA3]){
		if(sons=ATIVO) beep_pi_longo();
		
		Time t2=calcula_horario(tempo[RAMPA3]);
		hora_rampa[RAMPA3]=t2.hour;
		minuto_rampa[RAMPA3]=t2.min;
		
		etapa_da_mostura=EXEC_RAMPA3;
		}
	break;
	
	case EXEC_RAMPA3:
		if(realiza_rampa(RAMPA3)==0){
		if(sons=ATIVO) beep_pi_longo();
		menu_item=SUBIR_TEMP_MASHOUT;
		
	}
	break;
		
	}
				
	delay(300);
	}
}


unsigned int realiza_rampa(unsigned int RAMPA)
{
if(tempo_restante(RAMPA)>0){
	controla_temperatura(temperatura_rampa[RAMPA]);
	exibe_dados_alternados(RAMPA);
	
	ativa_bomba();
	
	return (1);
	}
return(0);
}

