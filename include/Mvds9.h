//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#ifndef CMVDS9_H
#define CMVDS9_H

#include <stdint.h>
#include "Device.h"
#include "Driver.h"
#include "MeasurementChannel.h"



enum
{
    MODULE_TYPE_MVDI = 0x01,
    MODULE_TYPE_MVDI5 = 0x01,
    MODULE_TYPE_MVDS9 = 0x01,
    MODULE_TYPE_MVDO5 = 0x02,
    MODULE_TYPE_MR53  = 0x03,
    MODULE_TYPE_MR54  = 0x04,
    MODULE_TYPE_MVI   = 0x05
};

//-----------------------------------------------------------------------------------------------------
class CMvsn21 : virtual public CDevice
{
public:

    enum
    {
        MASTER_DATA_EXCHANGE_MAX_MESSAGE_LENGTH = 64,
        MEASURE_CHANNEL_NUMBER = 8,
        DISCRETE_INPUT_NUMBER = 24,
        MEASURE_CHANNEL_STATE_BIT_NUMBER = 2,
        CHANNELS_IN_BYTE = 4,
        CHIP_NUMBER = 3,
    };

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

    struct TChipChannelData
    {
        CMeasurementChannel axMeasurementChannels[MEASURE_CHANNEL_NUMBER];
    };

    struct TChannelRemap
    {
        uint8_t uiChip;
        uint8_t uiChannel;
    };

    CMvsn21(uint8_t uiType, CDriver* pxDriver);
    virtual ~CMvsn21();
    static uint16_t TypeReport(uint8_t * , uint16_t );
    static void SlaveSelectInterruptEnable(void);
    static void SlaveSelectInterruptDisable(void);
    static void SpiBusExchangeEnable(void);
    static void SpiBusExchangeDisable(void);
    static void Reset(void);
    static int16_t Receive(void);
    static uint8_t Select(void);
    static int16_t Receive(uint8_t * , uint16_t );
    static int16_t Exchange(uint8_t *, uint8_t *, uint16_t  );
    static uint8_t FrameIsReceived(void);
    static uint16_t GetFrameLength(void);
    static int8_t FrameCheck(void);
    static int8_t FrameCheck(uint8_t * , uint16_t );
    static int16_t ReportType(uint8_t * , uint8_t * , uint16_t );
    static int16_t ReadData(uint8_t * , uint8_t * , uint16_t );
    static int16_t SpiReply(uint8_t * , uint8_t * , uint16_t );
    static void SpiFsm(void);
    static void ChannelsToDiscreteInput(void);
    static uint8_t ContinuousMeasure(void);
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
    static CDriver* m_pxDriver;
    static uint8_t m_uiFlowControl;
    static uint8_t m_uiMeasureFlowControl;
//-----------------------------------------------------------------------------------------------------
    static uint16_t m_uiMessageLength;
    static uint8_t m_uiChannel;
    static TChipChannelData axChipsChannelsData[CHIP_NUMBER];
//    static CMeasurementChannel axMasterMeasurementChannels[MEASURE_CHANNEL_NUMBER];
//    static CMeasurementChannel axSlave1MeasurementChannels[MEASURE_CHANNEL_NUMBER];
//    static CMeasurementChannel axSlave2MeasurementChannels[MEASURE_CHANNEL_NUMBER];
    static TChannelRemap axMeasurementChannelRemap[DISCRETE_INPUT_NUMBER];
    static uint8_t auiDiscreteInputBitData[12];

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

#endif // CMVDS9_H

