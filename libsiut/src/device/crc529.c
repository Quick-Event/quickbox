//***************************************************************************
//** Autor: Jürgen Ehms
//** 
//** Filename: crc529.c
//** 
//** Description: Programm to generate 16 BIT CRC
//** 
//** Return values: 16 BIT CRC
//** 
//** Errormessages: none
//** 
//** Version    last change    description
//** 
//** 1.00       07.09.2004  
//***************************************************************************


#define POLYNOM 0x8005


unsigned int crc(unsigned int uiCount,unsigned char *pucDat)
{
short int iTmp;
unsigned short int uiTmp,uiTmp1,uiVal;
unsigned char *pucTmpDat;

if (uiCount < 2) return(0);        // response value is "0" for none or one data byte
pucTmpDat = pucDat;

uiTmp1 = *pucTmpDat++;
uiTmp1 = (uiTmp1<<8) + *pucTmpDat++;

if (uiCount == 2) return(uiTmp1);   // response value is CRC for two data bytes
for (iTmp=(int)(uiCount>>1);iTmp>0;iTmp--)
{

if (iTmp>1)
{
  uiVal = *pucTmpDat++;
  uiVal= (uiVal<<8) + *pucTmpDat++;
}
else 
{
  if (uiCount&1)               // odd number of data bytes, complete with "0"         
  {
    uiVal = *pucTmpDat;
    uiVal= (uiVal<<8);
  }
  else
  {
    uiVal=0; //letzte Werte mit 0
  }
} 
  
for (uiTmp=0;uiTmp<16;uiTmp++)
{
   if (uiTmp1 & 0x8000)
   {
      uiTmp1  <<= 1;
      if (uiVal & 0x8000)uiTmp1++;
      uiTmp1 ^= POLYNOM;
   }
   else
   {
      uiTmp1  <<= 1;
      if (uiVal & 0x8000)uiTmp1++;
   }
   uiVal <<= 1;
 }
}
return(uiTmp1);
}