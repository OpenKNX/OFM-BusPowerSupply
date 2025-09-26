#include "BusPowerSupplyModule.h"
#include "OpenKNX.h"
#include "ModuleVersionCheck.h"

BusPowerSupplyModule openknxBusPowerSupplyModule;

BusPowerSupplyModule::BusPowerSupplyModule()
{
}

BusPowerSupplyModule::~BusPowerSupplyModule()
{
}

const std::string BusPowerSupplyModule::name()
{
    return "BusPower";
}

const std::string BusPowerSupplyModule::version()
{
    return MODULE_BusPowerSupply_Version;
}

void BusPowerSupplyModule::processInputKo(GroupObject &ko)
{
    logDebugP("processInputKo");
    logIndentUp();

    uint16_t asap = ko.asap();
    switch (asap)
    {
    }

    logIndentDown();
}

void BusPowerSupplyModule::setup(bool configured)
{
    openknx.gpio.pinMode(OPENKNX_BPS_PWR1_CHECK_PIN, INPUT);
    openknx.gpio.pinMode(OPENKNX_BPS_PWR1_SWITCH_ON_PIN, OUTPUT, true, !OPENKNX_BPS_PWR_SWITCH_ACTIVE_ON);
    openknx.gpio.pinMode(OPENKNX_BPS_PWR1_SWITCH_OFF_PIN, OUTPUT, true, !OPENKNX_BPS_PWR_SWITCH_ACTIVE_ON);
    openknx.gpio.pinMode(OPENKNX_BPS_PWR1_ALERT_PIN, INPUT);

    openknx.gpio.pinMode(OPENKNX_BPS_PWR2_CHECK_PIN, INPUT);
    openknx.gpio.pinMode(OPENKNX_BPS_PWR2_SWITCH_ON_PIN, OUTPUT, true, !OPENKNX_BPS_PWR_SWITCH_ACTIVE_ON);
    openknx.gpio.pinMode(OPENKNX_BPS_PWR2_SWITCH_OFF_PIN, OUTPUT, true, !OPENKNX_BPS_PWR_SWITCH_ACTIVE_ON);
    openknx.gpio.pinMode(OPENKNX_BPS_PWR2_ALERT_PIN, INPUT);

    openknx.gpio.pinMode(OPENKNX_BPS_STATUS_BUS, OUTPUT, true, !OPENKNX_BPS_STATUS_ACTIVE_ON);
    openknx.gpio.pinMode(OPENKNX_BPS_STATUS_TMP, OUTPUT, true, !OPENKNX_BPS_STATUS_ACTIVE_ON);
    openknx.gpio.pinMode(OPENKNX_BPS_STATUS_TRC, OUTPUT, true, !OPENKNX_BPS_STATUS_ACTIVE_ON);
    openknx.gpio.pinMode(OPENKNX_BPS_STATUS_DEV, OUTPUT, true, !OPENKNX_BPS_STATUS_ACTIVE_ON);
    openknx.gpio.pinMode(OPENKNX_BPS_STATUS_PW1, OUTPUT, true, !OPENKNX_BPS_STATUS_ACTIVE_ON);
    openknx.gpio.pinMode(OPENKNX_BPS_STATUS_PW2, OUTPUT, true, !OPENKNX_BPS_STATUS_ACTIVE_ON);
    openknx.gpio.pinMode(OPENKNX_BPS_STATUS_MAX, OUTPUT, true, !OPENKNX_BPS_STATUS_ACTIVE_ON);
    openknx.gpio.pinMode(OPENKNX_BPS_STATUS_RST, OUTPUT, true, !OPENKNX_BPS_STATUS_ACTIVE_ON);
    openknx.gpio.pinMode(OPENKNX_BPS_SWITCH_RST, INPUT);

    analogReadResolution(12);

    if (_inaKnx.begin())
    {
        logDebugP("KNX INA226 setup done with address %u", _inaKnx.getAddress());
        logIndentUp();

        uint32_t result = _inaKnx.setMaxCurrentShunt(3, OPENKNX_BPS_CURRENT_KNX_INA_SHUNT);
        if (result != 0)
            logDebugP("KNX INA226 setMaxCurrentShunt failed with error code %u", result);
        _inaKnx.setModeShuntContinuous();

#ifdef OPENKNX_DEBUG
        delay(1000);
        logDebugP("getMode %u", _inaKnx.getMode());
        logDebugP("isCalibrated %u", _inaKnx.isCalibrated());
        logDebugP("getCurrentLSB %.4f", _inaKnx.getCurrentLSB());
        logDebugP("getShunt %.4f", _inaKnx.getShunt());
        logDebugP("getMaxCurrent %.4f", _inaKnx.getMaxCurrent());
#endif
        logIndentDown();
    }
    else
        logDebugP("KNX INA226 not found at address %u", _inaKnx.getAddress());

    if (_inaAux.begin())
    {
        logDebugP("AUX INA226 setup done with address %u", _inaAux.getAddress());
        logIndentUp();

        uint32_t result = _inaAux.setMaxCurrentShunt(3, OPENKNX_BPS_CURRENT_AUX_INA_SHUNT);
        if (result != 0)
            logDebugP("AUX INA226 setMaxCurrentShunt failed with error code %u", result);
        _inaAux.setModeShuntContinuous();

#ifdef OPENKNX_DEBUG
        delay(1000);
        logDebugP("getMode %u", _inaAux.getMode());
        logDebugP("isCalibrated %u", _inaAux.isCalibrated());
        logDebugP("getCurrentLSB %.4f", _inaAux.getCurrentLSB());
        logDebugP("getShunt %.4f", _inaAux.getShunt());
        logDebugP("getMaxCurrent %.4f", _inaAux.getMaxCurrent());
#endif
        logIndentDown();
    }
    else
        logDebugP("AUX INA226 not found at address %u", _inaAux.getAddress());
}

