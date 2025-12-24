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
    openknx.gpio.pinMode(OPENKNX_BPS_PWR1_ALERT_PIN, INPUT);

    openknx.gpio.pinMode(OPENKNX_BPS_PWR2_CHECK_PIN, INPUT);
    openknx.gpio.pinMode(OPENKNX_BPS_PWR2_SWITCH_ON_PIN, OUTPUT, true, !OPENKNX_BPS_PWR_SWITCH_ACTIVE_ON);
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

    if (_inaKnx.begin(OPENKNX_BPS_CURRENT_KNX_INA_ADDR, &OPENKNX_GPIO_WIRE))
    {
        logDebugP("KNX INA238 setup done with address %u", OPENKNX_BPS_CURRENT_KNX_INA_ADDR);
        logIndentUp();

        _inaKnx.setShunt(OPENKNX_BPS_CURRENT_KNX_INA_SHUNT, 3);
        _inaKnx.setAveragingCount(INA2XX_COUNT_16);
        _inaKnx.setMode(INA2XX_MODE_CONTINUOUS);

#ifdef OPENKNX_DEBUG
        delay(1000);
        logDebugP("getMode %u", _inaKnx.getMode());
        // logDebugP("isCalibrated %u", _inaKnx.isCalibrated());
        // logDebugP("getCurrentLSB %.4f", _inaKnx.getCurrentLSB());
        // logDebugP("getShunt %.4f", _inaKnx.getShunt());
        // logDebugP("getMaxCurrent %.4f", _inaKnx.getMaxCurrent());
#endif
        logIndentDown();
    }
    else
        logDebugP("KNX INA238 not found at address %u", OPENKNX_BPS_CURRENT_KNX_INA_ADDR);

    if (_inaAux.begin(OPENKNX_BPS_CURRENT_AUX_INA_ADDR, &OPENKNX_GPIO_WIRE))
    {
        logDebugP("AUX INA238 setup done with address %u", OPENKNX_BPS_CURRENT_AUX_INA_ADDR);
        logIndentUp();

        _inaAux.setShunt(OPENKNX_BPS_CURRENT_AUX_INA_SHUNT, 3);
        _inaKnx.setAveragingCount(INA2XX_COUNT_16);
        _inaAux.setMode(INA2XX_MODE_CONTINUOUS);

#ifdef OPENKNX_DEBUG
        delay(1000);
        logDebugP("getMode %u", _inaAux.getMode());
        // logDebugP("isCalibrated %u", _inaAux.isCalibrated());
        // logDebugP("getCurrentLSB %.4f", _inaAux.getCurrentLSB());
        // logDebugP("getShunt %.4f", _inaAux.getShunt());
        // logDebugP("getMaxCurrent %.4f", _inaAux.getMaxCurrent());
#endif
        logIndentDown();
    }
    else
        logDebugP("AUX INA226 not found at address %u", OPENKNX_BPS_CURRENT_AUX_INA_ADDR);
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
    float pwr1Voltage = (float)analogRead(OPENKNX_BPS_PWR1_CHECK_PIN) / (float)4095 * (float)3.3 * (float)OPENKNX_BPS_PWR_CHECK_FACTOR;
    bool pwr1Ok = pwr1Voltage > POWER_OK_THRESHOLD_VOLTAGE;
    if (_pwr1Ok != pwr1Ok)
    {
        _pwr1Ok = pwr1Ok;
        openknx.gpio.digitalWrite(OPENKNX_BPS_STATUS_PW1, _pwr1Ok ? OPENKNX_BPS_STATUS_ACTIVE_ON : !OPENKNX_BPS_STATUS_ACTIVE_ON);

        if (ParamBPS_PowerSupply1ChangeSend)
            KoBPS_PowerSupply1Status.value(pwr1Ok, DPT_Switch);
    }

    float pwr2Voltage = (float)analogRead(OPENKNX_BPS_PWR2_CHECK_PIN) / (float)4095 * (float)3.3 * (float)OPENKNX_BPS_PWR_CHECK_FACTOR;
    bool pwr2Ok = pwr2Voltage > POWER_OK_THRESHOLD_VOLTAGE;
    if (_pwr2Ok != pwr2Ok)
    {
        _pwr2Ok = pwr2Ok;
        openknx.gpio.digitalWrite(OPENKNX_BPS_STATUS_PW2, _pwr2Ok ? OPENKNX_BPS_STATUS_ACTIVE_ON : !OPENKNX_BPS_STATUS_ACTIVE_ON);

        if (ParamBPS_PowerSupply2ChangeSend)
            KoBPS_PowerSupply2Status.value(pwr2Ok, DPT_Switch);
    }

    if (_reestActive && delayCheck(_resetStarted, ParamBPS_ResetTime * 1000))
    {
        _reestActive = false;
        _resetStarted = 0;
        openknx.gpio.digitalWrite(OPENKNX_BPS_STATUS_RST, !OPENKNX_BPS_STATUS_ACTIVE_ON);

        logInfoP("Bus reset finished");
    }

    if (!_reestActive)
    {
        if (_recentPwrSupplySwitches > MAX_CURRENT_SWITCH_PER_SECOND &&
            (_pwrActive == 1 && !_pwr1Ok || _pwrActive == 2 && !_pwr2Ok))
        {
            pwr1Off();
            pwr2Off();
            _pwrActive = 0;
            _overcurrent = true;
            _overcurrentStarted = delayTimerInit();

            logErrorP("Too many power supply switches, all power off");
        }
        else if (_pwrActive == 1 && !_pwr1Ok)
        {
            if (_pwr2Ok)
            {
                pwr1Off();
                pwr2On();
                _pwrActive = 2;
                _pwrErrorLogged = false;

                _recentPwrSupplySwitches++;
                _lastPwrSupplySwitch = delayTimerInit();

                logInfoP("PWR1 failed, switched to PWR2");
            }
            else
            {
                if (!_pwrErrorLogged)
                {
                    _pwrErrorLogged = true;
                    logErrorP("PWR1 failed, PWR2 is NOT available, too!");

                    openknx.common.triggerSavePin();
                    logInfoP("SAVE triggered.");
                }
            }
        }
        else if (_pwrActive == 2 && !_pwr2Ok)
        {
            if (_pwr1Ok)
            {
                pwr2Off();
                pwr1On();
                _pwrActive = 1;
                _pwrErrorLogged = false;

                _recentPwrSupplySwitches++;
                _lastPwrSupplySwitch = delayTimerInit();

                logInfoP("PWR2 failed, switched to PWR1");
            }
            else
            {
                if (!_pwrErrorLogged)
                {
                    _pwrErrorLogged = true;
                    logErrorP("PWR2 failed, PWR1 is NOT available, too!");

                    openknx.common.triggerSavePin();
                    logInfoP("SAVE triggered.");
                }
            }
        }
        else if (_pwrActive == 0)
        {
            if (!_overcurrent || delayCheck(_overcurrentStarted, CURRENT_OVERLOAD_TIMEOUT_MS))
            {
                if (_pwr1Ok)
                {
                    pwr2Off();
                    pwr1On();
                    _pwrActive = 1;
                    _pwrErrorLogged = false;
                    logInfoP("Power supply started, PWR1 available, switching to PWR1");
                }
                else if (_pwr2Ok)
                {
                    pwr1Off();
                    pwr2On();
                    _pwrActive = 2;
                    _pwrErrorLogged = false;
                    logInfoP("Power supply started, PWR2 available, switching to PWR2");
                }
                else
                {
                    if (!_pwrErrorLogged)
                    {
                        _pwrErrorLogged = true;
                        logErrorP("Power supply started, no power supply available!");
                    }
                }
            }
        }
    }

    float busVoltage = _inaKnx.getBusVoltage_V();
    float busCurrent = _inaKnx.getCurrent_mA() / 1000;
    float auxVoltage = _inaAux.getBusVoltage_V();
    float auxCurrent = _inaAux.getCurrent_mA() / 1000;
    float busLoad = estimateBusLoad();
    float temperature = _temperature.readTemperatureC();

