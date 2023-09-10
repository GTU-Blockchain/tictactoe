#include <WiFi.h>
#include <Web3.h>
#include <Contract.h>
#include "Arduino.h"

#define USE_SERIAL Serial

#define ENV_SSID "Mi 10T Pro"
#define ENV_WIFI_KEY "123456789."
#define MY_ADDRESS "0x36f54e82359c4f7E7A3AF4e80F69a6ad05205691"
#define CONTRACT_ADDRESS "0xc1054E37b5Ed45b92E122ef1bcC2630a9FB051d6"
#define INFURA_HOST 11155111
#define INFURA_PATH "/560e88014b8940a1b41497db3442776f"
 
const char *PRIVATE_KEY = "c3629e3a309ce2bcafd0235cdf9d0ba88aa2175270792b137036a40e276790e1";
Web3 web3(INFURA_HOST, INFURA_PATH);

void eth_send_example();

void setup()
{
    USE_SERIAL.begin(115200);

    for (int t = 4; t > 0; t--)
    {
        // USE_SERIAL.println("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    WiFi.begin(ENV_SSID, ENV_WIFI_KEY);

    // attempt to connect to Wifi network:
    while (WiFi.status() != WL_CONNECTED)
    {
        // Serial.print(".");
        // wait 1 second for re-trying
        delay(1000);
    }

    // USE_SERIAL.print("Connected");

    eth_send_example();
}

void loop()
{
    // put your main code here, to run repeatedly:
}

void eth_send_example()
{
    Contract contract(&web3, CONTRACT_ADDRESS);
    contract.SetPrivateKey(PRIVATE_KEY);
    uint32_t nonceVal = (uint32_t)web3.EthGetTransactionCount((string*)MY_ADDRESS);

    uint32_t gasPriceVal = 141006540;
    uint32_t gasLimitVal = 3000000;
    string toStr = CONTRACT_ADDRESS;
    uint256_t valueStr = "0x00";
    uint8_t dataStr[100];
    memset(dataStr, 0, 100);
    const char *func = "set(uint256)";
    string p = contract.SetupContractData(func, 123);
    string result = contract.SendTransaction(nonceVal, gasPriceVal, gasLimitVal, &toStr, &valueStr, &p);

    // USE_SERIAL.print(result);
}