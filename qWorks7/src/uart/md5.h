#ifndef __MD5_H__
#define __MD5_H__

#define MD5_SIZE				32

// ����ָ�����ݼ����32λ��MD5ֵ
void md5_hash(unsigned char * output, const unsigned char * input, int len);

#endif
