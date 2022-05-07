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


    uint8_t CModbus::m_uiSlave;
    uint8_t CModbus::m_uiFunctionCode;
    uint8_t CModbus::m_uiFlowControl;
//    CSocket* pxSocket;
    uint16_t CModbus::m_uiLastSystemTime;
    uint16_t CModbus::m_uiMessageLength;
    uint8_t *CModbus::m_puiRxBuffer;
    uint8_t *CModbus::m_puiTxBuffer;
    uint8_t *CModbus::m_puiCoils;
    uint8_t *CModbus::m_puiDiscreteInputs;
    uint16_t *CModbus::m_pui16HoldingRegisters;
    uint16_t *CModbus::m_pui16InputRegisters;
    uint16_t CModbus::m_uiCoilsNumber;
    uint16_t CModbus::m_uiDiscreteInputsNumber;
    uint16_t CModbus::m_uiHoldingRegistersNumber;
    uint16_t CModbus::m_uiInputRegistersNumber;

////-----------------------------------------------------------------------------------------------------
//CModbus::CModbus()
//{
//    //ctor
//}
//
//////-----------------------------------------------------------------------------------------------------
////CModbus::CModbus(CUart* pxDevice,
////                 uint32_t uiBaudRate,
////                 char cParity,
////                 uint8_t uiDataBit,
////                 uint8_t uiStopBit,
////                 uint8_t *puiRxBuffer,
////                 uint8_t *puiTxBuffer) :
////    pxDevice(pxDevice),
////    uiBaudRate(uiBaudRate),
////    cParity(cParity),
////    uiDataBit(uiDataBit),
////    uiStopBit(uiStopBit),
////    m_puiRxBuffer(puiRxBuffer),
////    m_puiTxBuffer(puiTxBuffer)
////{
////    m_uiFlowControl = MODBUS_IDDLE;
////}
//
////-----------------------------------------------------------------------------------------------------
//CModbus::~CModbus()
//{
//
//}

//-----------------------------------------------------------------------------------------------------
void CModbus::SlaveSet(uint8_t uiSlave)
{
    m_uiSlave = uiSlave;
}

////-----------------------------------------------------------------------------------------------------
//int8_t CModbus::MessengerIsReady(void)
//{
//    if (m_uiFlowControl == IDDLE)
//    {
//        return 1;
//    }
//    else
//    {
//        return 0;
//    }
//}

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
int16_t CModbus::ReadCoils(uint8_t *puiRequest, uint8_t *puiResponse, uint16_t uiLength)
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
    else if ((uiAddress + uiNumberB) > m_uiCoilsNumber)
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
                                 m_puiCoils,
                                 puiResponse,
                                 uiLength);
    }
    return uiLength;
}

