//-----------------------------------------------------------------------------------------------------
//  Sourse      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#ifndef CMODBUSRTU_H
#define CMODBUSRTU_H

#include <stdint.h>

#include "Modbus.h"
#include "Configuration.h"

/* Modbus_Application_Protocol_V1_1b.pdf Chapter 4 Section 1 Page 5
 * RS232 / RS485 ADU = 253 bytes + slave (1 byte) + CRC (2 bytes) = 256 bytes
 */
//#define MODBUS_RTU_MAX_ADU_LENGTH  256

#define _MODBUS_RTU_35_TIMEOUT        (((1000000UL / MODBUS_RTU_BAUD_RATE) * 8UL * 4UL) / 1000) + 1

#define _MODBUS_RTU_HEADER_LENGTH      1
#define _MODBUS_RTU_PRESET_REQ_LENGTH  6
#define _MODBUS_RTU_PRESET_RSP_LENGTH  2

#define _MODBUS_RTU_CHECKSUM_LENGTH    2

#define MODBUS_RTU_RS232 0
#define MODBUS_RTU_RS485 1

//enum
//{
//    MODBUS_RTU_MAX_ADU_LENGTH = 256
//};

//-----------------------------------------------------------------------------------------------------
class CModbusRTU : virtual public CModbus
{
public:
//    CModbusRTU();
////    CModbusRTU(CUart* pxDevice,
////               uint32_t uiBaudRate,
////               char cParity,
////               uint8_t uiDataBit,
////               uint8_t uiStopBit,
////               uint8_t *puiRxBuffer,
////               uint8_t *puiTxBuffer);
//    virtual ~CModbusRTU();
    static void Init(CUart* pxDevice,
                     uint32_t uiBaudRate,
                     char cParity,
                     uint8_t uiDataBit,
                     uint8_t uiStopBit,
                     uint8_t *puiRxBuffer,
                     uint8_t *puiTxBuffer,
                     uint8_t *puiCoils,
                     uint8_t *puiDiscreteInputs,
                     uint16_t *pui16HoldingRegisters,
                     uint16_t *pui16InputRegisters,
                     uint16_t uiCoilsNumber,
                     uint16_t uiDiscreteInputsNumber,
                     uint16_t uiInputRegistersNumber,
                     uint16_t uiHoldingRegistersNumber);
    static void Reset(void);
    static int8_t ReceiveEnable(void);
    static int8_t ReceiveDisable(void);
    static int16_t RequestBasis(uint8_t uiSlave,
                                uint8_t uiFunctionCode,
                                uint16_t uiAddress,
                                uint16_t uiBitNumber,
                                uint8_t *puiRequest);
    static int16_t ResponseBasis(uint8_t , uint8_t , uint8_t * );
    static int16_t Tail(uint8_t * , uint16_t );
    static int16_t Send(uint8_t * , uint16_t );
    static uint8_t MessageIsSended(void);
    static int16_t Receive(void);
    static uint8_t Select(void);
    static int16_t Receive(uint8_t * , uint16_t );
    static uint8_t FrameIsReceived(void);
    static uint8_t ReceiveTimeIsOver(void);
    static uint8_t ConfirmationReceiveTimeIsOver(void);
    static uint8_t TransmitDelayTimeIsOver(void);
    static void GetSystemTime(void);
    static uint16_t GetFrameLength(void);
//    static int8_t FrameCheck(void);
    static int8_t FrameCheck(uint8_t * , uint16_t );
    static void Execution(void);
//    void FsmSlave(void);
//    void FsmMaster(void);

    static const uint8_t HEADER_LENGTH = 1;
    static const uint8_t CRC_LENGTH = 2;

protected:
private:
    static CUart* m_pxDevice;
    /* Bauds: 9600, 19200, 57600, 115200, etc */
    static uint32_t m_uiBaudRate;
    /* Data bit */
    static uint8_t m_uiDataBit;
    /* Stop bit */
    static uint8_t m_uiStopBit;
    /* Parity: 'N', 'O', 'E' */
    static char m_cParity;
    // таймоут по отсутствию следующего байта 3.5 бода.
    const static uint8_t m_uiGuardTimeout = _MODBUS_RTU_35_TIMEOUT;
    // таймоут по отсутствию сообщения.
    const static uint8_t m_uiReceiveTimeout = 15000;
    // таймоут по отсутствию подтверждения.
    const static uint8_t m_uiConfirmationTimeout = 500;
    const static uint8_t m_uiTransmitDelayTimeout = 5;

//    friend class CModbusMasterRTU;
};

//-----------------------------------------------------------------------------------------------------




////-----------------------------------------------------------------------------------------------------
//class CModbusMasterRTU : virtual public CModbusRTU
//{
//public:
//    CModbusMasterRTU();
//    virtual ~CModbusMasterRTU();
//    void Execution(void);
//
//protected:
//private:
//
//};

//-----------------------------------------------------------------------------------------------------
#endif // CMODBUSRTU_H
