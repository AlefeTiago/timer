O desafio proposto nesta atividade é desenvolver um programa que faça com que dois LEDs pisquem em
diferentes frequências, ao mesmo tempo em que uma mensagem de texto é continuamente transmitida pela
UART. Mais especificamente, desejamos que o LED incorporado pisque com uma frequência de 1 Hz (0,5 s
aceso, 0,5 s apagado), e que um LED externo, conectado ao pino 12 da placa Arduino, pisque de forma a
ficar 0,78 s aceso e 0,78 s apagado.

A solução para a atividade deve necessariamente explorar a interrupção do timer 2 associada ao modo Clear
Timer on Compare Match (CTC). As especificações do temporizador (e.g., prescaler e valor máximo da
contagem) devem ser projetadas por cada aluno e justificadas nos comentários do código-fonte. Notem que
há várias maneiras de se configurar o temporizador para obter o comportamento desejado do sistema. Não
é permitido utilizar a função _delay_ms() ou qualquer outro tipo de espera ativa para temporizar os LEDs.
Em relação à UART, vamos utilizar a mesma configuração da atividade 6 (interrupções). Além disso, assim
como feito anteriormente, após a mensagem de texto ser enviada uma vez, deve ser introduzido um atraso
de 5 segundos utilizando a função _delay_ms() antes de se iniciar a nova transmissão. Substituam o vetor
de caracteres a ser transmitido pela seguinte mensagem:
char msg[] = "Atividade 7 – Interrupcoes temporizadas tratam concorrencia entre tarefas! \n\n";

Observação: uma abordagem baseada somente em espera ativa (i.e., que usa rotinas de atraso para gastar
tempo) encontraria algumas dificuldades para manter os vários dispositivos (LEDs e UART)
simultaneamente em operação, dada a concorrência existente entre as tarefas.

Especificações da USART:
- Velocidade de transmissão normal (i.e., modo double-speed desativado);
- Modo de transmissão multi-processador desabilitado;
- Número de bits de dados por frame igual a 8;
- Modo assíncrono de funcionamento da USART;
- Sem bits de paridade;
- Uso de um bit de parada;
- Baud rate igual a 19.200 bps.
