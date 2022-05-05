//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------

#include "Mvsn21.h"
#include "Platform.h"
#include "Crc.h"

uint8_t CMvsn21::m_uiType;
CDriver* CMvsn21::m_pxDriver;
uint8_t CMvsn21::m_uiFlowControl;
uint16_t CMvsn21::m_uiMessageLength;
uint8_t CMvsn21::m_uiChannel;
uint8_t CMvsn21::m_uiMeasureFlowControl;
TChipChannelData CMvsn21::axChipsChannelsData[CHIP_NUMBER];
//CMeasurementChannel CMvsn21::axMasterMeasurementChannels[];
//CMeasurementChannel CMvsn21::axSlave1MeasurementChannels[];
//CMeasurementChannel CMvsn21::axSlave2MeasurementChannels[];
// Сопоставление входа каналу.
__flash TChannelRemap CMvsn21::axMeasurementChannelRemap[] =
{
    // Нумерация входов сверху вниз.
    // разъём X1.
    {0, 6},
    {0, 7},
    {0, 0},
    {0, 1},
    {0, 2},
    {0, 3},
    // разъём X2.
    {0, 4},
    {0, 5},
    {1, 6},
    {1, 7},
    {1, 0},
    {1, 1},
    // разъём X3.
    {1, 2},
    {1, 3},
    {1, 4},
    {1, 5},
    {2, 6},
    {2, 7},
    // разъём X4.
    {2, 0},
    {2, 1},
    {2, 2},
    {2, 3},
    {2, 4},
    {2, 5},
};
uint8_t CMvsn21::auiDiscreteInputBitData[];

//-----------------------------------------------------------------------------------------------------
CMvsn21::CMvsn21(uint8_t uiType, CDriver* pxDriver)// :
//    m_uiType(uiType),
//    m_pxDriver(pxDriver)
{

}

//-----------------------------------------------------------------------------------------------------
CMvsn21::~CMvsn21()
{
    //dtor
}

//-----------------------------------------------------------------------------------------------------
void CMvsn21::SlaveSelectInterruptEnable(void)
{

}

//-----------------------------------------------------------------------------------------------------
void CMvsn21::SlaveSelectInterruptDisable(void)
{

}

//-----------------------------------------------------------------------------------------------------
void CMvsn21::SpiBusExchangeEnable(void)
{
    CPlatform::Int0InterruptEnable();
    m_uiFlowControl = FSM_START;
}

//-----------------------------------------------------------------------------------------------------
void CMvsn21::SpiBusExchangeDisable(void)
{
    CPlatform::Int0InterruptDisable();
}

//-----------------------------------------------------------------------------------------------------
void CMvsn21::Reset(void)
{
    m_uiMessageLength = 0;
    CSpi::Reset();
}

//-----------------------------------------------------------------------------------------------------
uint8_t CMvsn21::Select(void)
{
    return CSpi::ByteIsReceived();
}

//-----------------------------------------------------------------------------------------------------
int16_t CMvsn21::Exchange(void)
{
    return CSpi::Exchange();
}

//-----------------------------------------------------------------------------------------------------------------
uint8_t CMvsn21::FrameIsReceived(void)
{
    return CSpi::m_bfDataExchangeIsOccur;
};

//-----------------------------------------------------------------------------------------------------------------
uint16_t CMvsn21::GetFrameLength(void)
{
    return CSpi::GetFrameLength();
}

//-----------------------------------------------------------------------------------------------------------------
int8_t CMvsn21::FrameCheck(void)
{
//    if (m_pxDevice -> m_nuiRxBuffByteCounter < _MIN_MESSAGE_LENGTH)
//    {
//        return 0;
//    }
//
//    uint16_t uiCrc = ((static_cast<uint16_t>(m_puiRxBuffer[m_pxDevice -> m_nuiRxBuffByteCounter - 2]) << 8) |
//                      (static_cast<uint16_t>(m_puiRxBuffer[m_pxDevice -> m_nuiRxBuffByteCounter - 1])));
//
//    if (usCrc16(m_puiRxBuffer,
//                (m_pxDevice -> m_nuiRxBuffByteCounter - _MVSN21_RTU_CHECKSUM_LENGTH)) == uiCrc)
//    {
//        return 1;
//    }
//    else
//    {
//        return 0;
//    }
}

