#ifndef __MD5_H__
#define __MD5_H__

#define MD5_SIZE				32

// 根据指定内容计算出32位的MD5值
void md5_hash(unsigned char * output, const unsigned char * input, int len);

#endif
