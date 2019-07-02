#include <iostream>
#include <string>
// libcrytopp
#include "crypto.h"

//md5 
#define TEST_Text " Tist is example a "
int main(int argc,const char ** argv)
{
    std::string out__;
	std::string md5;
	std::string str(TEST_Text);
	//Crypto c;

    Crypto::cryptopp_generate_key(str,out__,str.size());
    std::cout << "Hex Encoded   : " << out__.c_str() << std::endl;
    out__.clear();
	Crypto::cryptopp_md5(str,str.size(),md5);
	std::cout << "MD5 Encoded   : " << md5.c_str() << std::endl;
	std::cout << "MD5 Encoded L : " << md5.size() << std::endl;

    std::string key = md5.substr(0,16);
    //std::cout << "key : " << key.c_str() << " key size : " << key.size() <<  std::endl;

    std::string sha256;
    Crypto::cryptopp_sha256(str,md5,sha256);
    std::cout << "SHA256  Encoded      : " << sha256.c_str() << std::endl;
	std::cout << "SHA256  Encoded size : " << sha256.size() << std::endl;

    std::string aes__;
    
    out__.clear();
    Crypto::cryptopp_ase_encode(str,Crypto::AES_ECB,sha256,sha256,aes__);
    std::cout << "AES ECB Encoded : " << aes__.c_str() << std::endl;
    Crypto::cryptopp_ase_decode(aes__, Crypto::AES_ECB,sha256,sha256,out__);
    std::cout << "AES ECB Decoded : " << out__.c_str() << std::endl;

    aes__.clear();
    out__.clear();
    Crypto::cryptopp_ase_encode(str,Crypto::AES_CBC,sha256,sha256,aes__);
    std::cout << "AES CBC Encoded : " << aes__.c_str() << std::endl;
    Crypto::cryptopp_ase_decode(aes__, Crypto::AES_CBC,sha256,sha256,out__);
    std::cout << "AES CBC Decoded : " << out__.c_str() << std::endl;

    aes__.clear();
    out__.clear();
    Crypto::cryptopp_ase_encode(str,Crypto::AES_CFB,sha256,sha256,aes__);
    std::cout << "AES CFB Encoded : " << aes__.c_str() << std::endl;
    Crypto::cryptopp_ase_decode(aes__, Crypto::AES_CFB,sha256,sha256,out__);
    std::cout << "AES CFB Decoded : " << out__.c_str() << std::endl;

    aes__.clear();
    out__.clear();
    Crypto::cryptopp_ase_encode(str,Crypto::AES_OFB,sha256,sha256,aes__);
    std::cout << "AES OFB Encoded : " << aes__.c_str() << std::endl;
    Crypto::cryptopp_ase_decode(aes__, Crypto::AES_OFB,sha256,sha256,out__);
    std::cout << "AES OFB Decoded : " << out__.c_str() << std::endl;

    aes__.clear();
    out__.clear();
    Crypto::cryptopp_ase_encode(str,Crypto::AES_CTR,sha256,sha256,aes__);
    std::cout << "AES CTR Encoded : " << aes__.c_str() << std::endl;
    Crypto::cryptopp_ase_decode(aes__, Crypto::AES_CTR,sha256,sha256,out__);
    std::cout << "AES CTR Decoded : " << out__.c_str() << std::endl;

    //generate rsa key
    std::string prikey,pubkey;
    std::string seed = "cryptopp_generate_RSAkey";
    //Crypto::cryptopp_generate_RSAkey(prikey,pubkey,seed,2048);
    Crypto::cryptopp_generate_RSAkey1(prikey,pubkey,2048);
    std::cout << "RSA Priave key     : " << prikey.c_str() << std::endl;
    std::cout << "RSA Public key     : " << pubkey.c_str() << std::endl;
    std::cout << "RSA Priave key len : " << prikey.size() << std::endl;
    std::cout << "RSA Public key len : " << pubkey.size() << std::endl;

    std::string prikey1;
    Crypto::cryptopp_save_key("./private_key",prikey);
    Crypto::cryptopp_load_key("./private_key",prikey1);
    if(prikey == prikey1)
    {
        std::cout << "Test two keys is same \n";
    }
    
    std::string result;
    out__.clear();
    Crypto::cryptopp_rsa_encode(pubkey,str,result);
    std::cout << "RSA Encoded : " << result.c_str() << std::endl;
    out__.clear();
    Crypto::cryptopp_rsa_decode(prikey,result,out__);
    std::cout << " RSA Decoded : " << out__.c_str() << std::endl;
   
    std:: string signer;
    Crypto::cryptopp_rsa_signer(prikey,seed,signer,str);

    bool isRight;
    isRight = Crypto::cryptopp_rsa_verify(pubkey,signer,str);
    if(isRight == true)
    {
        std::cout << " RSA verified message \n";
    }
    
    out__.clear();
    result.clear();
    Crypto::cryptopp_base64_encoder(str,result);
    std::cout << "Base64 Encoded : " << result.c_str() << std::endl;
    Crypto::cryptopp_base64_decoder(result,out__);
    std::cout << "Base64 Decoded : " << out__.c_str() << std::endl;
	return 0;
}
