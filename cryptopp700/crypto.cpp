#include <iostream>
#include <string>
#include "osrng.h"
#include "cryptlib.h"
#include "hex.h"
#include "filters.h"
#include "aes.h"
#include "modes.h"
#include "hmac.h"
#include "sha.h"
#include "secblock.h"
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include "md5.h"
#include "rsa.h"
#include "randpool.h"
#include "files.h"
#include "pssr.h"
#include "base64.h"

#include "crypto.h"
using namespace std;
using namespace CryptoPP;

// fuction :  generate key by input str, The len is generated`s key len, must be more than 16. ( len >= 16)
//  str : input
//  len : input key len
// key  :  output
int Crypto::cryptopp_generate_key(string str,string & key,int len)
{
    //string encode;
    if( len < 16 || str.size() < len)
    {
        std::cerr << " str size" << str.size() << "less   " << len <<std::endl;
        return -1;
    }
    
    StringSource((const unsigned char *)str.c_str(),len,true,new HexEncoder(new StringSink(key)));


    return key.size();
}



// Hash
 int Crypto::cryptopp_md5(string data,int size,string & md5)
{

    CryptoPP::Weak::MD5 md;
    string encode;

    StringSource(data.c_str(),true,new HashFilter(md,new StringSink(encode)));

    StringSource(encode.c_str(),true,new HexEncoder(new StringSink(md5)));
    //cout << "MD5 : " << md5.c_str() << endl; 
	return md5.size();
}

 int Crypto::cryptopp_sha256(string data,string key,string & sha256)
{
	string key1;
    string encode;

    if(key.size() < 16)
    {
        std::cerr << " key lenght is error \n";
        return -1;
    }
    
    key1 = key.substr(0,16); 
    //cout << "key str : " << key1.c_str()<<" key size : " << key1.size()<<endl;
    SecByteBlock key2((const unsigned char*)key1.c_str(),key1.size());
    
    StringSource(key2,key2.size(),true,new HexEncoder(new StringSink(encode)));
    //cout << "sha key : " << encode.c_str() <<" Len :" << encode.size()<< endl;

    HMAC< SHA256 > hmac(key2, key2.size());

    StringSource(data, true, new HashFilter(hmac,new StringSink(encode)) /*HashFilter */);


    StringSource(encode.c_str(),true,new HexEncoder(new StringSink(sha256)));
    
	return 0;
}