////-----------------------------------------------------------------------------------------------------
//int16_t CModbus::ReadDiscreteInputs(uint8_t *puiRequest, uint8_t *puiResponse, uint16_t uiLength)
//{
//    uint16_t uiOffset = HEADER_LENGTH;
//    int8_t uiSlave = puiRequest[uiOffset - 1];
//    int8_t uiFunctionCode = puiRequest[uiOffset];
//    uint16_t uiAddress = ((static_cast<uint16_t>(puiRequest[uiOffset + 1]) << 8) |
//                          (static_cast<uint16_t>(puiRequest[uiOffset + 2])));
//
//    uint16_t uiNumberB = ((static_cast<uint16_t>(puiRequest[uiOffset + 3]) << 8) |
//                          (static_cast<uint16_t>(puiRequest[uiOffset + 4])));
//
//    if (uiNumberB < 1 || MODBUS_MAX_READ_BITS < uiNumberB)
//    {
//        uiLength = ResponseException(uiSlave,
//                                     uiFunctionCode,
//                                     MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE,
//                                     puiResponse);
//    }
//    else if ((uiAddress + uiNumberB) > m_uiDiscreteInputsNumber)
//    {
//        uiLength = ResponseException(uiSlave,
//                                     uiFunctionCode,
//                                     MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS,
//                                     puiResponse);
//    }
//    else
//    {
//        uiLength = ResponseBasis(uiSlave, uiFunctionCode, puiResponse);
//
//        if (uiNumberB % 8)
//        {
//            puiResponse[uiLength++] = ((uiNumberB / 8) + 1);
//        }
//        else
//        {
//            puiResponse[uiLength++] = (uiNumberB / 8);
//        }
//        uiLength = ByteToBitPack(uiAddress,
//                                 uiNumberB,
//                                 m_puiDiscreteInputs,
//                                 puiResponse,
//                                 uiLength);
//    }
//    return uiLength;
//}
//
////-----------------------------------------------------------------------------------------------------
//int16_t CModbus::ReadHoldingRegisters(uint8_t *puiRequest, uint8_t *puiResponse, uint16_t uiLength)
//{
//    uint16_t uiOffset = HEADER_LENGTH;
//    int8_t uiSlave = puiRequest[uiOffset - 1];
//    int8_t uiFunctionCode = puiRequest[uiOffset];
//    uint16_t uiAddress = ((static_cast<uint16_t>(puiRequest[uiOffset + 1]) << 8) |
//                          (static_cast<uint16_t>(puiRequest[uiOffset + 2])));
//
//    uint16_t uiNumberB = ((static_cast<uint16_t>(puiRequest[uiOffset + 3]) << 8) |
//                          (static_cast<uint16_t>(puiRequest[uiOffset + 4])));
//
//    if (uiNumberB < 1 || MODBUS_MAX_READ_REGISTERS < uiNumberB)
//    {
//        uiLength = ResponseException(uiSlave,
//                                     uiFunctionCode,
//                                     MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE,
//                                     puiResponse);
//    }
//    else if ((uiAddress + uiNumberB) > m_uiHoldingRegistersNumber)
//    {
//        uiLength = ResponseException(uiSlave,
//                                     uiFunctionCode,
//                                     MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS,
//                                     puiResponse);
//    }
//    else
//    {
//        uiLength = ResponseBasis(uiSlave, uiFunctionCode, puiResponse);
//        puiResponse[uiLength++] = (uiNumberB << 1);
//        for (uint16_t i = 0; i < uiNumberB; i++)
//        {
//            puiResponse[uiLength++] = (static_cast<uint16_t>(m_pui16HoldingRegisters[uiAddress]) >> 8);
//            puiResponse[uiLength++] = (static_cast<uint16_t>(m_pui16HoldingRegisters[uiAddress++]) & 0x00FF);
//        }
//    }
//    return uiLength;
//}
//
////-----------------------------------------------------------------------------------------------------
//int16_t CModbus::ReadInputRegisters(uint8_t *puiRequest, uint8_t *puiResponse, uint16_t uiLength)
//{
//    uint16_t uiOffset = HEADER_LENGTH;
//    int8_t uiSlave = puiRequest[uiOffset - 1];
//    int8_t uiFunctionCode = puiRequest[uiOffset];
//    uint16_t uiAddress = ((static_cast<uint16_t>(puiRequest[uiOffset + 1]) << 8) |
//                          (static_cast<uint16_t>(puiRequest[uiOffset + 2])));
//
//    uint16_t uiNumberB = ((static_cast<uint16_t>(puiRequest[uiOffset + 3]) << 8) |
//                          (static_cast<uint16_t>(puiRequest[uiOffset + 4])));
//
//    if (uiNumberB < 1 || MODBUS_MAX_READ_REGISTERS < uiNumberB)
//    {
//        uiLength = ResponseException(uiSlave,
//                                     uiFunctionCode,
//                                     MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE,
//                                     puiResponse);
//    }
//    else if ((uiAddress + uiNumberB) > m_uiInputRegistersNumber)
//    {
//        uiLength = ResponseException(uiSlave,
//                                     uiFunctionCode,
//                                     MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS,
//                                     puiResponse);
//    }
//    else
//    {
//        uiLength = ResponseBasis(uiSlave, uiFunctionCode, puiResponse);
//        puiResponse[uiLength++] = (uiNumberB << 1);
//        for (uint16_t i = 0; i < uiNumberB; i++)
//        {
//            puiResponse[uiLength++] = (static_cast<uint16_t>(m_pui16InputRegisters[uiAddress]) >> 8);
//            puiResponse[uiLength++] = (static_cast<uint16_t>(m_pui16InputRegisters[uiAddress++]) & 0x00FF);
//        }
//    }
//    return uiLength;
//}

