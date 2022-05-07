//-----------------------------------------------------------------------------------------------------
//  Sourse      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#include "ModbusRTU.h"
#include "Crc.h"


CUart* CModbusRTU::m_pxDevice;
/* Bauds: 9600, 19200, 57600, 115200, etc */
uint32_t CModbusRTU::m_uiBaudRate;
/* Data bit */
uint8_t CModbusRTU::m_uiDataBit;
/* Stop bit */
uint8_t CModbusRTU::m_uiStopBit;
/* Parity: 'N', 'O', 'E' */
char CModbusRTU::m_cParity;

////-----------------------------------------------------------------------------------------------------
//CModbusRTU::CModbusRTU()
//{
//
//}
//
//////-----------------------------------------------------------------------------------------------------
////CModbusRTU::CModbusRTU(CUart* pxDevice,
////                       uint32_t uiBaudRate,
////                       char cParity,
////                       uint8_t uiDataBit,
////                       uint8_t uiStopBit,
////                       uint8_t *puiRxBuffer,
////                       uint8_t *puiTxBuffer) :
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
//
////-----------------------------------------------------------------------------------------------------
//CModbusRTU::~CModbusRTU()
//{
//
//}

//-----------------------------------------------------------------------------------------------------
void CModbusRTU::Init(CUart* pxDevice,
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
                      uint16_t uiHoldingRegistersNumber)
{
    m_pxDevice = pxDevice;
    m_uiBaudRate = uiBaudRate;
    m_cParity = cParity;
    m_uiDataBit = uiDataBit;
    m_uiStopBit = uiStopBit;
    m_puiRxBuffer = puiRxBuffer;
    m_puiTxBuffer = puiTxBuffer;
    m_puiCoils = puiCoils;
    m_puiDiscreteInputs = puiDiscreteInputs;
    m_pui16HoldingRegisters = pui16HoldingRegisters;
    m_pui16InputRegisters = pui16InputRegisters;
    m_uiCoilsNumber = uiCoilsNumber;
    m_uiDiscreteInputsNumber = uiDiscreteInputsNumber;
    m_uiInputRegistersNumber = uiInputRegistersNumber;
    m_uiHoldingRegistersNumber = uiHoldingRegistersNumber;

    CUart::Init(m_uiBaudRate,
                       m_cParity,
                       m_uiDataBit,
                       m_uiStopBit,
                       puiRxBuffer,
                       puiTxBuffer);
}

//-----------------------------------------------------------------------------------------------------
void CModbusRTU::Reset(void)
{
    CUart::Reset();
}

//-----------------------------------------------------------------------------------------------------
int8_t CModbusRTU::ReceiveEnable(void)
{
    CUart::Enable();
}

//-----------------------------------------------------------------------------------------------------
int8_t CModbusRTU::ReceiveDisable(void)
{
    CUart::Disable();
}

//-----------------------------------------------------------------------------------------------------
int16_t CModbusRTU::Tail(uint8_t *puiMessage, uint16_t uiLength)
{
    uint16_t uiCrc = usCrc16(puiMessage, uiLength);
    puiMessage[uiLength++] = uiCrc & 0x00FF;
    puiMessage[uiLength++] = uiCrc >> 8;

    return uiLength;
}

///* Builds a RTU request header */
//int16_t CModbusRTU::RequestBasis(uint8_t uiSlave,
//                                 uint8_t uiFunctionCode,
//                                 uint16_t uiAddress,
//                                 uint16_t uiBitNumber,
//                                 uint8_t *puiRequest)
//{
//    puiRequest[0] = uiSlave;
//    puiRequest[1] = uiFunctionCode;
//    puiRequest[2] = (static_cast<uint8_t>(uiAddress >> 8));
//    puiRequest[3] = (static_cast<uint8_t>(uiAddress & 0x00ff));
//    puiRequest[4] = (static_cast<uint8_t>(uiBitNumber >> 8));
//    puiRequest[5] = (static_cast<uint8_t>(uiBitNumber & 0x00ff));
//
//    return _MODBUS_RTU_PRESET_REQ_LENGTH;
//}