/**
 *
 *  AES : ECB CBC CFB OFB CRT
 *
 **/
 int Crypto::cryptopp_ase_encode(string data,ASE_MODE mode,string key,string iv,string & out)
{
	// key  >= 16 位
	// iv 16 位
	// ECB 模式下不需要初始化iv
	string encode;
    //byte __iv[AES::BLOCKSIZE];
    //byte __key[AES::DEFAULT_keyLength];
    string key1;
    string iv1;
   
    if(key.size() < AES::DEFAULT_KEYLENGTH)
	{
        std::cerr << "key lenght  less than " << AES::DEFAULT_KEYLENGTH << std::endl;
        return -1;
    }
    
    if(iv.size() < AES::BLOCKSIZE && mode != AES_ECB)
    {
        std::cerr << "Iv key len less then " << AES::BLOCKSIZE << std::endl;
        return -1;
    }

    key1 = key.substr(0,AES::DEFAULT_KEYLENGTH);
    iv1  = iv.substr(0,AES::BLOCKSIZE);
    //std::cout <<" key : " << key1.c_str() << " len : " << key1.size() << std::endl;
    //std::cout <<" iv key : " << iv1.c_str() << " iv len : " << iv1.size() << std::endl;
    SecByteBlock __key((const unsigned char*)key1.c_str(),key1.size()),__iv((const unsigned char*)iv1.c_str(),iv1.size());  
    switch (mode)
    {
        case AES_ECB:
        {
            CryptoPP::ECB_Mode<AES>::Encryption e;
            e.SetKey(__key,__key.size());
            StringSource(data, true, 
			    new StreamTransformationFilter(e,
				new StringSink(encode)
			    ) // StreamTransformationFilter  
            ); // StringSource
#if 1
            StringSource(encode, true,
		        new HexEncoder(
			    new StringSink(out)
		        ) // HexEncoder
	       
		    );
#else
         out = encode;
#endif
            break;
        }
        case AES_CBC:
        {

            CryptoPP::CBC_Mode<AES>::Encryption e;
            
            e.SetKeyWithIV(__key,__key.size(),__iv,__iv.size());
            StringSource(data, true, 
			    new StreamTransformationFilter(e,
				new StringSink(encode)
			    ) // StreamTransformationFilter  
            ); // StringSource

            StringSource(encode, true,
		        new HexEncoder(
			    new StringSink(out)
		        ) // HexEncoder
	       
		    );
           break;
        }
        case AES_CFB:
        {

            CryptoPP::CFB_Mode<AES>::Encryption e;
            
           e.SetKeyWithIV(__key,__key.size(),__iv,__iv.size());
            StringSource(data, true, 
			    new StreamTransformationFilter(e,
				new StringSink(encode)
			    ) // StreamTransformationFilter  
            ); // StringSource

            StringSource(encode, true,
		        new HexEncoder(
			    new StringSink(out)
		        ) // HexEncoder
		    );
            break;
        }
        case AES_OFB:
        {
            CryptoPP::OFB_Mode<AES>::Encryption e;
            
            e.SetKeyWithIV(__key,__key.size(),__iv,__iv.size());
            StringSource(data, true, 
			    new StreamTransformationFilter(e,
				new StringSink(encode)
			    ) // StreamTransformationFilter  
            ); // StringSource

            StringSource(encode, true,
		        new HexEncoder(
			    new StringSink(out)
		        ) // HexEncoder
		    );
            break;
        }
        case AES_CTR:
        {
            CryptoPP::CTR_Mode<AES>::Encryption e;
            
          e.SetKeyWithIV(__key,__key.size(),__iv,__iv.size());
            StringSource(data, true, 
			    new StreamTransformationFilter(e,
				new StringSink(encode)
			    ) // StreamTransformationFilter  
            ); // StringSource

            StringSource(encode, true,
		        new HexEncoder(
			    new StringSink(out)
		        ) // HexEncoder
		    );
            break;
        }
        default:
        std::cerr << "This mode not impl \n";
            break;
    }

    
	return out.size();
}

 int Crypto::cryptopp_ase_decode(string data1,ASE_MODE mode,string key,string iv,string & out)
{
	// key  >= 16 位
	// iv 16 位
	// ECB 模式下不需要初始化iv
	//string encode;
    string data;
    //byte __iv[AES::BLOCKSIZE];
    //byte __key[AES::DEFAULT_KEYLENGTH];
    string key1;
    string iv1;

    if(key.size() < AES::DEFAULT_KEYLENGTH)
	{
        std::cerr << "key lenght  less than " << AES::DEFAULT_KEYLENGTH << std::endl;
        return -1;
    }
    
    if( mode != AES_ECB && iv.size() < AES::BLOCKSIZE)
    {
        std::cerr << "Iv key len less then " << AES::BLOCKSIZE << std::endl;
        return -1;
    }

    key1 = key.substr(0,AES::DEFAULT_KEYLENGTH);
    iv1  = iv.substr(0,AES::BLOCKSIZE);
    //std::cout <<" key : " << key1.c_str() << " len : " << key1.size() << std::endl;
    //std::cout <<" iv key : " << iv1.c_str() << " iv len : " << iv1.size() << std::endl;

    SecByteBlock __key((const unsigned char*)key1.c_str(),key1.size()),__iv((const unsigned char*)iv1.c_str(),iv1.size()); 

    //Decoder into byte
    StringSource(data1, true,new HexDecoder(new StringSink(data)) /*HexDecoder*/);
    
    switch (mode)
    {
        case AES_ECB:
        {
           CryptoPP::ECB_Mode<AES>::Decryption e;
            e.SetKey(__key,__key.size());
            StringSource(data, true, 
			    new StreamTransformationFilter(e,
				new StringSink(out)
			    ) // StreamTransformationFilter  
            ); // StringSource

            
            break;
        }
        case AES_CBC:
        {

            CryptoPP::CBC_Mode<AES>::Decryption e;
            
            e.SetKeyWithIV(__key,__key.size(),__iv,__iv.size());
            StringSource(data, true, 
			    new StreamTransformationFilter(e,
				new StringSink(out)
			    ) // StreamTransformationFilter  
            ); // StringSource

           break;
        }
        case AES_CFB:
        {

            CryptoPP::CFB_Mode<AES>::Decryption e;
            
            e.SetKeyWithIV(__key,__key.size(),__iv,__iv.size());
            StringSource(data, true, 
			    new StreamTransformationFilter(e,
				new StringSink(out)
			    ) // StreamTransformationFilter  
            ); // StringSource


            break;
        }
        case AES_OFB:
        {
            CryptoPP::OFB_Mode<AES>::Decryption e;
            
            e.SetKeyWithIV(__key,__key.size(),__iv,__iv.size());
            StringSource(data, true, 
			    new StreamTransformationFilter(e,
				new StringSink(out)
			    ) // StreamTransformationFilter  
            ); // StringSource


            break;
        }
        case AES_CTR:
        {
            CryptoPP::CTR_Mode<AES>::Decryption e;
            
           e.SetKeyWithIV(__key,__key.size(),__iv,__iv.size());
            StringSource(data, true, 
			    new StreamTransformationFilter(e,
				new StringSink(out)
			    ) // StreamTransformationFilter  
            ); // StringSource

      
            break;
        }
        default:
        std::cerr << "This mode not impl \n";
            break;
    }
    
	return out.size();
}

 static RandomPool & GlobalRNG()

