//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#ifndef CMEASUREMENTCHANNEL_H
#define CMEASUREMENTCHANNEL_H

#include <stdint.h>

//-----------------------------------------------------------------------------------------------------
class CMeasurementChannel
{
public:
    enum ADC_LEVELS
    {
        WB_LEVEL	=  23,			// Уровень обрыва линии
        ON_LEVEL 	= 143,			// Уровень срабатывания
        OFF_LEVEL	= 161,			// Уровень деактивации
        SC_LEVEL	= 554			// Уровень короткого замыкания
    };

    enum PREFERENCES
    {
        DEBOUNCE_LEVELS = 150,		// Уровень фильтрации
        OUT_MASK	  = 0x00FFFFFF		// Маска выходных данных
    };

    enum SPECIAL_CODES
    {
        DEAD_ZONE	= 0xFF,			// Зона нечуствительности
        STOP_COUNT	= 0xFF,			// Признак останова счета
        INVALID	= 0x0F			// Недостоверность значения	[##]
    };

    enum FLAGS
    {
        MASTER_VALID	= 0,			// Флаг достоверности данных ведущего МК
        SLAVE_VALID	= 1			// Флаг достоверности данных ведомого МК
    };

    enum INPUT_STATE_CODE
    {
        WIRE_BREAK	= 0x02,			// Обрыв линии
        ON		= 0x01,			// Срабатывание датчика
        OFF		= 0x00,			// Дективация датчика
        SHORT_CIRCUIT	= 0x03			// Короткое замыкание
    };

    enum STATUS_CODE
    {
        NORM		= 0x00,			// Нормальное завершение
        DATA_WAIT	= 0x01,			// Ожидание готовности данных
    };

    CMeasurementChannel();
    virtual ~CMeasurementChannel();
    static uint8_t StatusCheck(uint16_t );

protected:
private:
//    uint16_t m_uiMeasuredValue;
    uint8_t m_uiState;
//    uint8_t m_uiChannelRemap;

    friend class CMvsn21;
};
//-----------------------------------------------------------------------------------------------------

#endif // CMEASUREMENTCHANNEL_H
