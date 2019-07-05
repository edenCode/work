
# interface 
    default 只能在interface 中使用，子类不能重写 interface定义的静态方法。


```java

// 重写 id() 方法导致 编译错误。

Exception in thread "main" java.lang.Error: Unresolved compilation problem: 
	The method id() of type InterfaceImpl must override or implement a supertype method

	at InterfaceImpl.id(InterfaceImpl.java:15)
InterfaceImpl.java:15
	at InterfaceImpl.main(InterfaceImpl.java:24)

```