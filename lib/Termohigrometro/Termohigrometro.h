#ifndef TERMOHIGROMETRO_H
#define TERMOHIGROMETRO_H

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
    Address AddTemp;
    Address AddUmid;
    Periodo intervalo;    
} THConf;


class Termohigrometro {

    THConf *conf;
    SoftwareSerial *RS485;
    ModbusMaster MB;
    Registrador temperatura;
    Registrador umidade;
    SemaphoreHandle_t mtx;

    public:
    Termohigrometro(THConf *conf,SoftwareSerial *Srl);
    void start();
    void refresh();
    Registrador getTemperatura();
    Registrador getUmidade();
    Periodo     getIntervalo();
    Address     getAddressTmp();
    Address     getAddressUmd();

    
};

#endif