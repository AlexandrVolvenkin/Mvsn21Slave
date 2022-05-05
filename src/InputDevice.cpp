//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#include "InputDevice.h"

//-----------------------------------------------------------------------------------------------------
CInputDevice::CInputDevice()
{

}

//-----------------------------------------------------------------------------------------------------
CInputDevice::CInputDevice(uint8_t (*fpuiEventSource)(void)) :
    m_fpuiEventSource(fpuiEventSource)
{
    m_uiEventCode = KEY_EVENT_UNPRESSED;
    m_uiFlowControl = KEY_EVENT_UNPRESSED;
}

//-----------------------------------------------------------------------------------------------------
CInputDevice::~CInputDevice()
{

}

//-----------------------------------------------------------------------------------------------------
void CInputDevice::SetCapabilities(uint8_t (*fpuiEventSource)(void))
{
    m_fpuiEventSource = fpuiEventSource;
    m_uiEventCode = KEY_EVENT_UNPRESSED;
    m_uiFlowControl = KEY_EVENT_UNPRESSED;
}

//-----------------------------------------------------------------------------------------------------
uint8_t CInputDevice::Report(uint8_t uiType, uint8_t uiCode)
{

}

//-----------------------------------------------------------------------------------------------------
uint8_t CInputDevice::Report(uint8_t uiCode)
{

}

//-----------------------------------------------------------------------------------------------------
uint8_t CInputDevice::EventHappened(void)
{
    if (m_fpuiEventSource())
    {
        return 1;
    }
    else
    {
        return 0;
    }

}

//-----------------------------------------------------------------------------------------------------
uint8_t CInputDevice::KeyEventHappened(uint8_t uiCode)
{
    if (m_uiEventCode == uiCode)
    {
        m_uiEventCode = KEY_EVENT_UNPRESSED;
        return 1;
    }
    else
    {
        return 0;
    }

}

//-----------------------------------------------------------------------------------------------------
void CInputDevice::KeyScan(void)
{
    // какое событие обрабатываем?
    switch (m_uiFlowControl)
    {
    case KEY_EVENT_UNPRESSED:
        m_uiFlowControl = KEY_EVENT_WAITING_PRESSED;
        break;

    case KEY_EVENT_WAITING_PRESSED:
        // кнопка нажата?
        if (EventHappened())
        {
            m_xTimer.Set(KEY_PRESSED_TIME_PUSH);
            m_uiFlowControl = KEY_EVENT_PRESSED_PUSH;
        }
        break;
    case KEY_EVENT_PRESSED_PUSH:
        // кнопка нажата?
        if (EventHappened())
        {
            if (m_xTimer.IsOverflow())
            {
                m_xTimer.Set(KEY_PRESSED_TIME_HOLD);
                m_uiEventCode = KEY_EVENT_PRESSED_PUSH;
                m_uiFlowControl = KEY_EVENT_PRESSED_HOLD;

            }
        }
        else
        {
            m_uiFlowControl = KEY_EVENT_UNPRESSED;
        }
        break;
    case KEY_EVENT_PRESSED_HOLD:
        // кнопка нажата?
        if (EventHappened())
        {
            if (m_xTimer.IsOverflow())
            {
                m_xTimer.Set(KEY_PRESSED_HOLD_REPEAT_TIME);
                m_uiEventCode = KEY_EVENT_PRESSED_HOLD;
                m_uiFlowControl = KEY_EVENT_PRESSED_REPEAT;

            }
        }
        else
        {
            m_uiFlowControl = KEY_EVENT_UNPRESSED;
        }
        break;

    case KEY_EVENT_PRESSED_REPEAT:
        // кнопка нажата?
        if (EventHappened())
        {
            if (m_xTimer.IsOverflow())
            {
                m_xTimer.Set(KEY_PRESSED_HOLD_REPEAT_TIME);
                m_uiEventCode = KEY_EVENT_PRESSED_REPEAT;
            }
        }
        else
        {
            m_uiFlowControl = KEY_EVENT_UNPRESSED;
        }
        break;
    default:
        break;
    }
}
