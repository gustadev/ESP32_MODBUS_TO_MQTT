#ifndef ANEMOMETRO_H
#define ANEMOMETRO_H

#include <Arduino.h>
#include <stdio.h>
#include "SoftwareSerial.h"
#include "ModbusMaster.h"

typedef uint8_t Id;
typedef uint8_t Address;
typedef float Registrador; 
typedef uint16_t Periodo;

typedef struct {
    Id      id;
    Address Add;
    Periodo intervalo;    
} AnConf;


class Anemometro {

    AnConf *conf;
    SoftwareSerial *RS485;
    ModbusMaster MB;
    Registrador velocidade;
    SemaphoreHandle_t mtx;


    public:
    Anemometro(AnConf *conf,SoftwareSerial *Srl);
    void start();
    void refresh();
    Registrador getVelocidade();
    Periodo     getIntervalo();
    Address     getAddress();
    
};

#endif