#ifdef OPENKNX_DEBUG
    if (delayCheck(_debugTimer, 1000))
    {
        logDebugP("PWR1 Voltage: %.2f V (%s), PWR2 Voltage: %.2f V (%s), active: %d", pwr1Voltage, _pwr1Ok ? "OK" : "NOT OK", pwr2Voltage, _pwr2Ok ? "OK" : "NOT OK", _pwrActive);
        logDebugP("KNX Power: %.2f mA at %.2f V", busCurrent * 1000, busVoltage);
        logDebugP("AUX Power: %.2f mA at %.2f V", auxCurrent * 1000, auxVoltage);
        logDebugP("Bus Load: %.2f %%, Temperature: %.2f °C", busLoad * 100, temperature);
        
        _debugTimer = delayTimerInit();
    }
#endif

    bool busOk = busVoltage > POWER_OK_THRESHOLD_VOLTAGE;
    if (_busOk != busOk)
    {
        _busOk = busOk;
        openknx.gpio.digitalWrite(OPENKNX_BPS_STATUS_BUS, _busOk ? !OPENKNX_BPS_STATUS_ACTIVE_ON : OPENKNX_BPS_STATUS_ACTIVE_ON);
    }

    float totalCurrentMa = (busCurrent + auxCurrent) * 1000;
    bool currentOk = _reestActive || totalCurrentMa < CURRENT_MAX_THRESHOLD_MA;
    if (_currentOk != currentOk)
    {
        _currentOk = currentOk;
        openknx.gpio.digitalWrite(OPENKNX_BPS_STATUS_MAX, _currentOk ? !OPENKNX_BPS_STATUS_ACTIVE_ON : OPENKNX_BPS_STATUS_ACTIVE_ON);
    }

    if (!_reestActive && totalCurrentMa > CURRENT_OVERLOAD_THRESHOLD_MA)
    {
        pwr1Off();
        pwr2Off();
        _pwrActive = 0;
        _overcurrent = true;
        _overcurrentStarted = delayTimerInit();

        logErrorP("Bus current overload detected: %.2f mA, all power off", totalCurrentMa);
    }

    if (_lastPwrSupplySwitch > 0 && delayCheck(_lastPwrSupplySwitch, POWER_SUPPLY_SWITCH_RECENT_RESET_MS))
    {
        _lastPwrSupplySwitch = 0;
        _recentPwrSupplySwitches = 0;
    }

    if (ParamBPS_PowerSupply1SendCyclicTimeMS > 0 && delayCheck(_powerSupply1SendTimer, ParamBPS_PowerSupply1SendCyclicTimeMS))
    {
        KoBPS_PowerSupply1Status.value(_pwr1Ok, DPT_Switch);
        _powerSupply1SendTimer = delayTimerInit();
    }

    if (ParamBPS_PowerSupply2SendCyclicTimeMS > 0 && delayCheck(_powerSupply2SendTimer, ParamBPS_PowerSupply2SendCyclicTimeMS))
    {
        KoBPS_PowerSupply2Status.value(_pwr2Ok, DPT_Switch);
        _powerSupply2SendTimer = delayTimerInit();
    }

    bool resetPressed = openknx.gpio.digitalRead(OPENKNX_BPS_SWITCH_RST) == OPENKNX_BPS_SWITCH_ACTIVE_ON;
    if (resetPressed && _resetStarted == 0)
    {
        _reestActive = true;
        _resetStarted = delayTimerInit();
        openknx.gpio.digitalWrite(OPENKNX_BPS_STATUS_RST, OPENKNX_BPS_STATUS_ACTIVE_ON);
        logInfoP("Bus reset started, all power off for %d sec.", ParamBPS_ResetTime);

        pwr1Off();
        pwr2Off();
        _pwrActive = 0;
    }

    processSendValue(KoBPS_BusVoltage, DPT_Value_Electric_Potential, ParamBPS_BusVoltageChangeSend, ParamBPS_BusVoltageSendMinChangePercent, ParamBPS_BusVoltageSendMinChangeAbsolute, ParamBPS_BusVoltageSendCyclicTimeMS, _busVoltageSendTimer, _lastBusVoltageSent, busVoltage);
    processSendValue(KoBPS_BusCurrent, DPT_Value_Electric_Current, ParamBPS_BusCurrentChangeSend, ParamBPS_BusCurrentSendMinChangePercent, ParamBPS_BusCurrentSendMinChangeAbsolute, ParamBPS_BusCurrentSendCyclicTimeMS, _busCurrentSendTimer, _lastBusCurrentSent, busCurrent, 1000);
    processSendValue(KoBPS_BusLoad, DPT_Scaling, ParamBPS_BusLoadChangeSend, ParamBPS_BusLoadSendMinChangePercent, ParamBPS_BusLoadSendMinChangeAbsolute, ParamBPS_BusLoadSendCyclicTimeMS, _busLoadSendTimer, _lastBusLoadSent, busLoad);

    processSendValue(KoBPS_AuxVoltage, DPT_Value_Electric_Potential, ParamBPS_AuxVoltageChangeSend, ParamBPS_AuxVoltageSendMinChangePercent, ParamBPS_AuxVoltageSendMinChangeAbsolute, ParamBPS_AuxVoltageSendCyclicTimeMS, _auxVoltageSendTimer, _lastAuxVoltageSent, auxVoltage);
    processSendValue(KoBPS_AuxCurrent, DPT_Value_Electric_Current, ParamBPS_AuxCurrentChangeSend, ParamBPS_AuxCurrentSendMinChangePercent, ParamBPS_AuxCurrentSendMinChangeAbsolute, ParamBPS_AuxCurrentSendCyclicTimeMS, _auxCurrentSendTimer, _lastAuxCurrentSent, auxCurrent, 1000);
    
    processSendValue(KoBPS_Temperature, DPT_Value_Temp, ParamBPS_TemperatureChangeSend, ParamBPS_TemperatureSendMinChangePercent, ParamBPS_TemperatureSendMinChangeAbsolute, ParamBPS_TemperatureSendCyclicTimeMS, _temperatureSendTimer, _lastTemperatureSent, temperature);
}