/* Builds a RTU response header */
int16_t CModbusRTU::ResponseBasis(uint8_t uiSlave, uint8_t uiFunctionCode, uint8_t *puiResponse)
{
    /* In this case, the slave is certainly valid because a check is already
     * done in _modbus_rtu_listen */
    puiResponse[0] = uiSlave;
    puiResponse[1] = uiFunctionCode;

    return _MODBUS_RTU_PRESET_RSP_LENGTH;
}

//-----------------------------------------------------------------------------------------------------
int16_t CModbusRTU::Send(uint8_t *puiMessage, uint16_t uiLength)
{
    return CUart::Write(puiMessage, uiLength);
}

//-----------------------------------------------------------------------------------------------------
uint8_t CModbusRTU::MessageIsSended(void)
{
    return CUart::FrameIsSended();
}

////-----------------------------------------------------------------------------------------------------
//int16_t CModbusRTU::Receive(void)
//{
//    return CUart::Read();
//}

//-----------------------------------------------------------------------------------------------------
uint8_t CModbusRTU::Select(void)
{
    return CUart::ByteIsReceived();
}

//-----------------------------------------------------------------------------------------------------
int16_t CModbusRTU::Receive(uint8_t *puiDestination, uint16_t uiLength)
{
    return CUart::Read(puiDestination, uiLength);
}

//-----------------------------------------------------------------------------------------------------
uint8_t CModbusRTU::FrameIsReceived(void)
{
    if ((CPlatform::GetSystemTick() - m_uiLastSystemTime)  >=
            m_uiGuardTimeout)
    {
        return 1;
    }
    else
    {
        return 0;
    }
};

//-----------------------------------------------------------------------------------------------------
uint8_t CModbusRTU::ReceiveTimeIsOver(void)
{
    if ((CPlatform::GetSystemTick() - m_uiLastSystemTime)  >=
            m_uiReceiveTimeout)
    {
        return 1;
    }
    else
    {
        return 0;
    }
};

//-----------------------------------------------------------------------------------------------------
uint8_t CModbusRTU::ConfirmationReceiveTimeIsOver(void)
{
    if ((CPlatform::GetSystemTick() - m_uiLastSystemTime)  >=
            m_uiConfirmationTimeout)
    {
        return 1;
    }
    else
    {
        return 0;
    }
};

//-----------------------------------------------------------------------------------------------------
uint8_t CModbusRTU::TransmitDelayTimeIsOver(void)
{
    if ((CPlatform::GetSystemTick() - m_uiLastSystemTime)  >=
            m_uiTransmitDelayTimeout)
    {
        return 1;
    }
    else
    {
        return 0;
    }
};

//-----------------------------------------------------------------------------------------------------
uint16_t CModbusRTU::GetFrameLength(void)
{
    return CUart::GetFrameLength();
}

//-----------------------------------------------------------------------------------------------------
void CModbusRTU::GetSystemTime(void)
{
    m_uiLastSystemTime = CPlatform::GetSystemTick();
}

////-----------------------------------------------------------------------------------------------------
//int8_t CModbusRTU::FrameCheck(void)
//{
//    if (CUart::m_nuiRxBuffByteCounter < _MIN_MESSAGE_LENGTH)
//    {
//        return 0;
//    }
//
//    uint16_t uiCrc = ((static_cast<uint16_t>(m_puiRxBuffer[CUart::m_nuiRxBuffByteCounter - 1]) << 8) |
//                      (static_cast<uint16_t>(m_puiRxBuffer[CUart::m_nuiRxBuffByteCounter - 2])));
//
//    if (usCrc16(m_puiRxBuffer,
//                (CUart::m_nuiRxBuffByteCounter - _MODBUS_RTU_CHECKSUM_LENGTH)) == uiCrc)
//    {
//        return 1;
//    }
//    else
//    {
//        return 0;
//    }
//}