////-----------------------------------------------------------------------------------------------------
//int16_t CModbus::WriteSingleCoil(uint8_t *puiRequest, uint8_t *puiResponse, uint16_t uiLength)
//{
//    uint16_t uiOffset = HEADER_LENGTH;
//    int8_t uiSlave = puiRequest[uiOffset - 1];
//    int8_t uiFunctionCode = puiRequest[uiOffset];
//    uint16_t uiAddress = ((static_cast<uint16_t>(puiRequest[uiOffset + 1]) << 8) |
//                          (static_cast<uint16_t>(puiRequest[uiOffset + 2])));
//
//    if (uiAddress >= m_uiCoilsNumber)
//    {
//        uiLength = ResponseException(uiSlave,
//                                     uiFunctionCode,
//                                     MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS,
//                                     puiResponse);
//    }
//    else
//    {
//        uint16_t uiData = ((static_cast<uint16_t>(puiRequest[uiOffset + 3]) << 8) |
//                           (static_cast<uint16_t>(puiRequest[uiOffset + 4])));
//
//        if (uiData == 0xFF00 || uiData == 0x0)
//        {
//            if (uiData)
//            {
//                m_puiCoils[uiAddress] = ON;
//            }
//            else
//            {
//                m_puiCoils[uiAddress] = OFF;
//            }
//            memcpy(puiResponse, puiRequest, uiLength);
//        }
//        else
//        {
//            uiLength = ResponseException(uiSlave,
//                                         uiFunctionCode,
//                                         MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE,
//                                         puiResponse);
//        }
//    }
//    return uiLength;
//}
//
////-----------------------------------------------------------------------------------------------------
//int16_t CModbus::WriteSingleRegister(uint8_t *puiRequest, uint8_t *puiResponse, uint16_t uiLength)
//{
//    uint16_t uiOffset = HEADER_LENGTH;
//    int8_t uiSlave = puiRequest[uiOffset - 1];
//    int8_t uiFunctionCode = puiRequest[uiOffset];
//    uint16_t uiAddress = ((static_cast<uint16_t>(puiRequest[uiOffset + 1]) << 8) |
//                          (static_cast<uint16_t>(puiRequest[uiOffset + 2])));
//
//    if (uiAddress >= m_uiHoldingRegistersNumber)
//    {
//        uiLength = ResponseException(uiSlave,
//                                     uiFunctionCode,
//                                     MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS,
//                                     puiResponse);
//    }
//    else
//    {
//        m_pui16HoldingRegisters[uiAddress] =
//            ((static_cast<uint16_t>(puiRequest[uiOffset + 3]) << 8) |
//             (static_cast<uint16_t>(puiRequest[uiOffset + 4])));
//        memcpy(puiResponse, puiRequest, uiLength);
//    }
//    return uiLength;
//}
//
////-----------------------------------------------------------------------------------------------------
//int16_t CModbus::WriteMultipleCoils(uint8_t *puiRequest, uint8_t *puiResponse, uint16_t uiLength)
//{
//    uint16_t uiOffset = HEADER_LENGTH;
//    int8_t uiSlave = puiRequest[uiOffset - 1];
//    int8_t uiFunctionCode = puiRequest[uiOffset];
//    uint16_t uiAddress = ((static_cast<uint16_t>(puiRequest[uiOffset + 1]) << 8) |
//                          (static_cast<uint16_t>(puiRequest[uiOffset + 2])));
//
//    uint16_t uiNumberB = ((static_cast<uint16_t>(puiRequest[uiOffset + 3]) << 8) |
//                          (static_cast<uint16_t>(puiRequest[uiOffset + 4])));
//
//    if (uiNumberB < 1 || MODBUS_MAX_WRITE_BITS < uiNumberB)
//    {
//        uiLength = ResponseException(uiSlave,
//                                     uiFunctionCode,
//                                     MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE,
//                                     puiResponse);
//    }
//    else if ((uiAddress + uiNumberB) > m_uiCoilsNumber)
//    {
//        uiLength = ResponseException(uiSlave,
//                                     uiFunctionCode,
//                                     MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS,
//                                     puiResponse);
//    }
//    else
//    {
//
//        SetBytesFromBits(m_puiCoils, uiAddress, uiNumberB, &puiRequest[uiOffset + 6]);
//        uiLength = ResponseBasis(uiSlave, uiFunctionCode, puiResponse);
//        /* 4 to copy the bit address (2) and the quantity of bits */
//        memcpy(puiResponse + uiLength, puiRequest + uiLength, 4);
//        uiLength += 4;
//    }
//    return uiLength;
//}
//
////-----------------------------------------------------------------------------------------------------
//int16_t CModbus::WriteMultipleRegisters(uint8_t *puiRequest, uint8_t *puiResponse, uint16_t uiLength)
//{
//    uint16_t uiOffset = HEADER_LENGTH;
//    int8_t uiSlave = puiRequest[uiOffset - 1];
//    int8_t uiFunctionCode = puiRequest[uiOffset];
//    uint16_t uiAddress = ((static_cast<uint16_t>(puiRequest[uiOffset + 1]) << 8) |
//                          (static_cast<uint16_t>(puiRequest[uiOffset + 2])));
//
//    uint16_t uiNumberB = ((static_cast<uint16_t>(puiRequest[uiOffset + 3]) << 8) |
//                          (static_cast<uint16_t>(puiRequest[uiOffset + 4])));
//
//    if (uiNumberB < 1 || MODBUS_MAX_WRITE_REGISTERS < uiNumberB)
//    {
//        uiLength = ResponseException(uiSlave,
//                                     uiFunctionCode,
//                                     MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE,
//                                     puiResponse);
//    }
//    else if ((uiAddress + uiNumberB) > m_uiHoldingRegistersNumber)
//    {
//        uiLength = ResponseException(uiSlave,
//                                     uiFunctionCode,
//                                     MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS,
//                                     puiResponse);
//    }
//    else
//    {
//        for (uint16_t i = 0; i < uiNumberB; i++)
//        {
//            m_pui16HoldingRegisters[uiAddress++] = ((static_cast<uint16_t>(puiRequest[uiOffset + 6]) << 8) |
//                                                    (static_cast<uint16_t>(puiRequest[uiOffset + 6 + 1])));
//            uiOffset += 2;
//        }
//
//        uiLength = ResponseBasis(uiSlave, uiFunctionCode, puiResponse);
//        /* 4 to copy the bit address (2) and the quantity of bits */
//        memcpy(puiResponse + uiLength, puiRequest + uiLength, 4);
//        uiLength += 4;
//
//    }
//    return uiLength;
//}