void BusPowerSupplyModule::processSendValue(GroupObject& ko, Dpt dpt, bool send, uint8_t sendMinChangePercent, uint16_t sendMinChangeAbsolute, uint32_t sendCyclicTimeMS, uint32_t& cyclicSendTimer, float& lastSentValue, float currentValue, uint16_t checkMultiply)
{
    if (!send)
        return;

    uint16_t currentDifference = round(abs(lastSentValue - currentValue * checkMultiply));
    if (currentDifference > 0)
    {
        if (lastSentValue > 0 && currentDifference >= lastSentValue * sendMinChangePercent / checkMultiply &&
            currentDifference >= sendMinChangeAbsolute)
        {
            ko.value(currentValue, dpt);
            lastSentValue = currentValue * checkMultiply;
        }
        else
            ko.valueNoSend(currentValue, dpt);
    }

    if (sendCyclicTimeMS > 0 && delayCheckMillis(cyclicSendTimer, sendCyclicTimeMS))
    {
        ko.value(currentValue, dpt);
        lastSentValue = currentValue * checkMultiply;
        cyclicSendTimer = delayTimerInit();
    }
}

void BusPowerSupplyModule::pwr1On()
{
    logDebugP("Turn PWR1 on");
    openknx.gpio.digitalWrite(OPENKNX_BPS_PWR1_SWITCH_ON_PIN, OPENKNX_BPS_PWR_SWITCH_ACTIVE_ON);
}

