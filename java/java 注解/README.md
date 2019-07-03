
#### Java 注解

---




```java

// RetentionPolicy.class

public enum RetentionPolicy {
    /**
     * Annotations are to be discarded by the compiler. // 会被编译器丢掉
     */
    SOURCE,

    /**
     * Annotations are to be recorded in the class file by the compiler
     * but need not be retained by the VM at run time.  This is the default
     * behavior. // 不会被丢掉，但是 不能在runtime 时保留
     */
    CLASS,

    /**
     * Annotations are to be recorded in the class file by the compiler and
     * retained by the VM at run time, so they may be read reflectively.
     *  可以在运行时 保留
     * @see java.lang.reflect.AnnotatedElement
     */
    RUNTIME
}

// Inter.java 

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

//这个时在java 运行时
@Retention(RetentionPolicy.RUNTIME)
public @interface Inter{
    String msg() default "aaa";
}

// Test.java

@Inter(msg="haha")
class Test{
    public static void main(String[] args) {

        // 设置之后生成两个class文件
        System.setProperty("sun.misc.ProxyGenerator.saveGeneratedFiles", "true");

        // java 反射 得到动态对象
        // 在java虚拟机中 每个class 文件被加载时 都会有一个Class 对象对这个class文件进行解析，解析完毕后这个文件就会产生一个Class 对象
        // 这个Class 对象保存了 class文件中的所有属性（包含 类变量 和 类方法）
        // 详情 请见： java.lang.Class 定义，这个是个final 类 不能被继承。
        Inter annotation = Test.class.getAnnotation(Inter.class);
        System.out.println("Inter msg : "+ annotation.msg());
        // print : msg : haha
    }
}

```

    Result print msg : haha


### javap

  javap   用法: javap <options> <classes>

  其中, 可能的选项包括:
    -help  --help  -?        输出此用法消息
    -version                 版本信息
    -v  -verbose             输出附加信息
    -l                       输出行号和本地变量表
    -public                  仅显示公共类和成员
    -protected               显示受保护的/公共类和成员
    -package                 显示程序包/受保护的/公共类
                           和成员 (默认)
    -p  -private             显示所有类和成员
    -c                       对代码进行反汇编
    -s                       输出内部类型签名
    -sysinfo                 显示正在处理的类的
                           系统信息 (路径, 大小, 日期, MD5 散列)
    -constants               显示最终常量
    -classpath <path>        指定查找用户类文件的位置
    -cp <path>               指定查找用户类文件的位置
    -bootclasspath <path>    覆盖引导类文件的位置

1. **javap $Proxy0.class**

```java

// Retention 实现
public final class com.sun.proxy.$Proxy0 extends java.lang.reflect.Proxy implements java.lang.annotation.Retention {
  public com.sun.proxy.$Proxy0(java.lang.reflect.InvocationHandler) throws ;
  public final boolean equals(java.lang.Object) throws ;
  public final java.lang.String toString() throws ;
  public final java.lang.Class annotationType() throws ;
  public final int hashCode() throws ;
  public final java.lang.annotation.RetentionPolicy value() throws ;
  static {} throws ;
}
```

2.**javap $Proxy1.class**

```java

 // 自定义注解 Inter 实现 
// Inter.class 中 继承了 注解类 Annotation
public interface Inter extends java.lang.annotation.Annotation {
  public abstract java.lang.String msg(); // 抽象方法
}

// Inter 抽象实现类
public final class com.sun.proxy.$Proxy1 extends java.lang.reflect.Proxy implements Inter
{
  public com.sun.proxy.$Proxy1(java.lang.reflect.InvocationHandler) throws ;
  public final boolean equals(java.lang.Object) throws ;
  public final java.lang.String toString() throws ;
  public final java.lang.String msg() throws ; // 实现了 msg 方法
  public final java.lang.Class annotationType() throws ;
  public final int hashCode() throws ;
  static {} throws ;
}

```
