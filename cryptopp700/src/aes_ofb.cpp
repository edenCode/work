// g++ -g3 -ggdb -O0 -DDEBUG -I/usr/include/cryptopp Driver.cpp -o Driver.exe -lcryptopp -lpthread
// g++ -g -O2 -DNDEBUG -I/usr/include/cryptopp Driver.cpp -o Driver.exe -lcryptopp -lpthread

#include "osrng.h"
using CryptoPP::AutoSeededRandomPool;

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include <string>
using std::string;

#include <cstdlib>
using std::exit;

#include "cryptlib.h"
using CryptoPP::Exception;

#include "hex.h"
using CryptoPP::HexEncoder;
using CryptoPP::HexDecoder;

#include "filters.h"
using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::StreamTransformationFilter;

#include "aes.h"
using CryptoPP::AES;

#include "modes.h"
using CryptoPP::OFB_Mode;

typedef unsigned char byte;


class Crypto{
public:
	static int cryptopp_md5(string data,int size,string & md5);
	static int cryptopp_sha256(string data,int size,string & md5);
	// key iv, ase crt ofb
	static int cryptopp_ase_encode(string key,int size,string key,string iv,string & out);
	static int cryptopp_ase_decode(string data,int size,string key,string iv,string & out);
	// for base 64
	
	// RSA 2048
};
int main(int argc, char* argv[])
{
	AutoSeededRandomPool prng;

	byte key[AES::DEFAULT_KEYLENGTH];
	prng.GenerateBlock(key, sizeof(key));

	byte iv[AES::BLOCKSIZE];
	prng.GenerateBlock(iv, sizeof(iv));

	string plain = "OFB Mode Test";
	string cipher, encoded, recovered;

	/*********************************\
	\*********************************/

	// Pretty print key
	encoded.clear();
	StringSource(key, sizeof(key), true,
		new HexEncoder(
			new StringSink(encoded)
		) // HexEncoder
	); // StringSource
	cout << "key: " << encoded << endl;

	// Pretty print iv
	encoded.clear();
	StringSource(iv, sizeof(iv), true,
		new HexEncoder(
			new StringSink(encoded)
		) // HexEncoder
	); // StringSource
	cout << "iv: " << encoded << endl;

	/*********************************\
	\*********************************/

	try
	{
		cout << "plain text: " << plain << endl;

		OFB_Mode< AES >::Encryption e;
		e.SetKeyWithIV(key, sizeof(key), iv);

		// OFB mode must not use padding. Specifying
		//  a scheme will result in an exception
		StringSource(plain, true, 
			new StreamTransformationFilter(e,
				new StringSink(cipher)
			) // StreamTransformationFilter      
		); // StringSource
	}
	catch(const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}

	/*********************************\
	\*********************************/

	// Pretty print
	encoded.clear();
	StringSource(cipher, true,
		new HexEncoder(
			new StringSink(encoded)
		) // HexEncoder
	); // StringSource
	cout << "cipher text: " << encoded << endl;

	/*********************************\
	\*********************************/

	try
	{
		OFB_Mode< AES >::Decryption d;
		d.SetKeyWithIV(key, sizeof(key), iv);

		// The StreamTransformationFilter removes
		//  padding as required.
		StringSource s(cipher, true, 
			new StreamTransformationFilter(d,
				new StringSink(recovered)
			) // StreamTransformationFilter
		); // StringSource

		cout << "recovered text: " << recovered << endl;
	}
	catch(const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}

	/*********************************\
	\*********************************/

	return 0;
}

