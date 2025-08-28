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
    openknx.gpio.pinMode(OPENKNX_BPS_PWR1_SWITCH_PIN, OUTPUT, true, !OPENKNX_BPS_PWR_SWITCH_ACTIVE_ON);
    openknx.gpio.pinMode(OPENKNX_BPS_PWR1_ALERT_PIN, INPUT);

    openknx.gpio.pinMode(OPENKNX_BPS_PWR2_CHECK_PIN, INPUT);
    openknx.gpio.pinMode(OPENKNX_BPS_PWR2_SWITCH_PIN, OUTPUT, true, !OPENKNX_BPS_PWR_SWITCH_ACTIVE_ON);
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
}

void BusPowerSupplyModule::loop()
{
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