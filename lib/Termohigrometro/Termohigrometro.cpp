#ifndef TERMOHIGROMETRO_CPP
#define TERMOHIGROMETRO_CPP

#include "Termohigrometro.h"

Termohigrometro::Termohigrometro(THConf *cfg,SoftwareSerial *Srl){
    conf = cfg;
    RS485 = Srl;
}


void Termohigrometro::start(){

    mtx = xSemaphoreCreateMutex();
    
    MB.begin(conf->id,*RS485);

    xTaskCreatePinnedToCore([](void *param){
        Termohigrometro *th = (Termohigrometro*)param;
        TickType_t ini;
        for(;;){
            ini = xTaskGetTickCount();
            th->refresh();
            vTaskDelayUntil(&ini,th->getIntervalo());
        }
    },"Refresh Temp",5000,this,1,NULL,0);
}


void Termohigrometro::refresh(){
    if(MB.readHoldingRegisters(conf->AddTemp,1) == ModbusMaster::ku8MBSuccess){
        if(xSemaphoreTake(mtx,1000)){
            temperatura = MB.getResponseBuffer(0)*0.1;
            //Serial.printf("Temperatura: %f\n",temperatura);
            xSemaphoreGive(mtx);
        }
    }
    else{
        Serial.println("Nao foi possivel obter Temperatura!");
    }
    if(MB.readHoldingRegisters(conf->AddUmid,1) == ModbusMaster::ku8MBSuccess){
        if(xSemaphoreTake(mtx,1000)){
            umidade = MB.getResponseBuffer(0)*0.1;
            //Serial.printf("Umidade: %f\n",umidade);
            xSemaphoreGive(mtx);
        }
    }
    else{
        Serial.println("Nao foi possivel obter Umidade!");
    }
}

Registrador Termohigrometro::getTemperatura(){
    Registrador out;
    if(xSemaphoreTake(mtx,1000)){
        out = temperatura;
        xSemaphoreGive(mtx);
    }
    return out;
}

Registrador Termohigrometro::getUmidade(){
    Registrador out;
    if(xSemaphoreTake(mtx,1000)){
        out = umidade;
        xSemaphoreGive(mtx);
    }
    return out;
}

Periodo Termohigrometro::getIntervalo(){
    Periodo out;
    if(xSemaphoreTake(mtx,1000)){
        out = conf->intervalo;
        xSemaphoreGive(mtx);
    }
    return out;
}

#endif