
#ifndef SPSDECODE_H_
#define SPSDECODE_H_

#include "os_define.h"


uint32_t Ue(uint8_t *pBuff, uint32_t nLen, uint32_t &nStartBit);
int Se(uint8_t *pBuff, uint32_t nLen, uint32_t &nStartBit);
DWORD u(uint32_t BitCount, uint8_t * buf, uint32_t &nStartBit);

/**
* H264的NAL起始码防竞争机制
*
* @param buf SPS数据内容
*
* @无返回值
*/
void de_emulation_prevention(uint8_t* buf, unsigned int* buf_size);


/**
* 解码SPS,获取视频图像宽、高信息
*
* @param buf SPS数据内容
* @param nLen SPS数据的长度
* @param width 图像宽度
* @param height 图像高度

* @成功则返回1 ,失败则返回0
*/
int h264_decode_sps(uint8_t * buf, unsigned int nLen, unsigned int &width, unsigned int &height/*, unsigned int &fps*/);

#endif /* SPSDECODE_H_ */
