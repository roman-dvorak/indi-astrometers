/*
    AMSKY01 INDI Weather Station Driver
    Simple and clean implementation for AMSKY01 weather station
    
    Protocol: Serial CSV data with $light, $cloud, $hygro messages
    
    Author: Roman Dvořák <info@astrometers.cz>
    Copyright (C) 2025 Astrometers
*/

#pragma once

#include <libindi/indiweather.h>
#include <libindi/connectionplugins/connectionserial.h>

namespace Connection
{
    class Serial;
}

class AMSKY01 : public INDI::Weather
{
public:
    AMSKY01();
    virtual ~AMSKY01();
    
    virtual bool initProperties() override;
    virtual bool updateProperties() override;
    virtual const char *getDefaultName() override;
    
    virtual IPState updateWeather() override;
    
protected:
    virtual void TimerHit() override;
    virtual bool ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n) override;

private:
    // Serial connection - handled by base Weather class
    bool Handshake();
    bool sendCommand(const char *cmd);
    
    // Properties - pouze základní status
    ITextVectorProperty StatusTP;
    IText StatusT[2];  // Device a Status
    
    // Data reading
    bool readSerialData();
    void processData(const std::string& data);
    
    // Weather data parsing
    bool parseHygro(const std::string& data);
    bool parseLight(const std::string& data);
    bool parseCloud(const std::string& data);
    
    // Weather values podle skutečných AMSKY01 dat
    struct {
        // Hygro sensor
        double temperature = 0.0;   // °C
        double humidity = 0.0;      // %
        double dewPoint = 0.0;      // °C (vypočítané)
        
        // Light sensor  
        double lux = 0.0;           // světelnost
        int raw1 = 0, raw2 = 0;     // raw hodnoty
        int gain = 0;               // zesílení
        int integrationTime = 0;    // integrační čas ms
        double skyBrightness = 0.0; // mag/arcsec² (vypočítané)
        
        // Cloud sensor (thermopile)
        double cloudTemp[5] = {0};  // 5 teplot oblohy (4 segmenty + zenit)
        double avgCloudTemp = 0.0;  // průměrná teplota oblohy
        double cloudCover = 0.0;    // oblačnost % (vypočítané)
        
        bool hygroValid = false;
        bool lightValid = false;
        bool cloudValid = false;
        bool dataValid = false;
    } weatherData;
};
