//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#ifndef CDRIVER_H
#define CDRIVER_H

#include <stdint.h>

//-----------------------------------------------------------------------------------------------------
class CDriver
{
public:
//    CDriver(uint8_t uiType);
//    virtual ~CDriver();
    uint16_t TypeReport(uint8_t * , uint16_t );

protected:
private:
    uint8_t m_uiType;
    uint8_t m_uiFlowControl;

    friend class CMvsn21;
    friend class CPss7;
};

//-----------------------------------------------------------------------------------------------------
#endif // CDRIVER_H