{

       static RandomPool randomPool;

       return randomPool;

}

// RSA 2048
int Crypto::cryptopp_generate_RSAkey(string & prikey,string& pubkey,string seed,int keylen)
{
    RandomPool randPool;
    randPool.IncorporateEntropy((byte *)seed.c_str(),seed.size());
 
    RSAES_OAEP_SHA_Decryptor priv(randPool, keylen);
    HexEncoder privFile(new StringSink(prikey));
    priv.AccessMaterial().Save(privFile);
    privFile.MessageEnd();
 
    RSAES_OAEP_SHA_Encryptor pub(priv);
    HexEncoder pubFile(new StringSink(pubkey));
    pub.AccessMaterial().Save(pubFile);
    pubFile.MessageEnd();
    return 0;
}

int Crypto::cryptopp_generate_RSAkey1(string& prikey,string& pukey,int keylen)
{
    
    string prikey__,pukey__;
    AutoSeededRandomPool rng;

    InvertibleRSAFunction parameters;
    parameters.GenerateRandomWithKeySize(rng, keylen); 

    //生成公钥和私钥
    RSA::PrivateKey privateKey( parameters );//私钥用于加密(n,d)
    RSA::PublicKey  publicKey( parameters ); //公钥用于解密(n,e)

    privateKey.Save(StringSink(prikey__).Ref());
    publicKey.Save(StringSink(pukey__).Ref());

    //std::cout << "privateKey : " << prikey__ << std::endl;
    //std::cout << "publickey  : " << pukey__ << std::endl;
    // HexEncode
    StringSource((const unsigned char *)prikey__.c_str(),prikey__.size(),true,new HexEncoder(new StringSink(prikey)));
    StringSource((const unsigned char *)pukey__.c_str(),pukey__.size(),true,new HexEncoder(new StringSink(pukey)));
    
    return 0;
}


int Crypto::cryptopp_rsa_encode(string pubkey,const string message,string & chiper)
{
    // HexDecode
    StringSource s((const byte *)pubkey.c_str(),pubkey.size(),true,new HexDecoder);
    RSAES_OAEP_SHA_Encryptor pub(s);
 
    //RandomPool randPool;
    //randPool.IncorporateEntropy((byte *)seed.c_str(),seed.size());

    StringSource(message, true, new PK_EncryptorFilter(GlobalRNG(), pub, new HexEncoder(new StringSink(chiper))));
    return chiper.size();
}

int  Crypto::cryptopp_rsa_decode(string prikey,const string &         ciphertext,string &result)
{
     // HexDecode
    StringSource s((const byte *)prikey.c_str(),prikey.size(),true,new HexDecoder);
    RSAES_OAEP_SHA_Decryptor priv(s);

    StringSource(ciphertext, true, new HexDecoder(new PK_DecryptorFilter(GlobalRNG(), priv, new StringSink(result))));
    return result.size();
}


/**
*   RSA 签名
**/
int Crypto::cryptopp_rsa_signer(string prikey,string seed,string & signer_data,string data)
{
    HexDecoder decoder;
    RSA::PrivateKey prikey__;
    
    decoder.Put((const byte *)prikey.c_str(),prikey.size());
    decoder.MessageEnd();
    prikey__.Load(decoder);
    
    
    RSASS<PSS, SHA1>::Signer signer(prikey__);
   // signer.AccessKey().Load(decoder);
    
    // Create signature space
    size_t length = signer.MaxSignatureLength();
    SecByteBlock signature(length);

    // Sign message
    length = signer.SignMessage(GlobalRNG(),(const byte*) data.c_str(),data.length(),signature);

    // Resize now we know the true size of the signature

    signature.resize(length);

    //signer_data = *signature;
    StringSource(signature,signature.size(),true,new HexEncoder(new StringSink(signer_data)));
    
    return signer_data.size();
}

