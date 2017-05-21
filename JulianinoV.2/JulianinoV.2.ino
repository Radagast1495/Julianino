// DESCRIPCION

// Este es el codigo del Julianino, v0.3
// Fue escrito por Teo Willink en carrera para la presentacion para FundacionUCR
// Esteban, Juanjo y Pablo deberian hacer su propio codigo

// LIBRERIAS USADAS

#include <SoftwareSerial.h>
#include "Adafruit_Soundboard.h"

// DEFINICIONES

// Pines para el reproductor
#define SFX_RST 14
#define SFX_TX 15
#define SFX_RX 16
#define SFX_ACT 17

// Pines especiales
#define REF_DER 13

// Cantidad de frutas
#define FRUTAS 9

// Orden del filtro digital
#define ORDEN 32

// PARAMETROS AJUSTABLES

// Entradas utilizadas para las diferentes frutas
int pinesFrutas[FRUTAS] = {
  2,3,4,5,6,7,8,9,10};
  
// Numero de pista signada a cada fruta
unsigned char sonidoFruta[FRUTAS] = {
  1,2,3,4,5,6,7,8,9};

// VARIABLES INTERNAS

// Banderas con el estado de cada fruta
boolean frutaActiva[FRUTAS] = {
  false,false,false,false,false,false,false,false,false};

// Buffers con el estado anterior de cada fruta
int bufferFrutas[ORDEN][FRUTAS];

// Promedio de los estados anteriores de cada fruta
int promedioFruta = 0;

// Estado de la fruta que se esta leyendo
boolean estadoFruta = false;

// OTRAS VARIABLES

// Puerto serial por software
SoftwareSerial ss = SoftwareSerial(SFX_TX, SFX_RX);

// Reproductor
Adafruit_Soundboard sfx = Adafruit_Soundboard(&ss, NULL, SFX_RST);

// INICIALIZACION

void setup() {
  
  // Puerto serie (solo para despulgar)
  Serial.begin(9600);
  
  // Bienvenida
  Serial.println("Este es el Julianino, v0.2");
  delay(1000);
  
  // Anuncio
  Serial.println("Primero hay que preparar algunas cosas:");
  delay(1000);
  
  // Puerto serie (para comunicarse con el reproductor)
  Serial.print("Iniciando el reproductor... ");
  ss.begin(9600);

  // Reproductor
  pinMode(SFX_ACT,INPUT);
  if (!sfx.reset()) {
    Serial.println("fallo :(");
    while (1);
  }
  else {
    Serial.println("listo");
  }
  
  // Estado de los pines de las frutas
  Serial.print("Iniciando las frutas... ");
  for (int fruta = 0; fruta < FRUTAS; fruta++)
  {
    pinMode(pinesFrutas[fruta],INPUT);
  }
  /*pinMode(pinesFrutas[0],INPUT);
  pinMode(pinesFrutas[1],INPUT);
  pinMode(pinesFrutas[2],INPUT);
  pinMode(pinesFrutas[3],INPUT);
  pinMode(pinesFrutas[4],INPUT);
  pinMode(pinesFrutas[5],INPUT);
  pinMode(pinesFrutas[6],INPUT);*/
  for (int i = 0; i < ORDEN; i++)
  {
    for (int f = 0; f < FRUTAS; f++)
    {
      bufferFrutas[i][f] = 0;
    }
  }
  pinMode(REF_DER,OUTPUT);
  digitalWrite(REF_DER,HIGH);
  delay(1000);
  Serial.println("listo");
  
  // Anuncio
  Serial.println("Listo, ya se puede tocar!");
  delay(200);
}

// LAZO

void loop() {
   // Revisar las diferentes frutas
  for (int fruta = 0; fruta < FRUTAS; fruta++)
  {
    // Rotar el buffer del filtro
    //Serial.println("Se rota el buffer del filtro");
    for (int i = ORDEN; i >= 1; i--)
    {
      bufferFrutas[i][fruta] = bufferFrutas[i-1][fruta];
    }
    
    // Leer el estado de la fruta
    //estadoFruta = digitalRead(pinesFrutas[fruta]);
    bufferFrutas[0][fruta] = digitalRead(pinesFrutas[fruta]);
    if (fruta == 1) {
      Serial.print("El valor de la muestra actual es ");
      Serial.println(int(bufferFrutas[0][fruta]));
    }
    
    // Aplicar el filtro
    if (fruta == 1)
      Serial.print("El promedio se calcula sumando: ");
    promedioFruta = 0;
    for (int i = 0; i < ORDEN; i++)
    {
      promedioFruta = promedioFruta + bufferFrutas[i][fruta];
      if (fruta == 1) {
        Serial.print(bufferFrutas[i][fruta]);
        Serial.print("+");
      }
    }
    if (fruta == 1)
      Serial.println("nada :)");
    if (fruta == 1) {
      Serial.print("El promedio de las muestras anteriores es ");
      Serial.println(promedioFruta);
    }
    estadoFruta = HIGH;
    if (promedioFruta < 7*ORDEN/8) {
      estadoFruta = LOW;
    }
    if (fruta == 1) {
      Serial.print("El estado de la fruta es ");
      Serial.println(estadoFruta);
    }
    
    // Revisar el estado de la fruta
    if (estadoFruta == LOW)
    {
      // Revisar si la fruta esta activa
      if (frutaActiva[fruta] == true)
      {
        Serial.print("Se solto la fruta ");
        Serial.print(fruta);
        Serial.println("!");
        
        // Revisar si alguna fruta estaba sonando
        if ( ! digitalRead(SFX_ACT) ) {
          // Detener el sonido asignado a la fruta
          Serial.print("Deteniendo sonido anterior... ");
          if (! sfx.stop() ) {
            Serial.println("fallo");
          }
          else {
            Serial.println("listo");
          }
        }

        // Marcar la fruta como desactivada
        frutaActiva[fruta] = false;
      }
    }
    else
    {
      // Revisar si la fruta esta inactiva
      if (frutaActiva[fruta] == false)
      {
        Serial.print("Se toco la fruta ");
        Serial.print(fruta);
        Serial.println("!");
        
        // Revisar si alguna fruta estaba sonando
        if ( !digitalRead(SFX_ACT) ) {
          // Detener el sonido asignado a la fruta
          Serial.print("Deteniendo sonido anterior... ");
          if (! sfx.stop() ) {
            Serial.println("fallo");
          }
          else {
            Serial.println("listo");
          }
        }
        
        // Iniciar el sonido asignado a la fruta
        Serial.print("Sonando la fruta... ");
        if (! sfx.playTrack(sonidoFruta[fruta]) ) {
          Serial.println("fallo");
        }
        else {
          Serial.println("listo");
        }

        // Marcar la fruta como activa
        frutaActiva[fruta] = true;
      }
    }
  }
  delay(10);
}
