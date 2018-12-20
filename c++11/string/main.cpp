#include <iostream>
#include <string>

using namespace std;
#define URL "rtmp://192.168.100.1:665/h264"
int main()
{
	string str("rtmp://192.168.100.1:665/h264");
	size_t  pos = 0;
	string s;
	if( (pos = str.find("://")) != string::npos)
	{
		string  st = str.substr(0,pos);
		
		str= str.substr(st.length() + 3);
		cout <<" " << st.length()<<" "<<st.c_str() << endl;
		cout <<" " << str.c_str()<< endl;
		
		pos = str.find("/");
		s = str.substr(0,pos);
		cout <<" " << s.c_str() << endl;
	}
	
	return 0;
}