#ifndef ANEMOMETRO_CPP
#define ANEMOMETRO_CPP

#include "Anemometro.h"

Anemometro::Anemometro(AnConf *cfg,SoftwareSerial *Srl){
    conf = cfg;
    RS485 = Srl;
}


void Anemometro::start(){

    mtx = xSemaphoreCreateMutex();
    
    MB.begin(conf->id,*RS485);

    xTaskCreatePinnedToCore([](void *param){
        Anemometro *th = (Anemometro*)param;
        TickType_t ini;
        for(;;){
            ini = xTaskGetTickCount();
            th->refresh();
            vTaskDelayUntil(&ini,th->getIntervalo());
        }
    },"Refresh Temp",5000,this,1,NULL,0);
}


void Anemometro::refresh(){
    uint8_t err;
    err = MB.readHoldingRegisters(conf->Add,2);
    if(err == ModbusMaster::ku8MBSuccess){
        if(xSemaphoreTake(mtx,1000)){
            velocidade = MB.getResponseBuffer(0)*0.01;
            Serial.printf("Velocidade: %f\n",velocidade);
            xSemaphoreGive(mtx);
        }
    }
    else{
        Serial.printf("Nao foi possivel obter Velocidade! Erro: %d\n",err);
    }
}

Registrador Anemometro::getVelocidade(){
    Registrador out;
    if(xSemaphoreTake(mtx,1000)){
        out = velocidade;
        xSemaphoreGive(mtx);
    }
    return out;
}



Periodo Anemometro::getIntervalo(){
    Periodo out;
    if(xSemaphoreTake(mtx,1000)){
        out = conf->intervalo;
        xSemaphoreGive(mtx);
    }
    return out;
}

#endif