////-----------------------------------------------------------------------------------------------------
//int16_t CModbus::ReadExceptionStatus(uint8_t *puiRequest, uint8_t *puiResponse, uint16_t uiLength)
//{
////    errno = ENOPROTOOPT;
//    return -1;
//}
//
////-----------------------------------------------------------------------------------------------------
//int16_t CModbus::WriteAndReadRegisters(uint8_t *puiRequest, uint8_t *puiResponse, uint16_t uiLength)
//{
//}
//
////-----------------------------------------------------------------------------------------------------
//int16_t CModbus::ReportSlaveID(uint8_t *puiRequest, uint8_t *puiResponse, uint16_t uiLength)
//{
//    uint16_t uiOffset = HEADER_LENGTH;
//    int8_t uiSlave = puiRequest[uiOffset - 1];
//    int8_t uiFunctionCode = puiRequest[uiOffset];
//
//    uiLength = ResponseBasis(uiSlave, uiFunctionCode, puiResponse);
//    puiResponse[uiLength++] = 3;			// Число байт данных
//    puiResponse[uiLength++] = 32;			// Количество окон сигнализации
//    puiResponse[uiLength++] = 7;			// Код модификации
//    puiResponse[uiLength++] = 50;			// Код устройства
//
//    return uiLength;
//}