//-----------------------------------------------------------------------------------------------------------------
int8_t CMvsn21::FrameCheck(uint8_t *puiSource, uint16_t uiLength)
{
//    if (uiLength < _MIN_MESSAGE_LENGTH)
//    {
//        return 0;
//    }
//
//    uint16_t uiCrc = ((static_cast<uint16_t>(puiSource[uiLength - 2]) << 8) |
//                      (static_cast<uint16_t>(puiSource[uiLength - 1])));
//
//    if (usCrc16(puiSource,
//                (uiLength - _MVSN21_RTU_CHECKSUM_LENGTH)) == uiCrc)
//    {
//        return 1;
//    }
//    else
//    {
//        return 0;
//    }
}

//-----------------------------------------------------------------------------------------------------
void CMvsn21::ChannelsToDiscreteInput(void)
{
    uint8_t uiLength = 0;
    uint8_t *puiMessage = auiDiscreteInputBitData;
//    TChannelRemap __flash *pxMeasurementChannelRemap;
//    pxMeasurementChannelRemap = &axMeasurementChannelRemap[0];

    // Протокол обмена данными по шине Spi.
    // Первый байт - пустой.
    puiMessage[uiLength++] = 0;
//    // Второй байт - "эхо".
//    puiMessage[uiLength++] = COMMAND_READ_DATA;
    // Третий байт - команда обмен данными.
    puiMessage[uiLength++] = COMMAND_READ_DATA;


    // Следующие шесть байт - данные состояния дискретных входов.
    // Упакуем двухбитовые данные состояния измерительных каналов всех чипов в дискретные входы.
    // i - индекс дискретных входов.
    for (int8_t i = 0; i < DISCRETE_INPUT_NUMBER; )
    {
        puiMessage[uiLength] = 0;
//        // Упакуем двухбитовые данные состояния восьми измерительных каналов одного чипа в дискретные входы.
        // Упакуем двухбитовые данные состояния четырёх измерительных каналов в байт по четыре дискретных входа.
        for (int8_t j= 0;
                j < (CHANNELS_IN_BYTE * MEASURE_CHANNEL_STATE_BIT_NUMBER);
                j += MEASURE_CHANNEL_STATE_BIT_NUMBER)
        {
            // Упаковали все данные состояния измерительных каналов?
            if (i < DISCRETE_INPUT_NUMBER)
            {
                // Отобразим данные состояния измерительных каналов на дискретные входы во временный буфер.
                puiMessage[uiLength] |=
                    // Следующие два бита в байте.
                    // Выберем канал по индексу чипа, карты сопоставления измерительных каналов со входами модуля.
                    ((axChipsChannelsData[axMeasurementChannelRemap[i].uiChip].
                      // Выберем канал по индексу входа модуля, карты сопоставления измерительных каналов со входами модуля.
                      axMeasurementChannels[axMeasurementChannelRemap[i].uiChannel].
                      // Состояние измерительного канала занимает два младших бита.
                      m_uiState & 0x03) << j);
                // Следующий дискретный вход.
                i++;
            }
            else
            {
                // Упаковали все данные состояния измерительных каналов.
                goto EndStateDataPack;
            }
        }
        // Следующий байт.
        uiLength++;
        puiMessage[uiLength] = 0;
    }

EndStateDataPack:

    // Последний байт - сумма всех байт начиная с третьего(пустой байт и "эхо" не учитываются).
    puiMessage[uiLength] =
        usCrcSummOneByteCalculation(&puiMessage[2], (uiLength - 2));
}

//-----------------------------------------------------------------------------------------------------------------
int16_t CMvsn21::ReportType(uint8_t *puiRequest, uint8_t *puiResponse, uint16_t uiLength)
{
//    *(puiResponse - 1) = 4;
//    puiResponse[0] = 2;
    uint8_t uiCrc = (COMMAND_REPORT_TYPE + MODULE_TYPE_MVSN21);
//    puiResponse[uiLength++] = 0;
    puiResponse[uiLength++] = COMMAND_REPORT_TYPE;
    puiResponse[uiLength++] = MODULE_TYPE_MVSN21;
    puiResponse[uiLength++] = uiCrc;

    return uiLength;
}

//-----------------------------------------------------------------------------------------------------------------
int16_t CMvsn21::ReadData(uint8_t *puiRequest, uint8_t *puiResponse, uint16_t uiLength)
{
//    // Закончено измерение входных каналов?
//    if (m_uiMeasureFlowControl == FSM_IDDLE)
//    {
    memcpy(puiResponse, auiDiscreteInputBitData, 10);
//    }
//    else
//    {
//        // Протокол обмена данными по шине Spi.
//        // Первый байт - пустой.
//        puiResponse[uiLength++] = 0;
//        // Второй байт - "эхо".
//        puiResponse[uiLength++] = COMMAND_DATA_NOT_READY;
//        // Третий байт - команда обмен данными.
//        puiResponse[uiLength++] = COMMAND_DATA_NOT_READY;
//
//
////    // Последний байт - сумма всех байт начиная с третьего(пустой байт и "эхо" не учитываются).
////    puiMessage[uiLength] =
////        usCrcSummOneByteCalculation(&puiMessage[2], (uiLength - 2));
//    }
    return uiLength;
}