/**
*   RSA 验证签名
**/
bool Crypto::cryptopp_rsa_verify(string pubkey,string & signa,string data)
{

    RSA::PublicKey pubkey__;
    HexDecoder decoder;
    decoder.Put((const byte *)pubkey.c_str(),pubkey.size());
    decoder.MessageEnd();

    pubkey__.Load(decoder);
    
    RSASS<PSS, SHA1>::Verifier verifier(pubkey__);
    
    std::string signare;
    StringSource(signa,true,new HexDecoder(new StringSink(signare)));
    SecByteBlock signature((const byte *)signare.c_str(),signare.size());
    
    bool result = verifier.VerifyMessage((const byte*)data.c_str(),data.length(), signature, signature.size());

    return result;
}

// save a key into file
int Crypto::cryptopp_save_key(string path,string key)
{
     StringSource(key.c_str(),true,new Base64Encoder(new FileSink(path.c_str())));
    return 0;
}

// Load a key from file
int Crypto::cryptopp_load_key(string path,string & key)
{
      FileSource(path.c_str(),true,new Base64Decoder(new StringSink(key)));
      return 0;
}

int Crypto::cryptopp_base64_encoder(string in,string & out)
{
    StringSource(in.c_str(),true,new Base64Encoder(new StringSink(out)));
    return 0;
}

int Crypto::cryptopp_base64_decoder(string in,string & out)
{
    StringSource(in.c_str(),true,new Base64Decoder(new StringSink(out)));
    return 0;
}

#if 0
int Crypto::test_cryptopp_rsa()
{
AutoSeededRandomPool rng;

InvertibleRSAFunction parameters;
parameters.GenerateRandomWithKeySize(rng, 1536);

RSA::PrivateKey privateKey(parameters);
RSA::PublicKey publicKey(parameters);

// Message
string message = "Yoda said, Do or Do Not. There is no try.";

// Signer object
RSASS<PSSR, SHA256>::Signer signer(privateKey);

// Create signature space
size_t length = signer.MaxSignatureLength();
SecByteBlock signature(length);

// Sign message
length = signer.SignMessage(rng, (const byte*) message.c_str(),
    message.length(), signature);

// Resize now we know the true size of the signature
signature.resize(length);

// Verifier object
RSASS<PSSR, SHA256>::Verifier verifier(publicKey);

// Verify
bool result = verifier.VerifyMessage((const byte*)message.c_str(),
    message.length(), signature, signature.size());

// Result
if(true == result) {
    cout << "Signature on message verified" << endl;
} else {
    cout << "Message verification failed" << endl;
}
}

string SignMessage( const std::string& privateKeyFileName, const std::string& message )
{
    std::string signedMessage = "";
    string encString;
    FileSource privFile( privateKeyFileName.c_str(), true, new Base64Decoder(new StringSink(encString)));
    RSASSA_PKCS1v15_SHA_Signer priv;

    HexDecoder decoder;
    decoder.Put( (byte*)encString.c_str(), encString.size() );
    decoder.MessageEnd();

    priv.AccessKey().Load(decoder);

    AutoSeededRandomPool rng;
    StringSource s1(message, true, new SignerFilter(rng, priv, new HexEncoder(new StringSink(signedMessage))));
    
    return string(signedMessage.c_str());
}

bool CKeyController::VerifySignature( const std::string& publicKeyFileName, const std::string& message, const std::string& signedMessage )
{
    string decString;
    FileSource pubFile( publicKeyFileName.c_str(), true, new Base64Decoder(new StringSink(decString)) );
    RSASSA_PKCS1v15_SHA_Verifier pub;

    HexDecoder decoder;
    decoder.Put( (byte*)decString.c_str(), decString.size() );
    decoder.MessageEnd();

    pub.AccessKey().Load(decoder);

    StringSource signatureFile( signedMessage, true, new HexDecoder);
    if (signatureFile.MaxRetrievable() != pub.SignatureLength())
    { throw std::string( "Signature Size Problem" ); }

    SecByteBlock signature(pub.SignatureLength());
    signatureFile.Get(signature, signature.size());

    VerifierFilter *verifierFilter = new VerifierFilter(pub);
    verifierFilter->Put(signature, pub.SignatureLength());
    StringSource s(message, true, verifierFilter);

    return verifierFilter->GetLastResult();
}
#endif