////-----------------------------------------------------------------------------------------------------
//int16_t CModbus::Programming(uint8_t *puiRequest, uint8_t *puiResponse, uint16_t uiLength)
//{
//// Функция 0x46 "программирование". Описание протокола обмена:
//// Buff[0] - адрес Slave.
//// Buff[1] - код функции.
//// Buff[2] - количество байт в запросе команды старший байт.
//// Buff[3] - количество байт в запросе команды младший байт.
//// Buff[4] - код операции(0 - запрос версии ПО, 1 - чтение блокаБД, 2 - запись блокаБД).
//// Buff[5] - номер блока БД.
//
//    // Смещения в принятом буфере.
//    enum
//    {
//        REQUEST_LENGTH = 2,
//        REQUEST_COMMAND = 3,
//        BLOCK_NUMBER = 4,
//        DATA_BEGIN = 5,
//    };
//
//    uint16_t uiOffset = HEADER_LENGTH;
//    int8_t uiSlave = puiRequest[uiOffset - 1];
//    int8_t uiFunctionCode = puiRequest[uiOffset];
//
//    uint16_t uiNumberB = (static_cast<uint16_t>(puiRequest[uiOffset + REQUEST_LENGTH]));
//
//    if (uiNumberB != (uiLength - 6))
//    {
//        uiLength = ResponseException(uiSlave,
//                                     uiFunctionCode,
//                                     MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS,
//                                     puiResponse);
//    }
//    else
//    {
//        uint8_t uiBlockNumber;
//        uiLength = ResponseBasis(uiSlave, uiFunctionCode, puiResponse);
//        // Какой код операции?
//        switch(puiRequest[uiOffset + REQUEST_COMMAND])
//        {
//        case 0x00:
//            // Получение версии ПО.
//            puiResponse[uiLength++] = 0;
//            puiResponse[uiLength++] = 4;
//            puiResponse[uiLength++] = puiRequest[uiOffset + REQUEST_COMMAND];
//            puiResponse[uiLength++] = 1; // 1.7.01
//            puiResponse[uiLength++] = 7;
//            puiResponse[uiLength++] = 0x01;
//            break;
//
//        case 0x01:
//            // Чтение блока БД.
//            uiBlockNumber = puiRequest[uiOffset + BLOCK_NUMBER];
//            // Блок не существует?
//            if (uiBlockNumber > CDataBase::BLOCKS_QUANTITY)
//            {
//                uiLength = ResponseException(uiSlave,
//                                             uiFunctionCode,
//                                             MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS,
//                                             puiResponse);
//            }
//            else
//            {
//                // Прочитаем блок БД.
//                uint16_t uiLengthLocal = CDataBase::Read(&puiResponse[uiOffset + DATA_BEGIN], uiBlockNumber);
//                puiResponse[uiLength++] = 0;
//                puiResponse[uiLength++] = uiLengthLocal;
//                puiResponse[uiLength++] = puiRequest[uiOffset + REQUEST_COMMAND];
//                puiResponse[uiLength++] = uiBlockNumber;
//                uiLength += uiLengthLocal;
//            }
//            break;
//
//
//        case 0x02:
//            // Запись блока БД.
//            uiBlockNumber = puiRequest[uiOffset + BLOCK_NUMBER];
//            // Блок не существует?
//            if (uiBlockNumber > CDataBase::BLOCKS_QUANTITY)
//            {
//                uiLength = ResponseException(uiSlave,
//                                             uiFunctionCode,
//                                             MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS,
//                                             puiResponse);
//            }
//            // Размер блока не соответствует?
//            else if (puiRequest[uiOffset + REQUEST_LENGTH] != CDataBase::m_DStruct[uiBlockNumber].Size)
//            {
//                uiLength = ResponseException(uiSlave,
//                                             uiFunctionCode,
//                                             MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE,
//                                             puiResponse);
//            }
//            else
//            {
//                // Сохраним блок БД.
//                // Успешно?
//                if (CDataBase::Write(&puiRequest[uiOffset + DATA_BEGIN], uiBlockNumber))
//                {
//                    CDataBase::SetStatus(1);
//                }
//                else
//                {
//                    CDataBase::SetStatus(0);
//                }
//            }
//            break;
//
//
//        case 0x03:
//            if(!(CDataBase::GetStatus()))
//            {
//                uiLength = ResponseException(uiSlave,
//                                             uiFunctionCode,
//                                             MODBUS_EXCEPTION_ILLEGAL_FUNCTION,
//                                             puiResponse);
//            }
//            break;
//
//        default:
//            uiLength = ResponseException(uiSlave,
//                                         uiFunctionCode,
//                                         MODBUS_EXCEPTION_ILLEGAL_FUNCTION,
//                                         puiResponse);
//            break;
//        };
//    }
//
//    return uiLength;
//}
//
////-----------------------------------------------------------------------------------------------------
//int16_t CModbus::PollProgramming(uint8_t *puiRequest, uint8_t *puiResponse, uint16_t uiLength)
//
//{
//    uint16_t uiOffset = HEADER_LENGTH;
//    int8_t uiSlave = puiRequest[uiOffset - 1];
//    int8_t uiFunctionCode = puiRequest[uiOffset];
//
//    if(!(CDataBase::GetStatus()))
//    {
//        uiLength = ResponseException(uiSlave,
//                                     uiFunctionCode,
//                                     MODBUS_EXCEPTION_ILLEGAL_FUNCTION,
//                                     puiResponse);
//    }
//    else
//    {
//        uiLength = ResponseBasis(uiSlave, uiFunctionCode, puiResponse);
//    }
//
////    if(Storage::GetStatus() != Storage::IDLE)
////    {
////        return ModBus::SLAVE_DEVICE_BUSY;
////    }
//
//    return uiLength;
//}

