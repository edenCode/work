#include <iostream>
#include <string>

namespace Crypto {

    typedef  enum{
         AES_ECB = 0,
         AES_CBC,
         AES_CFB,
         AES_OFB,
         AES_CTR
      }ASE_MODE;

    //base Function
    int cryptopp_generate_key(std::string str,std::string & key,int len = 16);
    int cryptopp_save_key(std::string path,std::string key);
    int cryptopp_load_key(std::string path,std::string & key);
    
    // Hash Function
	int cryptopp_md5(std::string data,int size,std::string & md5);
	int cryptopp_sha256(std::string data,std::string key,std::string & md5);

    // ASE Function
    int cryptopp_ase_encode(std::string data,ASE_MODE mode,std::string key,std::string iv,std::string & out);
	int cryptopp_ase_decode(std::string data,ASE_MODE mode,std::string key,std::string iv,std::string & out);




    //  \\ RSA fuction 
    
    /**
    *   param seed : in, Random seed.
    *   keylen     : in,key`s len.
    *   prikey     : out,RSA private key,The size is keylen.
    *   pubkey     : out,RSA public key,The size is keylen.
    *   Function   :  generate a pair of key for RSA.
    **/
    int cryptopp_generate_RSAkey(std::string & prikey,std::string& pubkey,std::string seed,int keylen);
    int cryptopp_generate_RSAkey1(std::string& prikey,std::string& pukey,int keylen);

    int cryptopp_rsa_signer(std::string prikey,std::string seed,std::string & signer_data,std::string data);
    bool cryptopp_rsa_verify(std::string pubkey,std::string & signature,std::string data);
    
    int cryptopp_rsa_encode(std::string pubkey,const std::string message,std::string & chiper);
    int cryptopp_rsa_decode(std::string prikey,const std::string & ciphertext,std::string &result);

    // base64 function
    int cryptopp_base64_encoder(std::string in,std::string & out);
    int cryptopp_base64_decoder(std::string in,std::string & out);

}


