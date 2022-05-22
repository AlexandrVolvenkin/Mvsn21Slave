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

uint8_t CMvsn21::m_uiFlowControl;
uint8_t CMvsn21::m_uiChannel;
uint8_t CMvsn21::m_uiMeasureFlowControl;
uint8_t CMvsn21::m_aucRtuDiscreteInputsArray[];
uint8_t CMvsn21::m_aui8ReceiveMessageBuff[];
uint8_t CMvsn21::m_aui8TransmitMessageBuff[];
//-----------------------------------------------------------------------------------------------------
void CMvsn21::MeasureFsm(void)
{
    switch (m_uiMeasureFlowControl)
    {
    case FSM_IDDLE:
        break;

    case FSM_START:
        m_uiChannel = 0;
        CAdc::Enable();
        CAdc::ChannelSelect(m_uiChannel);
        CAdc::Start();
        m_uiMeasureFlowControl = FSM_CONTINUOUS_MEASURE;
        break;

    case FSM_CONTINUOUS_MEASURE:
        if (CAdc::MeasureIsComlete())
        {
            uint8_t uiState =
                CMeasurementChannel::StatusCheck(CAdc::GetMeasureValue());
            m_aucRtuDiscreteInputsArray[m_uiChannel * 2] = (uiState & 0x01);//(uiState & 0x01);
            m_aucRtuDiscreteInputsArray[(m_uiChannel * 2) + 1] = ((uiState >> 1) & 0x01);//((uiState >> 1) & 0x01);

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
                m_uiMeasureFlowControl = FSM_IDDLE;
            }
        }
        break;

    default:
        break;
    }
}