//-----------------------------------------------------------------------------------------------------
int16_t CModbus::Reply(uint8_t *puiRequest, uint8_t *puiResponse, uint16_t uiFrameLength)
{
    uint16_t uiOffset = HEADER_LENGTH;
    int8_t uiSlave = puiRequest[uiOffset - 1];
    int8_t uiFunctionCode = puiRequest[uiOffset];
    uint16_t uiLength = uiFrameLength;

    /* Filter on the Modbus unit identifier (slave) in RTU mode */
    if (uiSlave != m_uiSlave && uiSlave != MODBUS_BROADCAST_ADDRESS)
    {
        return 0;
    }

    switch (uiFunctionCode)
    {
    case _FC_READ_COILS:
        uiLength = ReadCoils(puiRequest, puiResponse, uiLength);
        break;

//    case _FC_READ_DISCRETE_INPUTS:
//        uiLength = ReadDiscreteInputs(puiRequest, puiResponse, uiLength);
//        break;
//
//    case _FC_READ_HOLDING_REGISTERS:
//        uiLength = ReadHoldingRegisters(puiRequest, puiResponse, uiLength);
//        break;
//
//    case _FC_READ_INPUT_REGISTERS:
//        uiLength = ReadInputRegisters(puiRequest, puiResponse, uiLength);
//        break;
//
//    case _FC_WRITE_SINGLE_COIL:
//        uiLength = WriteSingleCoil(puiRequest, puiResponse, uiLength);
//        break;
//
//    case _FC_WRITE_SINGLE_REGISTER:
//        uiLength = WriteSingleRegister(puiRequest, puiResponse, uiLength);
//        break;
//
//    case _FC_READ_EXCEPTION_STATUS:
//        uiLength = ReadExceptionStatus(puiRequest, puiResponse, uiLength);
//        break;
//
//    case _FC_WRITE_MULTIPLE_COILS:
//        uiLength = WriteMultipleCoils(puiRequest, puiResponse, uiLength);
//        break;
//
//    case _FC_PROGRAMMING_COMPLETION_REQUEST:
//        break;
//
//    case _FC_WRITE_MULTIPLE_REGISTERS:
//        uiLength = WriteMultipleRegisters(puiRequest, puiResponse, uiLength);
//        break;
//
//    case _FC_REPORT_SLAVE_ID:
//        uiLength = ReportSlaveID(puiRequest, puiResponse, uiLength);
//        break;
//
//    case _FC_WRITE_AND_READ_REGISTERS:
//        uiLength = WriteAndReadRegisters(puiRequest, puiResponse, uiLength);
//        break;
//
//    case _FC_DATA_EXCHANGE:
//        break;
//
//    case _FC_DATA_BASE_READ:
//        break;
//
//    case _FC_DATA_BASE_WRITE:
//        break;
//
//    case _FC_PROGRAMMING:
//        uiLength = Programming(puiRequest, puiResponse, uiLength);
//        break;

    default:
        uiLength = ResponseException(uiSlave,
                                     uiFunctionCode,
                                     MODBUS_EXCEPTION_ILLEGAL_FUNCTION,
                                     puiResponse);
        break;
    }

    m_uiMessageLength = uiLength;
    return uiLength;
//    return SendMessage(puiResponse, uiLength);
}