float BusPowerSupplyModule::estimateBusLoad()
{
    TpUartDataLinkLayer* dll = knx.bau().getDataLinkLayer();
    TPUart::Statistics& statistics = dll->getTPUart().getStatistics();
    uint32_t currentTime = millis();
    uint32_t timeDiff = currentTime - _rxLastBusLoadTime;
    uint32_t currentBytes = statistics.getRxBusBytes();
    uint32_t bytesPerSecond = (currentBytes - _rxLastBusBytes) / timeDiff * 1000;
    _rxLastBusLoadTime = currentTime;
    _rxLastBusBytes = currentBytes;

    return (float)bytesPerSecond / (float)BUS_LOAD_MAX_BYTES_PER_SECOND;
}

void BusPowerSupplyModule::loop()
{
#ifdef OPENKNX_DEBUG
    if (delayCheck(_debugTimer, 1000)) {
        float pwr1Voltage = (float)analogRead(OPENKNX_BPS_PWR1_CHECK_PIN) / (float)4095 * (float)3.3 * (float)OPENKNX_BPS_PWR_CHECK_FACTOR;
        float pwr2Voltage = (float)analogRead(OPENKNX_BPS_PWR2_CHECK_PIN) / (float)4095 * (float)3.3 * (float)OPENKNX_BPS_PWR_CHECK_FACTOR;
        logDebugP("PWR1 Voltage: %.2f V, PWR2 Voltage: %.2f V", pwr1Voltage, pwr2Voltage);

        float busCurrent = _inaKnx.getCurrent_mA();
        float busVoltage = _inaKnx.getBusVoltage();
        logDebugP("KNX Power: %.2f mA at %.2f V", busCurrent, busVoltage);

        float auxCurrent = _inaAux.getCurrent_mA();
        float auxVoltage = _inaAux.getBusVoltage();
        float auxPower = _inaAux.getPower_mW();
        int test = _inaAux.getRegister(0x02);
        logDebugP("AUX Power: %.2f mA at %.2f V, auxPower: %.2f, REG 0x02: %u", auxCurrent, auxVoltage, auxPower, test);

        _debugTimer = delayTimerInit();
    }
#endif

    if (ParamBPS_BusVoltageChangeSend)
    {
        float busVoltage = _inaKnx.getBusVoltage_mV();
        if (abs(_lastBusVoltageSent - busVoltage) > VOLTAGE_MIN_DIFFERENCE ||
            ParamBPS_BusVoltageCyclicTimeMS > 0 && delayCheck(_busVoltageSentTimer, ParamBPS_BusVoltageCyclicTimeMS))
        {
            KoBPS_BusVoltage.value(busVoltage, DPT_Value_Volt);
            _lastBusVoltageSent = busVoltage;
            _busVoltageSentTimer = delayTimerInit();
        }
    }

    if (ParamBPS_BusCurrentChangeSend)
    {
        float busCurrent = _inaKnx.getCurrent_mA();
        if (abs(_lastBusCurrentSent - busCurrent) > CURRENT_MIN_DIFFERENCE ||
            ParamBPS_BusCurrentCyclicTimeMS > 0 && delayCheck(_busCurrentSentTimer, ParamBPS_BusCurrentCyclicTimeMS))
        {
            KoBPS_BusCurrent.value(busCurrent, DPT_Value_Volt);
            _lastBusCurrentSent = busCurrent;
            _busCurrentSentTimer = delayTimerInit();
        }
    }

    if (ParamBPS_BusLoadChangeSend)
    {
        if (delayCheck(_busLoadSentTimer, BUS_LOAD_UPDATE_RATE))
        {
            float busLoad = estimateBusLoad();
            if (abs(_lastBusLoadSent - busLoad) > LOAD_MIN_DIFFERENCE ||
                ParamBPS_BusLoadCyclicTimeMS > 0 && delayCheck(_busLoadSentTimer, ParamBPS_BusLoadCyclicTimeMS))
            {
                KoBPS_BusLoad.value(busLoad, DPT_Scaling);
                _lastBusLoadSent = busLoad;
                _busLoadSentTimer = delayTimerInit();
            }
        }
    }

    if (ParamBPS_AuxVoltageChangeSend)
    {
        float auxVoltage = _inaKnx.getBusVoltage_mV();
        if (abs(_lastAuxVoltageSent - auxVoltage) > VOLTAGE_MIN_DIFFERENCE ||
            ParamBPS_AuxVoltageCyclicTimeMS > 0 && delayCheck(_auxVoltageSentTimer, ParamBPS_AuxVoltageCyclicTimeMS))
        {
            KoBPS_AuxVoltage.value(auxVoltage, DPT_Value_Volt);
            _lastAuxVoltageSent = auxVoltage;
            _auxVoltageSentTimer = delayTimerInit();
        }
    }

    if (ParamBPS_AuxCurrentChangeSend)
    {
        float auxCurrent = _inaKnx.getCurrent_mA();
        if (abs(_lastAuxCurrentSent - auxCurrent) > CURRENT_MIN_DIFFERENCE ||
            ParamBPS_AuxCurrentCyclicTimeMS > 0 && delayCheck(_auxCurrentSentTimer, ParamBPS_AuxCurrentCyclicTimeMS))
        {
            KoBPS_AuxCurrent.value(auxCurrent, DPT_Value_Volt);
            _lastAuxCurrentSent = auxCurrent;
            _auxCurrentSentTimer = delayTimerInit();
        }
    }
    
    if (ParamBPS_TemperatureChangeSend)
    {
        float temperature = _temperature.readTemperatureC();
        if (abs(_lastTemperatureSent - temperature) > TEMPERATURE_MIN_DIFFERENCE ||
            ParamBPS_TemperatureCyclicTimeMS > 0 && delayCheck(_temperaturSentTimer, ParamBPS_TemperatureCyclicTimeMS))
        {
            KoBPS_Temperature.value(temperature, DPT_Value_Temp);
            _lastTemperatureSent = temperature;
            _temperaturSentTimer = delayTimerInit();
        }
    }
}

