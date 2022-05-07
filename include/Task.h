//-----------------------------------------------------------------------------------------------------
//  Sourse      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#ifndef CTASK_H
#define CTASK_H

#include <stdint.h>

#include "Timer.h"

//-----------------------------------------------------------------------------------------------------
class CTask
{
public:
    enum
    {
        IDDLE = 0,
        START,
        STOP,
        RESTART,
    };

//    CTask();
//    virtual ~CTask();
//-----------------------------------------------------------------------------------------------------
//    virtual void Init(void) = 0;
    virtual void Execution(void) = 0;
    uint8_t FlowControlGet(void)
    {
        return m_uiFlowControl;
    };
    void FlowControlSet(uint8_t uiData)
    {
        m_uiFlowControl = uiData;
    };
    uint8_t StateGet(void)
    {
        return m_uiState;
    };
    void StateSet(uint8_t uiData)
    {
        m_uiState = uiData;
    };

//-----------------------------------------------------------------------------------------------------
protected:
private:
    uint8_t m_uiFlowControl;
    uint8_t m_uiState;
    uint8_t m_uiOwnAddress;
    CTimer m_xTimer;

    friend class CUartMT;
    friend class CModbus;
    friend class CModbusRTU;
    friend class CModbusMasterRTU;
};

//-----------------------------------------------------------------------------------------------------
#endif // CTASK_H