//-----------------------------------------------------------------------------------------------------------------
int16_t CMvsn21::SpiReply(uint8_t *puiRequest, uint8_t *puiResponse, uint16_t uiFrameLength)
{
    uint16_t uiLength = uiFrameLength;
    // Какой поступил запрос?
    switch (puiRequest[0])
    {
    case COMMAND_REPORT_TYPE:
        uiLength = ReportType(puiRequest, puiResponse, uiLength);
        break;

    case COMMAND_READ_DATA:
        uiLength = ReadData(puiRequest, puiResponse, uiLength);
        break;

    case COMMAND_READ_DATA_BASE:
//        uiLength = ReadHoldingRegisters(puiRequest, puiResponse, uiLength);
        break;

    case COMMAND_WRITE_DATA_BASE:
//        uiLength = ReadInputRegisters(puiRequest, puiResponse, uiLength);
        break;

    default:
        break;
    }
    return uiLength;
}

//-----------------------------------------------------------------------------------------------------------------
void CMvsn21::SpiFsm(void)
{
    int16_t iReceivedCounter;

    switch (m_uiFlowControl)
    {
    case FSM_IDDLE:
        break;

    case FSM_START:
        Reset();
        m_uiFlowControl = FSM_WAITING_MESSAGE;
        break;

    case FSM_WAITING_MESSAGE:
        if (Select())
        {
            iReceivedCounter = Exchange();

            if (iReceivedCounter > 0)
            {
                m_uiMessageLength += iReceivedCounter;

                if (SpiReply(CSpi::m_puiRxBuffer, CSpi::m_puiTxBuffer, m_uiMessageLength))
                {
                    m_uiFlowControl = FSM_DATA_EXCHANGE_END_WAITING;
                }
                else
                {
                    m_uiFlowControl = FSM_IDDLE;
                }
            }
            else if (iReceivedCounter == -1)
            {
                m_uiFlowControl = FSM_IDDLE;
            }
        }

        break;

    case FSM_DATA_EXCHANGE_END_WAITING:

        break;

    default:
        break;
    }

}

////-----------------------------------------------------------------------------------------------------
//uint8_t CMvsn21::ContinuousMeasure(void)
//{
//    if (m_uiChannel < MEASURE_CHANNEL_NUMBER)
//    {
//        CAdc::ChannelSelect(m_uiChannel++);
//        CAdc::Start();
//        return 0;
//    }
//    else
//    {
//        return 1;
//    }
//}

//-----------------------------------------------------------------------------------------------------
void CMvsn21::MeasureFsm(void)
{
    switch (m_uiMeasureFlowControl)
    {
    case FSM_IDDLE:
        break;

    case FSM_START:
        m_uiChannel = 0;
        CAdc::ChannelSelect(m_uiChannel);
        CAdc::Enable();
        CAdc::Start();
        m_uiMeasureFlowControl = FSM_CONTINUOUS_MEASURE;
        break;

    case FSM_CONTINUOUS_MEASURE:
        if (CAdc::MeasureIsComlete())
        {
            uint16_t uiData = CAdc::GetMeasureValue();
            axChipsChannelsData[MASTER_CHIP_ADDRESS].axMeasurementChannels[m_uiChannel].m_uiState =
                CMeasurementChannel::StatusCheck(CAdc::GetMeasureValue());
            m_uiChannel++;
            if (m_uiChannel < MEASURE_CHANNEL_NUMBER)
            {
                CAdc::Stop();
                CAdc::ChannelSelect(m_uiChannel);
                CAdc::Start();
            }
            else
            {
                CAdc::Disable();
                ChannelsToDiscreteInput();
                m_uiMeasureFlowControl = FSM_IDDLE;
            }
        }
        break;

    default:
        break;
    }
}







////-----------------------------------------------------------------------------------------------------
//CMvsn21Driver::CMvsn21Driver(uint8_t uiType) :
//
//    m_uiType(uiType),
//{
//
//}
//
////-----------------------------------------------------------------------------------------------------
//CMvsn21Driver::~CMvsn21Driver()
//{
//
//}
//
////-----------------------------------------------------------------------------------------------------
//uint16_t CMvsn21Driver::TypeReport(uint8_t *puiDestination, uint16_t uiLength)
//{
//
//}
