#pragma once
#include "OpenKNX.h"
#include "hardware.h"
#include "knxprod.h"
#include "INA226.h"
#ifdef OPENKNX_BPS_TEMPSENS_ADDR
  #include <Temperature_LM75_Derived.h>
#endif

// #define OPENKNX_BPS_FLASH_VERSION 0
// #define OPENKNX_BPS_FLASH_MAGIC_WORD 0

#define CH_SWITCH_DEBOUNCE 250

#define TEMPERATURE_MIN_DIFFERENCE 0.5
#define VOLTAGE_MIN_DIFFERENCE 500
#define CURRENT_MIN_DIFFERENCE 50
#define LOAD_MIN_DIFFERENCE 5

#define BUS_LOAD_UPDATE_RATE 1000
#define BUS_LOAD_MAX_BYTES_PER_SECOND 800

class BusPowerSupplyModule : public OpenKNX::Module
{
  public:
    BusPowerSupplyModule();
    ~BusPowerSupplyModule();

    void processInputKo(GroupObject &ko);
    void setup(bool configured);
    void loop();

    void writeFlash() override;
    void readFlash(const uint8_t* data, const uint16_t size) override;
    uint16_t flashSize() override;

    void savePower() override;
    bool restorePower() override;

    const std::string name() override;
    const std::string version() override;

    float estimateBusLoad();

    void showHelp() override;
    bool processCommand(const std::string cmd, bool diagnoseKo) override;
    void runTestMode();

  private:
    INA226 _inaKnx = INA226(OPENKNX_BPS_CURRENT_KNX_INA_ADDR, &OPENKNX_GPIO_WIRE);
    INA226 _inaAux = INA226(OPENKNX_BPS_CURRENT_AUX_INA_ADDR, &OPENKNX_GPIO_WIRE);

#ifdef OPENKNX_BPS_TEMPSENS_ADDR
    Generic_LM75_9_to_12Bit_OneShot _temperature = Generic_LM75_9_to_12Bit_OneShot(&OPENKNX_GPIO_WIRE, OPENKNX_BPS_TEMPSENS_ADDR);
#endif

    float _lastBusVoltageSent = 0;
    float _lastBusCurrentSent = 0;
    float _lastBusLoadSent = 0;
    float _lastAuxVoltageSent = 0;
    float _lastAuxCurrentSent = 0;
    float _lastTemperatureSent = 0;
    uint32_t _busVoltageSentTimer = 0;
    uint32_t _busCurrentSentTimer = 0;
    uint32_t _busLoadSentTimer = 0;
    uint32_t _auxVoltageSentTimer = 0;
    uint32_t _auxCurrentSentTimer = 0;
    uint32_t _temperaturSentTimer = 0;

    uint32_t _busLoadUpdateTimer = 0;
    uint32_t _rxLastBusLoadTime;
    uint32_t _rxLastBusBytes;

    uint32_t _debugTimer = 0;
};

extern BusPowerSupplyModule openknxBusPowerSupplyModule;