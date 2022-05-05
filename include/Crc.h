#ifndef CRC_H_INCLUDED
#define CRC_H_INCLUDED


//-----------------------------------------------------------------------------
// crc16 constants.
//-----------------------------------------------------------------------------
#define START_VAL_CRC16 0xFFFF;
#define CRC16_LENGTH 2;
//-----------------------------------------------------------------------------


unsigned short usCrcSummTwoByteCalculation(unsigned char *puiSource, unsigned int nuiNbyte);
int iCrcSummTwoByteCompare(unsigned char *puiSource, unsigned int nuiNbyte);
uint8_t usCrcSummOneByteCalculation(
    uint8_t *puiSource,
    uint16_t nuiNbyte);
int iCrcSummOneByteCompare(unsigned char *puiSource, unsigned int nuiNbyte);
unsigned short usCrc16(unsigned char *puiSource, unsigned short nusLength);
int iCrc16Check(unsigned char *puiSource, unsigned short nusLength);
unsigned short CRC16(const unsigned char *puiSource, unsigned short nusLength);

#endif // CRC_H_INCLUDED
