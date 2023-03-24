/*  
*  AUTOR:    BrincandoComIdeias
*  APRENDA:  https://cursodearduino.net/
*  SKETCH:   Sensor Fim de Curso
*  DATA:     07/03/23
*/

#include <Nextion.h>

// DEFINIÇÕES DE PINOS
#define pinLdr A0
#define pinAbrir 9
#define pinFechar 10

#define pinFimAbre 4
#define pinFimFecha 5

#define pinEspAbrir 2
#define pinEspFechar 3

// DEFINIÇÕES
#define LIGA LOW
#define DESLIGA HIGH

#define luzAbrir 30
#define luzFechar 50

#define SUBINDO 2
#define ABERTA 1
#define PARADA 0
#define FECHADA -1
#define DESCENDO -2

#define AUTO 1
#define MANUAL 0

#define intervalo 5000

// VARIÁVEIS GLOBAIS
int persiana;
int modo = MANUAL;
unsigned long ultimaLeitura;

// INSTANCIANDO OBJETOS
NexButton btAbre = NexButton(1, 1, "b0");
NexButton btFecha = NexButton(1, 2, "b1");
NexButton btStop = NexButton(1, 8, "b2");
NexDSButton btModo = NexDSButton(1, 4, "bt0");
NexNumber nvLuz = NexNumber(1, 6, "n0");

NexTouch *nex_listen_list[] = {
  &btAbre,
  &btFecha,
  &btModo,
  &btStop,
  NULL
};

// DECLARANDO FUNÇÕES
bool abrirPersiana();
bool fecharPersiana();

void btAbreApertado(void *ptr);
void btAbreSolto(void *ptr);
void btFechaApertado(void *ptr);
void btFechaSolto(void *ptr);
void btModoApertado(void *ptr);
void btStopApertado(void *ptr);

void setup() {
  nexInit();

  btAbre.attachPush(btAbreApertado);
  btAbre.attachPop(btAbreSolto);
  btFecha.attachPush(btFechaApertado);
  btFecha.attachPop(btFechaSolto);
  btModo.attachPush(btModoApertado);
  btStop.attachPush(btStopApertado);

  // CONFIGURAÇÃO DOS PINOS
  pinMode(pinLdr, INPUT);

  pinMode(pinFimAbre, INPUT_PULLUP);
  pinMode(pinFimFecha, INPUT_PULLUP);

  pinMode(pinEspAbrir, INPUT);
  pinMode(pinEspFechar, INPUT);

  pinMode(pinAbrir, OUTPUT);
  pinMode(pinFechar, OUTPUT);

  pinMode(13, OUTPUT);  // DEBUG

  // CONFIRMA O MOTOR PARADO
  digitalWrite(pinAbrir, DESLIGA);
  digitalWrite(pinFechar, DESLIGA);

  // VERIFICANDO POSICAO DA CORTINA AO INICIAR
  if (!digitalRead(pinFimAbre)) {
    persiana = ABERTA;
  } else if (!digitalRead(pinFimFecha)) {
    persiana = FECHADA;
  } else {
    persiana = PARADA;
  }

  btModo.setValue(modo);
  btModo.setText("Manual");
}

