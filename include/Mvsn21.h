//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#ifndef CMVSN21_H
#define CMVSN21_H

#include <stdint.h>
#include "Device.h"
#include "Driver.h"
#include "MeasurementChannel.h"
#include "DataTypes.h"



enum
{
    MODULE_TYPE_MVDI = 0x01,
    MODULE_TYPE_MVDI5 = 0x01,
    MODULE_TYPE_MVSN21 = 0x01,
    MODULE_TYPE_MVDO5 = 0x02,
    MODULE_TYPE_MR53  = 0x03,
    MODULE_TYPE_MR54  = 0x04,
    MODULE_TYPE_MVI   = 0x05
};

//-----------------------------------------------------------------------------------------------------
// Обмен данными модуля ввода дискретных сигналов МВДС9 с МЦП
// * Запрос на циклический обмен:$42
// Ответ:$42-данные готовы(ЭХО), $24-данные не готовы, конец связи
// * На выходе:сост.входов - 4б, доп.байт - 1б -> в sec1 - sec1+4
// байт 1:D0,D1-сост.вх.1, D2,D3-сост.вх.2,D4,D5-сост.вх.3,D6,D7-сост.вх.4
// байт 2:D0,D1-сост.вх.5, D2,D3-сост.вх.6,D4,D5-сост.вх.7,D6,D7-сост.вх.8
// байт 3:D0,D1-сост.вх.9, D2,D3-сост.вх.10,D4,D5-сост.вх.11,D6,D7-сост.вх.12
// байт 4:D0,D1-сост.вх.13, D2,D3-сост.вх.14,D4,D5-сост.вх.15,D6,D7-сост.вх.16
// байт 4:D0,D1-сост.вх.17, D2,D3-сост.вх.18,D4,D5-сост.вх.19,D6,D7-сост.вх.20
// байт 6:D0,D1-сост.вх.21, D2,D3-сост.вх.22,D4,D5-сост.вх.23,D6,D7-сост.вх.24
// байт 7: в ПАС-05 не используется
// байт 8: доп. байт
// байт 9: КС байтов 1-9

//  состояние входов в байтах 1-3:
//  четные биты (D0,D2,D4,D6) =0 - разомкнуто (OFF), =1 - замкнуто (ON)
//  нечетные биты (D1,D3,D5,D7) =0 - достоверно, =1 - недостоверно

class CMvsn21// : virtual public CDevice
{
public:

    enum
    {
        FSM_IDDLE = 0,
        FSM_START,
        FSM_WAITING_MESSAGE,
        FSM_RECEIVE_MESSAGE,
        FSM_RECEIVE_CONFIRMATION,
        FSM_REPLY,
        FSM_FRAME_TRANSMIT,
        FSM_FRAME_TRANSMIT_END_WAITING,
        FSM_RESTART,
        FSM_DATA_EXCHANGE_END_WAITING,
        FSM_CONTINUOUS_MEASURE,
    };

    enum
    {
        COMMAND_DATA_NOT_READY = 0x24,
        COMMAND_READ_DATA = 0x42,
        COMMAND_WRITE_DATA_BASE = 0x81,
        COMMAND_READ_DATA_BASE = 0x82,
        COMMAND_REPORT_TYPE = 0x91,
    };

////    CMvsn21(uint8_t uiType, CDriver* pxDriver);
//    CMvsn21(uint8_t uiType);
//    virtual ~CMvsn21();
    static uint16_t TypeReport(uint8_t * , uint16_t );
//    static void SlaveSelectInterruptEnable(void);
//    static void SlaveSelectInterruptDisable(void);
//    static void SpiBusExchangeEnable(void);
//    static void SpiBusExchangeDisable(void);
//    static void Reset(void);
//    static int16_t Receive(void);
//    static uint8_t Select(void);
//    static int16_t Exchange(void);
//    static uint8_t FrameIsReceived(void);
//    static uint16_t GetFrameLength(void);
//    static int8_t FrameCheck(void);
//    static int8_t FrameCheck(uint8_t * , uint16_t );
//    static int16_t ReportType(uint8_t * , uint8_t * , uint16_t );
//    static int16_t ReadData(uint8_t * , uint8_t * , uint16_t );
//    static int16_t SpiReply(uint8_t * , uint8_t * , uint16_t );
//    static void SpiFsm(void);
//    static void ChannelsToDiscreteInput(void);
////    static uint8_t ContinuousMeasure(void);
    static void MeasureFsm(void);
    static uint8_t FlowControlGet(void)
    {
        return m_uiFlowControl;
    };
    static void FlowControlSet(uint8_t uiData)
    {
        m_uiFlowControl = uiData;
    };
    static uint8_t MeasureFlowControlGet(void)
    {
        return m_uiMeasureFlowControl;
    };
    static void MeasureFlowControlSet(uint8_t uiData)
    {
        m_uiMeasureFlowControl = uiData;
    };

//    static void ConnectDevice(uint8_t );
//    static void DisconnectDevice(void);

protected:
private:
    static uint8_t m_uiType;
//    static CDriver* m_pxDriver;
    static uint8_t m_uiFlowControl;
    static uint8_t m_uiMeasureFlowControl;
//-----------------------------------------------------------------------------------------------------
    static uint16_t m_uiMessageLength;
    static uint8_t m_uiChannel;
    static TChipChannelData axChipsChannelsData[CHIP_NUMBER];
//    static CMeasurementChannel axMasterMeasurementChannels[MEASURE_CHANNEL_NUMBER];
//    static CMeasurementChannel axSlave1MeasurementChannels[MEASURE_CHANNEL_NUMBER];
//    static CMeasurementChannel axSlave2MeasurementChannels[MEASURE_CHANNEL_NUMBER];
//    static TChannelRemap axMeasurementChannelRemap[DISCRETE_INPUT_NUMBER];
    static uint8_t auiDiscreteInputBitData[12];

public:
    static uint8_t m_aucRtuCoilsArray[COILS_WORK_ARRAY_LENGTH];
    static uint8_t m_aucRtuDiscreteInputsArray[DISCRETE_INPUTS_ARRAY_LENGTH];
    static uint16_t m_aucRtuHoldingRegistersArray[HOLDING_REGISTERS_ARRAY_LENGTH];
    static uint16_t m_aucRtuInputRegistersArray[INPUT_REGISTERS_ARRAY_LENGTH];
    static uint8_t m_aui8ReceiveMessageBuff[MODBUS_RTU_MAX_ADU_LENGTH];
    static uint8_t m_aui8TransmitMessageBuff[MODBUS_RTU_MAX_ADU_LENGTH];
//    static uint8_t m_auiSpiRxBuffer[SPI_BUFFER_LENGTH];
//    static uint8_t m_auiSpiTxBuffer[SPI_BUFFER_LENGTH];

};

////-----------------------------------------------------------------------------------------------------
//class CMvsn21Driver : virtual public CDriver
//{
//public:
//    CMvsn21Driver(uint8_t uiType);
//    virtual ~CMvsn21Driver();
//    uint16_t TypeReport(uint8_t * , uint16_t );
//
//protected:
//private:
//};

#endif // CMVSN21_H

