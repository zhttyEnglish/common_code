#include <stdio.h>
#include <stdbool.h>

#include "crc16.h"

const CRC_8 crc_8 = {0x07,0x00,0x00,false,false};
const CRC_8 crc_8_ITU = {0x07,0x00,0x55,false,false};
const CRC_8 crc_8_ROHC = {0x07,0xff,0x00,true,true};
const CRC_8 crc_8_MAXIM = {0x31,0x00,0x00,true,true};
 
const CRC_16 crc_16_IBM = {0x8005,0x0000,0x0000,true,true};
const CRC_16 crc_16_MAXIM = {0x8005,0x0000,0xffff,true,true};
const CRC_16 crc_16_USB = {0x8005,0xffff,0xffff,true,true};
const CRC_16 crc_16_MODBUS = {0x8005,0xffff,0x0000,true,true};
const CRC_16 crc_16_CCITT = {0x1021,0x0000,0x0000,true,true};
const CRC_16 crc_16_CCITT_FALSE = {0x1021,0xffff,0x0000,false,false};
const CRC_16 crc_16_X5 = {0x1021,0xffff,0xffff,true,true};
const CRC_16 crc_16_XMODEM = {0x1021,0x0000,0x0000,false,false};
const CRC_16 crc_16_DNP = {0x3d65,0x0000,0xffff,true,true};
 
const CRC_32 crc_32 = {0x04c11db7,0xffffffff,0xffffffff,true,true};
const CRC_32 crc_32_MPEG2 = {0x04c11db7,0xffffffff,0x00000000,false,false};
 
/*****************************************************************************
*function name:reverse8
*function: 字节反转，如1100 0101 反转后为1010 0011
*input：1字节
*output:反转后字节
******************************************************************************/
uint8_t reverse8(uint8_t data)
{
    uint8_t i;
    uint8_t temp=0;
    for(i=0;i<8;i++)	//字节反转
        temp |= ((data>>i) & 0x01)<<(7-i);
    return temp;
}
/*****************************************************************************
*function name:reverse16
*function: 双字节反转，如1100 0101 1110 0101反转后为1010 0111 1010 0011
*input：双字节
*output:反转后双字节
******************************************************************************/
uint16_t reverse16(uint16_t data)
{
    uint8_t i;
    uint16_t temp=0;
    for(i=0;i<16;i++)		//反转
        temp |= ((data>>i) & 0x0001)<<(15-i);
    return temp;
}
/*****************************************************************************
*function name:reverse32
*function: 32bit字反转
*input：32bit字
*output:反转后32bit字
******************************************************************************/
uint32_t reverse32(uint32_t data)
{
    uint8_t i;
    uint32_t temp=0;
    for(i=0;i<32;i++)		//反转
        temp |= ((data>>i) & 0x01)<<(31-i);
    return temp;
}
 
/*****************************************************************************
*function name:crc8
*function: CRC校验，校验值为8位
*input:addr-数据首地址；num-数据长度（字节）；type-CRC8的算法类型
*output:8位校验值
******************************************************************************/
uint8_t crc8(uint8_t *addr, int num,CRC_8 type)  
{  
    uint8_t data;
    uint8_t crc = type.InitValue;                   //初始值
    int i;  
    for (; num > 0; num--)               
    {  
        data = *addr++;
        if(type.InputReverse == true)
        data = reverse8(data);                 //字节反转
        crc = crc ^ data ;                     //与crc初始值异或 
        for (i = 0; i < 8; i++)                //循环8位 
        {  
            if (crc & 0x80)                    //左移移出的位为1，左移后与多项式异或
                crc = (crc << 1) ^ type.poly;    
            else                               //否则直接左移
                crc <<= 1;                  
        }
    }
    if(type.OutputReverse == true)             //满足条件，反转
        crc = reverse8(crc);
    crc = crc^type.xor;                        //最后返与结果异或值异或
    return(crc);                               //返回最终校验值
}
 
/*****************************************************************************
*function name:crc16
*function: CRC校验，校验值为16位
*input:addr-数据首地址；num-数据长度（字节）；type-CRC16的算法类型
*output:16位校验值
******************************************************************************/
uint16_t crc16(uint8_t *addr, int num,CRC_16 type)  
{  
    uint8_t data;
    uint16_t crc = type.InitValue;					//初始值
    int i;  
    for (; num > 0; num--)               
    {  
        data = *addr++;
        if(type.InputReverse == true)
            data = reverse8(data);				//字节反转
        crc = crc ^ (data<<8) ;					//与crc初始值高8位异或 
        for (i = 0; i < 8; i++)					//循环8位 
        {  
            if (crc & 0x8000)					//左移移出的位为1，左移后与多项式异或
                crc = (crc << 1) ^ type.poly;    
            else		                        //否则直接左移
                crc <<= 1;                  
        }
    }
    if(type.OutputReverse == true)              //满足条件，反转
        crc = reverse16(crc);
    crc = crc^type.xor;	                        //最后返与结果异或值异或
    return(crc);                                //返回最终校验值
}
/*****************************************************************************
*function name:crc32
*function: CRC校验，校验值为32位
*input:addr-数据首地址；num-数据长度（字节）；type-CRC32的算法类型
*output:32位校验值
******************************************************************************/
uint32_t crc32(uint8_t *addr, int num,CRC_32 type)  
{  
    uint8_t data;
    uint32_t crc = type.InitValue;					//初始值
    int i;  
    for (; num > 0; num--)               
    {  
        data = *addr++;
        if(type.InputReverse == true)
            data = reverse8(data);				//字节反转
        crc = crc ^ (data<<24) ;				//与crc初始值高8位异或 
        for (i = 0; i < 8; i++)					//循环8位 
        {  
            if (crc & 0x80000000)				//左移移出的位为1，左移后与多项式异或
                crc = (crc << 1) ^ type.poly;    
            else                                //否则直接左移
                crc <<= 1;                  
        }
    }
    if(type.OutputReverse == true)              //满足条件，反转
        crc = reverse32(crc);
    crc = crc^type.xor;	                        //最后返与结果异或值异或
    return(crc);                                //返回最终校验值
}