void loop() {
  nexLoop(nex_listen_list);

  static bool espAbrirAnt = 0;
  static bool espFecharAnt = 0;

  bool espAbrir = digitalRead(pinEspAbrir);
  bool espFechar = digitalRead(pinEspFechar);

  if (espAbrir && espFechar) {

    // NÃO FAZ NADA, ERRO NO ESP!

  } else {

    if (espAbrir && !espAbrirAnt) {
      if (persiana != ABERTA && persiana != SUBINDO) {
        modo = MANUAL;  // MODO MANUAL PARA O SENSOR NÃO FECHAR A PERSIANA
        btModo.setValue(modo);
        btModo.setText("Manual");

        if (abrirPersiana()) persiana = ABERTA;
        else return;
      }
    }

    if (espFechar && !espFecharAnt) {
      if (persiana >= PARADA) {  // (persiana != FECHADA && persiana != DESCENDO)
        modo = MANUAL;           // MODO MANUAL PARA O SENSOR NÃO FECHAR A PERSIANA
        btModo.setValue(modo);
        btModo.setText("Manual");

        if (fecharPersiana()) persiana = FECHADA;
        else return;
      }
    }
  }

  if (millis() - ultimaLeitura > intervalo) {
    /* Leitura do sensor */
    int leitura = analogRead(pinLdr);

    /* Descomentar abaixo para inverter a lógica do sensor analógico */
    //leitura = map(leitura, 0, 1023, 1023, 0);

    /* Converte o valor analogico para porcentagem */
    leitura = map(leitura, 0, 1023, 0, 100);

    /* Envia o valor da luminosidade para o Nextion */
    if (persiana != SUBINDO && persiana != DESCENDO)
      nvLuz.setValue(leitura);

    if (modo == AUTO) {

      if (persiana != ABERTA) {
        if (leitura < luzAbrir) {
          if (abrirPersiana()) persiana = ABERTA;
          else return;
        }
      }

      if (persiana != FECHADA) {
        if (leitura > luzFechar) {
          if (fecharPersiana()) persiana = FECHADA;
          else return;
        }
      }
    }

    ultimaLeitura = millis();
  }

  if (modo == MANUAL) {
    if (!digitalRead(pinFimAbre) && persiana == SUBINDO) {
      digitalWrite(pinAbrir, DESLIGA);
      digitalWrite(pinFechar, DESLIGA);

      persiana = ABERTA;
    }

    if (!digitalRead(pinFimFecha) && persiana == DESCENDO) {
      digitalWrite(pinAbrir, DESLIGA);
      digitalWrite(pinFechar, DESLIGA);

      persiana = FECHADA;
    }
  }

  /* Fica piscando o led13 para mostrar que esta rodando no loop() */
  digitalWrite(13, bitRead(millis(), 8));
}



bool abrirPersiana() {
  /* Liga o motor */
  digitalWrite(pinAbrir, LIGA);
  digitalWrite(pinFechar, DESLIGA);

  /* Não faz nada até que o fim de curso seja pressionado */
  while (digitalRead(pinFimAbre)) {
    if (Serial.available()) {
      return false;
    }
  }

  /* Desliga o motor */
  digitalWrite(pinAbrir, DESLIGA);
  digitalWrite(pinFechar, DESLIGA);

  return true;
}

bool fecharPersiana() {
  /* Liga o motor */
  digitalWrite(pinAbrir, DESLIGA);
  digitalWrite(pinFechar, LIGA);

  /* Não faz nada até que o fim de curso seja pressionado */
  while (digitalRead(pinFimFecha)) {
    if (Serial.available()) {
      return false;
    }
  }

  /* Desliga o motor */
  digitalWrite(pinAbrir, DESLIGA);
  digitalWrite(pinFechar, DESLIGA);
  return true;
}

void btAbreApertado(void *ptr) {
  /* VERIFICA SE NÃO ESTÁ NO FIM DE CURSO */
  if (digitalRead(pinFimAbre)) {
    modo = MANUAL;
    btModo.setValue(modo);
    btModo.setText("Manual");

    /* Liga o motor */
    digitalWrite(pinAbrir, LIGA);
    digitalWrite(pinFechar, DESLIGA);

    persiana = SUBINDO;
  }
}

void btAbreSolto(void *ptr) {
  /* Desliga o motor */
  digitalWrite(pinAbrir, DESLIGA);
  digitalWrite(pinFechar, DESLIGA);

  persiana = PARADA;
}

void btFechaApertado(void *ptr) {
  /* Verificar se não está no fim de curso */
  if (digitalRead(pinFimFecha)) {
    modo = MANUAL;
    btModo.setValue(modo);
    btModo.setText("Manual");

    /* Liga o motor */
    digitalWrite(pinAbrir, DESLIGA);
    digitalWrite(pinFechar, LIGA);

    persiana = DESCENDO;
  }
}

void btFechaSolto(void *ptr) {
  /* Desliga o motor */
  digitalWrite(pinAbrir, DESLIGA);
  digitalWrite(pinFechar, DESLIGA);

  persiana = PARADA;
}

void btModoApertado(void *ptr) {
  uint32_t estadoBot;

  btModo.getValue(&estadoBot);
  modo = estadoBot;
  if (estadoBot == AUTO) {
    btModo.setText("Auto");
  } else if (estadoBot == MANUAL) {
    btModo.setText("Manual");
  }
}

void btStopApertado(void *ptr) {
  /* Desliga o motor */
  digitalWrite(pinAbrir, DESLIGA);
  digitalWrite(pinFechar, DESLIGA);

  modo = MANUAL;           // MODO MANUAL PARA O SENSOR NÃO FECHAR A PERSIANA
  btModo.setValue(modo);
  btModo.setText("Manual");
  persiana = PARADA;
}