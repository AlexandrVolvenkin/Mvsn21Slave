//-----------------------------------------------------------------------------------------------------
//  Sourse      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------

#include "Modbus.h"
#include "ModbusRTU.h"
#include "Platform.h"


uint8_t CModbus::m_uiOwnAddress;
uint8_t CModbus::m_uiSlaveAddress;
uint8_t CModbus::m_uiFunctionCode;
uint16_t CModbus::m_uiQuantity;
uint8_t CModbus::m_uiFlowControl;
uint16_t CModbus::m_uiLastSystemTime;
uint16_t CModbus::m_uiMessageLength;
uint8_t *CModbus::m_puiRxBuffer;
uint8_t *CModbus::m_puiTxBuffer;
uint8_t *CModbus::m_puiDiscreteInputs;
uint16_t CModbus::m_uiDiscreteInputsNumber;

//-----------------------------------------------------------------------------------------------------
void CModbus::SlaveSet(uint8_t uiSlave)
{
    m_uiOwnAddress = uiSlave;
}

//-----------------------------------------------------------------------------------------------------
int16_t CModbus::SendMessage(uint8_t *puiMessage, uint16_t uiLength)
{
    uiLength = CModbusRTU::Tail(puiMessage, uiLength);
    return CModbusRTU::Send(puiMessage, uiLength);
}

//-----------------------------------------------------------------------------------------------------
/* Build the exception response */
int16_t CModbus::ResponseException(uint8_t uiSlave, uint8_t uiFunctionCode, uint8_t uiExceptionCode, uint8_t *puiResponse)
{
    int16_t uiLength;

    uiLength = CModbusRTU::ResponseBasis(uiSlave, (uiFunctionCode | 0x80), puiResponse);
    /* Positive exception code */
    puiResponse[uiLength++] = uiExceptionCode;

    return uiLength;
}

//-----------------------------------------------------------------------------------------------------
uint8_t CModbus::ResponseIsReceived(void)
{
    if (m_uiFlowControl  == IDDLE)
    {
        return 1;
    }
    else
    {
        return 0;
    }
};

//-----------------------------------------------------------------------------------------------------
int16_t CModbus::ByteToBitPack(uint16_t uiAddress,
                               uint16_t uiNumberB,
                               uint8_t *m_puiCoils,
                               uint8_t *puiResponse,
                               uint16_t uiLength)
{
    uint8_t uiData = 0;
    uint8_t uiShift = 0;
    for (uint16_t i = 0; i < uiNumberB; i++)
    {
        if (m_puiCoils[uiAddress++])
        {
            uiData |= (1 << uiShift);
        }
        // Byte is full?
        if (uiShift == 7)
        {
            puiResponse[uiLength++] = uiData;
            uiData = 0;
            uiShift = 0;
        }
        else
        {
            uiShift++;
        }
    }

    if (uiShift != 0)
    {
        puiResponse[uiLength++] = uiData;
    }

    return uiLength;
}

//-----------------------------------------------------------------------------------------------------
int16_t CModbus::ReadDiscreteInputs(uint8_t *puiRequest, uint8_t *puiResponse, uint16_t uiLength)
{
    uint16_t uiOffset = HEADER_LENGTH;
    int8_t uiSlave = puiRequest[uiOffset - 1];
    int8_t uiFunctionCode = puiRequest[uiOffset];
    uint16_t uiAddress = ((static_cast<uint16_t>(puiRequest[uiOffset + 1]) << 8) |
                          (static_cast<uint16_t>(puiRequest[uiOffset + 2])));

    uint16_t uiNumberB = ((static_cast<uint16_t>(puiRequest[uiOffset + 3]) << 8) |
                          (static_cast<uint16_t>(puiRequest[uiOffset + 4])));

    if (uiNumberB < 1 || MODBUS_MAX_READ_BITS < uiNumberB)
    {
        uiLength = ResponseException(uiSlave,
                                     uiFunctionCode,
                                     MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE,
                                     puiResponse);
    }
    else if ((uiAddress + uiNumberB) > m_uiDiscreteInputsNumber)
    {
        uiLength = ResponseException(uiSlave,
                                     uiFunctionCode,
                                     MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS,
                                     puiResponse);
    }
    else
    {
        uiLength = CModbusRTU::ResponseBasis(uiSlave, uiFunctionCode, puiResponse);

        if (uiNumberB % 8)
        {
            puiResponse[uiLength++] = ((uiNumberB / 8) + 1);
        }
        else
        {
            puiResponse[uiLength++] = (uiNumberB / 8);
        }
        uiLength = ByteToBitPack(uiAddress,
                                 uiNumberB,
                                 m_puiDiscreteInputs,
                                 puiResponse,
                                 uiLength);
    }
    return uiLength;
}

//-----------------------------------------------------------------------------------------------------
int16_t CModbus::Reply(uint8_t *puiRequest, uint8_t *puiResponse, uint16_t uiFrameLength)
{
    uint16_t uiOffset = HEADER_LENGTH;
    uint8_t uiSlave = puiRequest[uiOffset - 1];
    uint8_t uiFunctionCode = puiRequest[uiOffset];
    uint16_t uiLength = uiFrameLength;

    /* Filter on the Modbus unit identifier (slave) in RTU mode */
    if (uiSlave != m_uiOwnAddress && uiSlave != MODBUS_BROADCAST_ADDRESS)
    {
        return 0;
    }

    switch (uiFunctionCode)
    {
    case _FC_READ_DISCRETE_INPUTS:
        uiLength = ReadDiscreteInputs(puiRequest, puiResponse, uiLength);
        break;

    default:
        uiLength = ResponseException(uiSlave,
                                     uiFunctionCode,
                                     MODBUS_EXCEPTION_ILLEGAL_FUNCTION,
                                     puiResponse);
        break;
    }

    m_uiMessageLength = uiLength;
    return uiLength;
}

//-----------------------------------------------------------------------------------------------------






