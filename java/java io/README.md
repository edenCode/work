# 测试 java io
    
    java 文件io 和 网络io

```java
File f = new File("test");

// 字符 流 （一个字符可以由几个字节组成）
// ASCII uTF-8 
FileWriter fw = new FileWriter(f);

//字节流
FileOutputStream fos = new FileOutputStream(f);
```