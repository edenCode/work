#include <iostream>

class A{
	public:
		// 构造函数
		A(){
			std::cout << "A()" << std::endl;
		}
		A(A&a){
			std::cout << "复制构造函数" << std::endl;
		}

};


class B{
	public:
		A a;

		B():a(){ // 参数列表中 a() 会调用构造函数
			std::cout << "B()" << std::endl;
		}
};

int main(){
	A a0; // 构造
	A a(a0);// 复制构造

	A a1(); // 这个不会调用A 类的构造函数
	//A b = a();

	B b; // 这个在参数列表中会调用 A类的z构造函数

}