//-----------------------------------------------------------------------------------------------------
int8_t CModbusRTU::FrameCheck(uint8_t *puiSourse, uint16_t uiLength)
{
    if (uiLength < _MIN_MESSAGE_LENGTH)
    {
        return 0;
    }

    uint16_t uiCrc = ((static_cast<uint16_t>(puiSourse[uiLength - 1]) << 8) |
                      (static_cast<uint16_t>(puiSourse[uiLength - 2])));
    uint16_t uiCrcTemp = usCrc16(puiSourse,
                                 (uiLength - _MODBUS_RTU_CHECKSUM_LENGTH));
    if (usCrc16(puiSourse,
                (uiLength - _MODBUS_RTU_CHECKSUM_LENGTH)) == uiCrc)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------
void CModbusRTU::Execution(void)
{
    int16_t iReceivedCounter;

    switch (m_uiFlowControl)
    {
    case IDDLE:
        break;

    case START_REQUEST:
        GetSystemTime();
        Reset();
        m_uiMessageLength = 0;
        m_uiFlowControl = WAITING_MESSAGE_REQUEST;
        break;

    case WAITING_MESSAGE_REQUEST:
        if (Select())
        {
            iReceivedCounter = Receive(m_puiRxBuffer + m_uiMessageLength, MODBUS_RTU_MAX_ADU_LENGTH - m_uiMessageLength);

            if (iReceivedCounter > 0)
            {
                m_uiMessageLength += iReceivedCounter;
                GetSystemTime();
                m_uiFlowControl = RECEIVE_MESSAGE_REQUEST;
            }
            else if (iReceivedCounter == -1)
            {
                m_uiFlowControl = START_REQUEST;
            }
        }

        if (ReceiveTimeIsOver())
        {
            m_uiFlowControl = START_REQUEST;
        }

        break;

    case RECEIVE_MESSAGE_REQUEST:
        if (Select())
        {
            iReceivedCounter = Receive(m_puiRxBuffer + m_uiMessageLength, MODBUS_RTU_MAX_ADU_LENGTH - m_uiMessageLength);

            if (iReceivedCounter > 0)
            {
                m_uiMessageLength += iReceivedCounter;
                GetSystemTime();
            }
            else if (iReceivedCounter == -1)
            {
                m_uiFlowControl = START_REQUEST;
            }
        }

        if (FrameIsReceived())
        {
            if (FrameCheck(m_puiRxBuffer, m_uiMessageLength))
            {
                m_uiFlowControl = REPLY_REQUEST;
            }
            else
            {
                m_uiFlowControl = START_REQUEST;
            }
        }

        break;

    case REPLY_REQUEST:
        if (Reply(m_puiRxBuffer, m_puiTxBuffer, m_uiMessageLength))
        {
//            CPlatform::TxLedOn();
            m_uiFlowControl = FRAME_TRANSMIT_CONFIRMATION;
        }
        else
        {
//            CPlatform::TxLedOff();
            m_uiFlowControl = START_REQUEST;
        }
        break;

    case FRAME_TRANSMIT_CONFIRMATION:
        Reset();
        GetSystemTime();
        m_uiFlowControl = WAITING_FRAME_TRANSMIT_CONFIRMATION;
        break;

    case WAITING_FRAME_TRANSMIT_CONFIRMATION:
        if (TransmitDelayTimeIsOver())
        {
            SendMessage(m_puiTxBuffer, m_uiMessageLength);
            m_uiFlowControl = END_WAITING_FRAME_TRANSMIT_CONFIRMATION;
        }
        break;

    case END_WAITING_FRAME_TRANSMIT_CONFIRMATION:
        if (MessageIsSended())
        {
//            CPlatform::TxLedOff();
            m_uiFlowControl = START_REQUEST;
        }

        break;

////-----------------------------------------------------------------------------------------------------
//// ModbusMaster
//    case START_CONFIRMATION:
//        GetSystemTime();
//        Reset();
//        m_uiMessageLength = 0;
//        m_uiFlowControl = WAITING_MESSAGE_CONFIRMATION;
//        break;
//
//    case WAITING_MESSAGE_CONFIRMATION:
//        if (Select())
//        {
//            iReceivedCounter = Receive(m_puiRxBuffer + m_uiMessageLength, MODBUS_RTU_MAX_ADU_LENGTH - m_uiMessageLength);
//
//            if (iReceivedCounter > 0)
//            {
//                m_uiMessageLength += iReceivedCounter;
//                GetSystemTime();
//                m_uiFlowControl = RECEIVE_MESSAGE_CONFIRMATION;
//            }
//            else if (iReceivedCounter == -1)
//            {
//                m_uiFlowControl = START_CONFIRMATION;
//            }
//        }
//
//        if (ConfirmationReceiveTimeIsOver())
//        {
//            m_uiFlowControl = START_CONFIRMATION;
//        }
//
//        break;
//
//    case RECEIVE_MESSAGE_CONFIRMATION:
//        if (Select())
//        {
//            iReceivedCounter = Receive(m_puiRxBuffer + m_uiMessageLength, MODBUS_RTU_MAX_ADU_LENGTH - m_uiMessageLength);
//
//            if (iReceivedCounter > 0)
//            {
//                m_uiMessageLength += iReceivedCounter;
//                GetSystemTime();
//            }
//            else if (iReceivedCounter == -1)
//            {
//                m_uiFlowControl = START_CONFIRMATION;
//            }
//        }
//
//        if (FrameIsReceived())
//        {
//            if (FrameCheck(m_puiRxBuffer, m_uiMessageLength))
//            {
//                m_uiFlowControl = REPLY_CONFIRMATION;
//            }
//            else
//            {
//                m_uiFlowControl = START_CONFIRMATION;
//            }
//        }
//
//        break;
//
//    case REPLY_CONFIRMATION:
//        if (Reply(m_puiRxBuffer, m_puiTxBuffer, m_uiMessageLength))
//        {
//            CPlatform::TxLedOn();
//            m_uiFlowControl = FRAME_TRANSMIT_REQUEST;
//        }
//        else
//        {
//            CPlatform::TxLedOff();
//            m_uiFlowControl = START_CONFIRMATION;
//        }
//        break;
//
//    case FRAME_TRANSMIT_REQUEST:
//        Reset();
//        GetSystemTime();
//        m_uiFlowControl = WAITING_FRAME_TRANSMIT_REQUEST;
//        break;
//
//    case WAITING_FRAME_TRANSMIT_REQUEST:
//        if (TransmitDelayTimeIsOver())
//        {
//            SendMessage(m_puiTxBuffer, m_uiMessageLength);
//            m_uiFlowControl = END_WAITING_FRAME_TRANSMIT_REQUEST;
//        }
//        break;
//
//    case END_WAITING_FRAME_TRANSMIT_REQUEST:
//        if (MessageIsSended())
//        {
//            CPlatform::TxLedOff();
//            m_uiFlowControl = START_CONFIRMATION;
//        }
//
//        break;

    default:
        break;
    }
}
//-----------------------------------------------------------------------------------------------------






////-----------------------------------------------------------------------------------------------------
//CModbusMasterRTU::CModbusMasterRTU()
//{
//
//}
//
////-----------------------------------------------------------------------------------------------------
//CModbusMasterRTU::~CModbusMasterRTU()
//{
//
//}
//
////-----------------------------------------------------------------------------------------------------
//void CModbusMasterRTU::Execution(void)
//{
//    int16_t iReceivedCounter;
//
//    switch (m_uiFlowControl)
//    {
//    case IDDLE:
//        break;
//
//    case START:
//        Reset();
//        GetSystemTime();
//        m_uiMessageLength = 0;
//        m_uiFlowControl = FRAME_TRANSMIT;
//        break;
//
//    case WAITING_MESSAGE_INDICATION:
//        if (Select())
//        {
//            iReceivedCounter = Receive(m_puiRxBuffer + m_uiMessageLength, MODBUS_RTU_MAX_ADU_LENGTH - m_uiMessageLength);
//
//            if (iReceivedCounter > 0)
//            {
//                m_uiMessageLength += iReceivedCounter;
//                GetSystemTime();
//                m_uiFlowControl = RECEIVE_MESSAGE;
//            }
//            else if (iReceivedCounter == -1)
//            {
//                m_uiFlowControl = START;
//            }
//        }
//
//        if (ReceiveTimeIsOver())
//        {
//            m_uiFlowControl = START;
//        }
//
//        break;
//
//    case START_MESSAGE_CONFIRMATION:
//        Reset();
//        GetSystemTime();
//        m_uiMessageLength = 0;
//        m_uiFlowControl = WAITING_MESSAGE_CONFIRMATION;
//        break;
//
//    case WAITING_MESSAGE_CONFIRMATION:
//        if (Select())
//        {
//            iReceivedCounter = Receive(m_puiRxBuffer + m_uiMessageLength, MODBUS_RTU_MAX_ADU_LENGTH - m_uiMessageLength);
//
//            if (iReceivedCounter > 0)
//            {
//                m_uiMessageLength += iReceivedCounter;
//                GetSystemTime();
//                m_uiFlowControl = RECEIVE_MESSAGE;
//            }
//            else if (iReceivedCounter == -1)
//            {
//                m_uiFlowControl = START;
//            }
//        }
//
//        if (ConfirmationReceiveTimeIsOver())
//        {
//            m_uiFlowControl = START;
//        }
//
//        break;
//
//    case RECEIVE_MESSAGE:
//        if (Select())
//        {
//            iReceivedCounter = Receive(m_puiRxBuffer + m_uiMessageLength, MODBUS_RTU_MAX_ADU_LENGTH - m_uiMessageLength);
//
//            if (iReceivedCounter > 0)
//            {
//                m_uiMessageLength += iReceivedCounter;
//                GetSystemTime();
//            }
//            else if (iReceivedCounter == -1)
//            {
//                m_uiFlowControl = START;
//            }
//        }
//
//        if (FrameIsReceived())
//        {
//            if (FrameCheck(m_puiRxBuffer, m_uiMessageLength))
//            {
//                m_uiFlowControl = REPLY;
//            }
//            else
//            {
//                m_uiFlowControl = START;
//            }
//        }
//
//        break;
//
//    case REPLY:
//        if (Reply(m_puiRxBuffer, m_puiTxBuffer, m_uiMessageLength))
//        {
//            CPlatform::TxLedOn();
//            m_uiFlowControl = FRAME_TRANSMIT;
//        }
//        else
//        {
//            CPlatform::TxLedOff();
//            m_uiFlowControl = START;
//        }
//        break;
//
//    case FRAME_TRANSMIT:
//        Reset();
//        GetSystemTime();
//        m_uiFlowControl = WAITING_FRAME_TRANSMIT;
//        break;
//
//    case WAITING_FRAME_TRANSMIT:
//        if (TransmitDelayTimeIsOver())
//        {
//            SendMessage(m_puiTxBuffer, m_uiMessageLength);
//            m_uiFlowControl = FRAME_TRANSMIT_END_WAITING;
//        }
//        break;
//
//    case FRAME_TRANSMIT_END_WAITING:
//        if (MessageIsSended())
//        {
//            CPlatform::TxLedOff();
//            m_uiFlowControl = IDDLE;
//        }
//
//        break;
//
//    default:
//        break;
//    }
//}
