// Universidad del Valle de Guatemala
// BE3023 Digital 2
// Marre Ponce 22601
// Camila Santiago 22572
// Laboratorio 3
// Micro: ESP32 DEV Kit 1.0
// Enciende leds con los botones

#include <Arduino.h>
#include <stdint.h>


const int ledPins[] = {32, 33, 26,27}; // Pines de los LEDs del contador manual
const int numLeds = 4; // Número de LEDs
int counter = 0; // Contador para el modo binario
int countertimer = 0;
const int ledPins2[] = {23,22,14,21}; //Pines de los LEDS del temporizador
int conttimer;
int maxmanual;

#define ledalarm 19
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

// la estructura de los 3 botones
struct Button {
  const uint8_t PIN;
  bool pressed;
  bool laststate;
};


struct Button2 {
  const uint8_t PIN;
  bool pressed;
  bool laststate;
};

struct Button3 {
  const uint8_t PIN;
  bool pressed;
  bool laststate;
};



Button button1 = {13, false, false};
Button2 button2 = {25, false, false};
Button3 button3 = {12, false, false};
//Paso1
//Paso 1: Instanciar configuración timer

hw_timer_t *timer0 = NULL;

//Prototipo de funcion
void initTimer0(void);

// las interrupciones de los 3 botones
void IRAM_ATTR BTN1_ISR(){
  portENTER_CRITICAL_ISR(&mux);
    button1.pressed = true;
  portEXIT_CRITICAL_ISR(&mux);
}

void IRAM_ATTR BTN2_ISR(){
  portENTER_CRITICAL_ISR(&mux);
    button2.pressed = true;
  portEXIT_CRITICAL_ISR(&mux);
}

void IRAM_ATTR BTN3_ISR(){
  portENTER_CRITICAL_ISR(&mux);
    button3.pressed = true;
  portEXIT_CRITICAL_ISR(&mux);
}

//la interrupción del temporizador, aquí de una vez se encienden los leds
void IRAM_ATTR Timer0_ISR (void){
  conttimer++;
  countertimer = (conttimer - 1 + (1 << numLeds)) % (1 << numLeds);
  for (int i = 0; i < numLeds; i++) {
  digitalWrite(ledPins[i], (countertimer >> i) & 1);
    }
}


void setup() {

  Serial.begin(115200);


  //Paso 2
  for (int i = 0; i < numLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
    pinMode(ledPins2[i], OUTPUT);
  }
  pinMode(button1.PIN, INPUT_PULLUP);
  pinMode(button2.PIN, INPUT_PULLUP);
  pinMode(button3.PIN, INPUT_PULLUP);
  pinMode(ledalarm, OUTPUT);
  attachInterrupt(button1.PIN,  BTN1_ISR, RISING);
  attachInterrupt(button2.PIN, BTN2_ISR, RISING);
  attachInterrupt(button3.PIN, BTN3_ISR, RISING);
   initTimer0();
}



void loop() {

// condición para saber si se presionó el botón y aumentar el contador
  if(button1.pressed != button1.laststate){
    if(button1.pressed){

      maxmanual++;
      Serial.printf("Buttonmax has been pressed %u times\n", maxmanual);
      if(maxmanual >= 15){
        maxmanual=-1;
      }     
      button1.pressed = false;
    }
  }

  // condición para saber si se presionó el botón y decrementar el contador
  if(button2.pressed != button2.laststate){
    if(button2.pressed){

    maxmanual--;
    Serial.printf("Buttondec has been pressed %u times\n", maxmanual);     
    if(maxmanual <= 0){
      maxmanual=16;
    }
    }
    button2.pressed = false;
  }

// condición para cambiar el estado de la led cada vez que el contador del timer y del manual sean iguales
  if(counter == countertimer){
    int togleled = digitalRead(ledalarm);
    digitalWrite(ledalarm, !togleled);
  }

//condición para resetear el contador del timer
    if(button3.pressed != button3.laststate){
    if(button3.pressed){
 
      conttimer =0;
      Serial.printf("reseteo has been pressed %u times\n", conttimer);     
    
      button3.pressed = false;
    }
  }
//encendido de las leds del timer manual
  counter = (maxmanual - 1 + (1 << numLeds)) % (1 << numLeds);
  for (int i = 0; i < numLeds; i++) {
  digitalWrite(ledPins2[i], (counter >> i) & 1);
    }
    

//cosas para antirebote
  button1.laststate = button1.pressed;
  button2.laststate = button2.pressed;
  button3.laststate = button3.pressed;
  

  // Pequeño retardo para evitar el rebote del botón
  delay(50);


}


//funcion para inicializar el timer 0
void initTimer0(void){
  //Paso 2: Seleccionar #Timer, Prescaler, Flag
  timer0 = timerBegin(0,80,true);
  //Paso 3: Definir función de ISR(HANDLER)
  timerAttachInterrupt(timer0,&Timer0_ISR, true);
  //Paso 4: Establecer alarma (TimerTicks) y si queremos reload
  timerAlarmWrite(timer0,250000, true);
  // Paso 5: Habilitar la alarma
  timerAlarmEnable(timer0);
}