////-----------------------------------------------------------------------------------------------------
//void CModbus::SetByteFromBits(uint8_t *dest, int16_t index, const uint8_t value)
//{
//    int16_t i;
//
//    for (i=0; i<8; i++)
//    {
//        dest[index+i] = (value & (1 << i)) ? 1 : 0;
//    }
//}
//
////-----------------------------------------------------------------------------------------------------
//void CModbus::SetBytesFromBits(uint8_t *dest, int16_t index, uint16_t nb_bits,
//                               const uint8_t *tab_byte)
//{
//    int16_t i;
//    int16_t shift = 0;
//
//    for (i = index; i < index + nb_bits; i++)
//    {
//        dest[i] = tab_byte[(i - index) / 8] & (1 << shift) ? 1 : 0;
//        /* gcc doesn't like: shift = (++shift) % 8; */
//        shift++;
//        shift %= 8;
//    }
//}
//
////-----------------------------------------------------------------------------------------------------
//uint8_t CModbus::GetByteFromBits(const uint8_t *src, int16_t index,
//                                 uint16_t nb_bits)
//{
//    int16_t i;
//    uint8_t value = 0;
//
//    if (nb_bits > 8)
//    {
//        /* Assert is ignored if NDEBUG is set */
////        assert(nb_bits < 8); //?
//        nb_bits = 8;
//    }
//
//    for (i=0; i < nb_bits; i++)
//    {
//        value |= (src[index+i] << i);
//    }
//
//    return value;
//}
//
////-----------------------------------------------------------------------------------------------------
//float CModbus::GetFloat(const uint16_t *src)
//{
//    float f = 0.0f;
//    uint32_t i;
//
//    i = (((uint32_t)src[1]) << 16) + src[0];
//    memcpy(&f, &i, sizeof(float));
//
//    return f;
//}
//
////-----------------------------------------------------------------------------------------------------
//void CModbus::SetFloat(float f, uint16_t *dest)
//{
//    uint32_t i = 0;
//
//    memcpy(&i, &f, sizeof(uint32_t));
//    dest[0] = (uint16_t)i;
//    dest[1] = (uint16_t)(i >> 16);
//}

//-----------------------------------------------------------------------------------------------------






