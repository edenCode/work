#include <iostream>
#include <atomic>
#include <string>
#include <vector>
#include <thread>
#include <sstream> // 字符串操作

std::atomic<int> flag(0);
std::atomic_flag lock = ATOMIC_FLAG_INIT;
std::stringstream stream;

int test_sstream(std::string & buf)
{
	std::string str;
	std::istringstream ss(buf);
	
	do{
		ss >> str;
		std::cout << str << std::endl;
	}while(ss)
}

void append_number(int x)
{
	while(lock.test_and_set());
		stream << "thread # " << x << "\n";
}	

int main(int argc,char * argv[])
{
	std::vector<std::thread> threads;
	std::string buf("Lies have short legs. 谎言站不长。");
	test_sstream(buf);
	
	for(int i = 0;i < 10;i ++)
		threads.push_back(std::thread(append_number,i));
	
	for(int i = 0; i < 10; i ++)
		threads[i].join();
	
	std::cout << stream.str()<< std::endl;
	return 0;
}