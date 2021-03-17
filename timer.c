//Primeiramente, incluo as bibliotecas necesárias.
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include<avr/interrupt.h>

/* Aqui, inicio o programa criando alguns ponteiros para os registradores que 
atuarão na interrupção do tipo Temporizador */

unsigned char * p_tccr2a; 
unsigned char * p_tccr2b; // Os dois registradores até então são registradores de configuração para o modo de operação do teporizador.
unsigned char * p_timsk2; // TiMsk2 é o temporizador relacionado a ativação da interrupção por overflow.


/*Como só vou usar as portas b das GPIO como portas de saída,
não preciso definir um ponteiro para p_in*/

unsigned char *p_ddrb;
unsigned char *p_portb;

/* Aqui eu declaro algumas variáveis que serão utilizadas no código:
(1) led13 é a variável que vai guardar um valor que será comparado com a mascara do GPIO 13 para o 
switch-case.

(2)led12 tem a mesma funcionalidade da varivel led13, mas para o GPIO 12.

(3) conta1, conta2 são variáveis contadoras que serão utilizadas para definir quando um led vai ficar
acesso e quando ele vai ficar apagado. Cada um para um led.

(4)A variável i é apenas um iterando. 
*/


unsigned char led13 = 0x20;
unsigned char led12 = 0x10;
unsigned int conta1;
unsigned int conta2;
unsigned int i;

//Mensagem que será exibida no monitor serial.

char msg[] = "Atividade 7 - Interrupcoes temporizadas tratam concorrencia entre tarefas! \n\n";

//Nas proximas linhas serão definidos alguns ponteiros que ja foram utilizados na atividade 6, eles se referem à interrupções de transmissão.

unsigned char *p_ubrr0h; // Ponteiro que será utilizado para definir o Baud Rate.
unsigned char *p_ubrr0l; // Ponteiro que será utilizado também para definir o Baud Rate.
unsigned char *p_ucsr0a; // Ponteiro que será utilizado para verificar se os dados foram transferidos, configurar a velocidade de transmissão dos dados e também a função de multiprocessamento.
unsigned char *p_ucsr0b; // Ponteiro que será utilziado para definir as interrupções; configurações de transmissão e recepção de dados; numero de bits.
unsigned char *p_ucsr0c; // Ponteiro para definir o modo de operaçaõ da USART; paridade;bits de parada.
unsigned char *p_udr0; // Ponteiro para o registrador de dados

void setup()
{
  cli(); // No inicio eu desabilito as interrupções para me precaver de algum problema de setup.
  
  // Declaro a posição da memoria onde se encontram os ponteiros supracitados.
  p_ubrr0h = (unsigned char *) 0xC5;
  p_ubrr0l = (unsigned char *) 0xC4;
  p_ucsr0a = (unsigned char *) 0xC0;
  p_ucsr0b = (unsigned char *) 0xC1;
  p_ucsr0c = (unsigned char *) 0xC2;
  p_udr0 = (unsigned char *) 0xC6;
  
  p_tccr2a = (unsigned char *) 0xB0;
  p_tccr2b = (unsigned char *) 0xB1;
  p_timsk2 = (unsigned char *) 0x70;
  p_ddrb = (unsigned char *) 0x24;
  p_portb= (unsigned char *)0x25;
  
  *p_ubrr0h |= 0x00;
  *p_ubrr0l &= 0x00;
  *p_ubrr0l |= 0x33; /*As configurações supracitadas em
  p_ubrr0h e p_ubrr0l são para definir um Baud Rate de 19.2kBps (51 nos regs)*/
  *p_ucsr0a &= 0xFC; // Desabilito aqui a taxa de transmissão dobrada e o multiprocessamento.
  *p_ucsr0b &= 0x03;
  *p_ucsr0b |= 0x98; /* Aqui, habilito a rececpção completa e deixo o modo Registrador de Dados Vazio em baixa por enquanto,
  visto que será ativado posteriormente*/
  *p_ucsr0c &=0x00;
  *p_ucsr0c |= 0x06; // Configuração para utilizar a USART em modo assincrono, 2 bits de parada, sem paridade.

  
  *p_tccr2a = 0; //Aqui defino o modo de operação normal, e OCnA desconectados. 
  *p_tccr2b = 2; //Aqui defino apenas o prescale de 8
  *p_timsk2 = 1; // Aqui defino a interrupção por overflow. 
  
  //Declaro que as portas 12,13 serão portas de saida e inicio em zero.
  *p_ddrb |= 0x30;
  *p_portb &= ~(0x30);
  
  sei();// habilito as interrupções novamente.
}

ISR(TIMER2_OVF_vect){
  //Toda vez que o programa entra na rotina de temporização as variaveis contadoras são incrrementas.
  conta1++;
  conta2++;
  
  //Crio uma mascara de avaliação das variáveis led13 e led12 para usar como comparação bit a bit no switch case.
  led13 = *p_portb & 0x20;
  led12= *p_portb & 0x10;
  if (conta1 >= 3907){ /* Quando o valor de conta1 chega a 3907, se o led está apago acende e se está aceso apaga. O 
  valor 3907 vem da expressão: 
  T=(2^prescale)* periodo_relogio_entrada e ciclos=tempo/T
  No nosso caso, T=128e-6 e para 0,5s temos 3907 ciclos. Para 0,78s temos 6094 ciclos.
  */
    
  conta1=0; //Ja tempo de 0,5 s foi alcançado a variavel deve ser zerada para recomeçar a contagem.
    switch(led13){
    case (0x20): // caso ligado, desliga
    *p_portb &= ~(0x20);
     break;
    case (0x00):// caso desligado, liga.
    *p_portb |= 0x20;  
    }// Lembrando que esse switch se refere ao led integrado. Todas as mascaras fazem referencia a PB5 (0x20)
  }
    
  if (conta2 >= 6094){ /*Aqui, temos exatemente a mesma logica do ultimo bloco, mas para o GPIO na porta 12
  e com 0,78 s entre cada mudança.*/
    
  	conta2=0;
     switch(led12){
    case (0x10):
    *p_portb &= ~(0x10);
     break;
    case (0x00):
    *p_portb |= 0x10; 
    }    
  }
}

//Essa é a rotina de interrupção de transmissão.
ISR(USART_UDRE_vect){
  if (msg[i] != '\0'){ /*Enquanto a mensagem não terminar (termino marcado por \0), os caracteres são enviados a cada
   vez que a interrupção é chamada. Como ela sempre vai ser chamada desde que esteja habilitada, toda mensagem é enviada.
   */
  *p_udr0 = msg[i];
  i++;
  }
  else { /*Quando a mensagem inteira é enviada, eu zero meu auxiliar de iteração (i) e desabilito a interrupção
  para esperar 5s dentro da main*/
    
   i=0;
  *p_ucsr0b &= ~(0x20);
  }

}
int main (void){
  
  /*Iniciando o setup aqui, as interrupções tambem terão inicio aqui. Logo, a partir desse momento, acontecer o 
  overflow, a execução do programa é redirecionada para a rotina de interrupção associada ao temporizador. No caso do 
  prescale=8, o registrador tem que chegar em 255. */
  
  setup();
  
  while(1){
    _delay_ms(1);//Para o bug do TinkerCad
    
    
    _delay_ms(5000);/*Visto que no primeiro laço o programa ja vai entrar na rotina de interrupção de transmissão,
    e lá eu paro essa interrupção (no final), posso esperar aqui no programa principal os 5 segundos antes de 
    habilitar a interrupção novamente.*/
    
    *p_ucsr0b |= 0x20; // Habilito a interrupção de transmissão.
  }

}