////-----------------------------------------------------------------------------------------------------
//uint8_t CModbus::CheckConfirmation(uint8_t *puiRequest, uint16_t uiLength)
//{
//    uint16_t uiOffset = HEADER_LENGTH;
//    int8_t uiSlave = puiRequest[uiOffset - 1];
//    int8_t uiFunctionCode = puiRequest[uiOffset];
//
//    if ((m_uiSlave == uiSlave) &&
//            (m_uiFunctionCode == uiFunctionCode))
//    {
//
//        return 1;
//    }
//    else if (uiLength == (uiOffset + 2 + CRC_LENGTH) &&
//             (uiFunctionCode & 0x80))
//    {
//        /* EXCEPTION CODE RECEIVED */
//        int8_t uiExceptionCode =
//            puiRequest[uiOffset + MODBUS_EXCEPTION_CODE_OFFSET];
//        if (uiExceptionCode < MODBUS_EXCEPTION_MAX)
//        {
//            return MODBUS_ENOBASE + uiExceptionCode;
//        }
//        else
//        {
//            return EMBBADEXC;
//        }
//    }
//}
//
////-----------------------------------------------------------------------------------------------------
/////* Reads IO status */
////static int read_io_status(uint8_t uiSlave,
////                          uint8_t uiFunctionCode,
////                          uint16_t uiAddress,
////                          uint16_t uiBitNumber,
////                          uint8_t *puiRequest)
//////                                    (modbus_t *ctx, int function,
//////                          int addr, int nb, unsigned char *dest)
////{
////    uint16_t uiLength;
////
////    int rc;
////    int req_length;
////
////    unsigned char req[_MIN_REQ_LENGTH];
////    unsigned char rsp[MAX_MESSAGE_LENGTH];
////
//////    req_length = ctx->backend->build_request_basis(ctx, function, addr, nb, req);
////
////    uiLength = RequestBasis(uiSlave,
////                            uiFunctionCode,
////                            uiAddress,
////                            uiBitNumber,
////                            puiRequest);
////
//////    rc = send_msg(ctx, req, req_length);
////
////
////    if (SendMessage(puiRequest, uiLength))
////    {
////        int i, temp, bit;
////        int pos = 0;
////        int offset;
////        int offset_end;
////
////        rc = receive_msg(ctx, rsp, MSG_CONFIRMATION);
////        if (rc == -1)
////            return -1;
////
////        rc = check_confirmation(ctx, req, rsp, rc);
////        if (rc == -1)
////            return -1;
////
////        offset = ctx->backend->header_length + 2;
////        offset_end = offset + rc;
////        for (i = offset; i < offset_end; i++)
////        {
////            /* Shift reg hi_byte to temp */
////            temp = rsp[i];
////
////            for (bit = 0x01; (bit & 0xff) && (pos < nb);)
////            {
////                dest[pos++] = (temp & bit) ? TRUE : FALSE;
////                bit = bit << 1;
////            }
////
////        }
////    }
////
////    return rc;
////}
//
////-----------------------------------------------------------------------------------------------------
///* Reads the boolean status of bits and sets the array elements
//   in the destination to TRUE or FALSE (single bits). */
//int8_t CModbus::ReadCoilsRequest(uint16_t uiAddress,
//                                  uint16_t uiBitNumber)
//{
//    if (uiBitNumber > MODBUS_MAX_READ_BITS)
//    {
//        return EMBMDATA;
//    }
//
//    if (MessengerIsReady())
//    {
//        m_uiFunctionCode = _FC_READ_COILS;
//        m_uiMessageLength = RequestBasis(m_uiSlave,
//                                         m_uiFunctionCode,
//                                         uiAddress,
//                                         uiBitNumber,
//                                         m_puiTxBuffer);
//        FlowControlSet(FRAME_TRANSMIT_REQUEST);
//        return 1;
//    }
//    else
//    {
//        return 0;
//    }
//}
//
////-----------------------------------------------------------------------------------------------------
//int16_t CModbus::ReadCoilsReply(uint8_t *puiDestination)
//{
//
//    if (MessengerIsReady())
//    {
//
//        FlowControlSet(FRAME_TRANSMIT_REQUEST);
//        return 1;
//    }
//    else
//    {
//        return 0;
//    }
//}
