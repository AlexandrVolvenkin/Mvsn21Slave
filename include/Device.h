//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#ifndef CDEVICE_H
#define CDEVICE_H

#include <stdint.h>
#include "Driver.h"

//-----------------------------------------------------------------------------------------------------
class CDevice
{
public:
    uint16_t TypeReport(uint8_t * , uint16_t );

protected:
private:
    uint8_t m_uiType;
    CDriver* m_pxDriver;
    uint8_t m_uiFlowControl;

    friend class CMvsn21;
    friend class CPss7;
};

////-----------------------------------------------------------------------------------------------------
//class CPss7 : public CDevice
//{
//public:
//    CPss7(uint8_t uiType, CDriver* pxDriver);
//    virtual ~CPss7();
//    static uint16_t TypeReport(uint8_t * , uint16_t );
//
//protected:
//private:
//    static uint8_t m_uiType;
//    static CDriver* m_pxDriver;
//};

//-----------------------------------------------------------------------------------------------------
#endif // CDEVICE_H