void BusPowerSupplyModule::pwr1Off()
{
    logDebugP("Turn PWR1 off");
    openknx.gpio.digitalWrite(OPENKNX_BPS_PWR1_SWITCH_ON_PIN, !OPENKNX_BPS_PWR_SWITCH_ACTIVE_ON);
}

void BusPowerSupplyModule::pwr2On()
{
    logDebugP("Turn PWR2 on");
    openknx.gpio.digitalWrite(OPENKNX_BPS_PWR2_SWITCH_ON_PIN, OPENKNX_BPS_PWR_SWITCH_ACTIVE_ON);
}

void BusPowerSupplyModule::pwr2Off()
{
    logDebugP("Turn PWR2 off");
    openknx.gpio.digitalWrite(OPENKNX_BPS_PWR2_SWITCH_ON_PIN, !OPENKNX_BPS_PWR_SWITCH_ACTIVE_ON);
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

    if (cmd.length() == 10 && cmd.substr(3, 7) == "pwr1 on")
    {
        pwr1On();
        return true;
    }
    else if (cmd.length() == 11 && cmd.substr(3, 8) == "pwr1 off")
    {
        pwr1Off();
        return true;
    }
    else if (cmd.length() == 10 && cmd.substr(3, 7) == "pwr2 on")
    {
        pwr2On();
        return true;
    }
    else if (cmd.length() == 11 && cmd.substr(3, 8) == "pwr2 off")
    {
        pwr2Off();
        return true;
    }

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