void BusPowerSupplyModule::readFlash(const uint8_t *data, const uint16_t size)
{
    if (size == 0)
        return;

    // logDebugP("Reading state from flash");
    // logIndentUp();

    // uint8_t version = openknx.flash.readByte();
    // if (version != OPENKNX_BPS_FLASH_VERSION)
    // {
    //     logDebugP("Invalid flash version %u", version);
    //     return;
    // }

    // uint32_t magicWord = openknx.flash.readInt();
    // if (magicWord != OPENKNX_BPS_FLASH_MAGIC_WORD)
    // {
    //     logDebugP("Flash content invalid");
    //     return;
    // }

    
    // logIndentDown();
}

void BusPowerSupplyModule::writeFlash()
{
    // openknx.flash.writeByte(OPENKNX_BPS_FLASH_VERSION);
    // openknx.flash.writeInt(OPENKNX_BPS_FLASH_MAGIC_WORD);
}

uint16_t BusPowerSupplyModule::flashSize()
{
    return 0;
}

void BusPowerSupplyModule::savePower()
{
    
}

bool BusPowerSupplyModule::restorePower()
{
    bool success = true;
    
    return success;
}

void BusPowerSupplyModule::showHelp()
{
    //logInfo("sa run test mode", "Test all channels one after the other.");
}

bool BusPowerSupplyModule::processCommand(const std::string cmd, bool diagnoseKo)
{
    if (cmd.substr(0, 2) != "bs")
        return false;


    // Commands starting with ba are our diagnose commands
    logInfoP("ba (BusPowerSupply) command with bad args");
    if (diagnoseKo)
    {
        openknx.console.writeDiagenoseKo("ba: bad args");
    }
    return true;
}

void BusPowerSupplyModule::runTestMode()
{
    // logInfoP("Starting test mode");
    // logIndentUp();


    // logInfoP("Testing finished.");
    // logIndentDown();
}