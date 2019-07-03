### Java 反射

    java 反射 指动态的获取一个类的对象。

1. 通过包名+类名

```java

import java.lang.reflect.*;


class Test{
    public static void main(String[] args) {
        //Class clazz;
        // Not import Student.class
        try {

            // 通过 类名来获得 Class 对象
            Class clazz = Class.forName("Student");

            // 通过 Class 对象获得 类名
            System.out.println("Class Name : \n \t" +clazz.getName());
            //System.out.println(clazz.getFields());

            // 通过 Class 对象获得 定义的 属性字段
            Field [] fields = clazz.getDeclaredFields();
            System.out.println("Fileds : ");
            for(Field f: fields){
                System.out.println("\t"+f);
            }

            // 通过 Class 对象获得  student 对象的构造方法
            Constructor [] constructors = clazz.getDeclaredConstructors();
            System.out.println("Constructors : ");
            for(Constructor c : constructors){
                System.out.println("\t"+c);
            } 

            // 通过Class 对象 获取 所有的方法（包含默认继承的Object对象的方法）
            Method [] methods = clazz.getMethods();
            System.out.println("Methods : ");
            for(Method m: methods ){
                System.out.println("\t" + m);
            }


            // 通过获取构造方法创建一个Student 对象。 （这里调用了带有参数的构造方法）
            Object obj = clazz.getDeclaredConstructor(String.class,int.class).newInstance("Gongcm",12);

            // 获得Student 对象特定的方法 toString 方法，该方法是被重写了的。
            Method m1 = clazz.getDeclaredMethod("toString");
            // 调用该方法 invoke 调用
            System.out.println(m1.invoke(obj));
            // setSex 带参数的方法，通过方法名 和 方法参数签名信息获取。
            Method m2 = clazz.getDeclaredMethod("setSex",int.class);
            m2.invoke(obj,1);
            System.out.println(m1.invoke(obj));

            // setAge 带参数的方法，通过方法名 和 方法参数签名信息获取。
            //
            Method m3 = clazz.getMethod("setAge",int.class);
            m3.invoke(obj,12);
            System.out.println(m1.invoke(obj));

            //System.out.println("load student ");

        } catch (Exception e) {
            //TODO: handle exception
            System.out.println(e.getMessage()+": " + e.toString());
        }

            //Object obj = clazz.newInstance();


    }
}

```

    运行结果：
            Class Name :
                 Student
            Fileds :
                private int Student.age
                private java.lang.String Student.name
                private int Student.sex
            Constructors :
                Student(java.lang.String,int)
                Student(java.lang.String,int,int)
                Student()
            Methods :
                public java.lang.String Student.toString()
                public void Student.setAge(int)
                public void Student.setSex(int)
                public final void java.lang.Object.wait() throws java.lang.InterruptedException
                public final void java.lang.Object.wait(long,int) throws java.lang.InterruptedException
                public final native void java.lang.Object.wait(long) throws java.lang.InterruptedException
                public boolean java.lang.Object.equals(java.lang.Object)
                public native int java.lang.Object.hashCode()
                public final native java.lang.Class java.lang.Object.getClass()
                public final native void java.lang.Object.notify()
                public final native void java.lang.Object.notifyAll()
            
            Name : Gongcm   Age : 0 Sex : 12
            Name : Gongcm   Age : 0 Sex : 1
            Name : Gongcm   Age : 12    Sex : 1

#### 总结

    上面的例子中使用了java反射机制，动态的创建一个类的对象。在该实例中并没有导入Student这个文件。
    通过 Class.forName("Student") 方法来构造一个Class 对象。
    
    Class.forName 方法：
        就是动态的加载一个编译的class文件（例如：Student.class文件）到内存中，并且解析，
        创建一个CLass对象来进行描述这个已经加载到内存中class文件中一个类的所有信息（包含 类属性、类方法，父类属性 和 父类方法）.

    Class 对象理解：
        由于java 文件中一个类对应一个java文件，编译之后的class 文件保存了这个类所有信息。
        java 虚拟机加载该文件之后会创建一个Class对象保存这些信息

#### Class.forName 调用流程

```java

//java.lang.Class
public static Class<?> forName(String className)
                throws ClassNotFoundException {
        Class<?> caller = Reflection.getCallerClass();

        return forName0(className, true, ClassLoader.getClassLoader(caller), caller);
    }

// java.lang.Class
// native 方法,最后调用这个返回一个Class 对象
// 该对象应该是在 C 代码中产生的。
// @name className  类名
// @ initialize 初始化
// @loader class loader 对象
// @Class<?> caller 
 /** Called after security check for system loader access checks have been made. */
private static native Class<?> forName0(String name, boolean initialize,
                                            ClassLoader loader,
                                            Class<?> caller)throws ClassNotFoundException;

// java.lang.Reflection
public static native  java.lang.Class<?> getCallerClass();


//ClassLoader 对象
// Returns the class's class loader, or null if none.
static ClassLoader getClassLoader(Class<?> caller) {
        // This can be null if the VM is requesting it
        if (caller == null) {
            return null;
        }
        // Circumvent security check since this is package-private
        return caller.getClassLoader0();
}

//

ClassLoader getClassLoader0() { return classLoader; }

//通过构造方法传递
private Class(ClassLoader loader) {
        // Initialize final field for classLoader.  The initialization value of non-null
        // prevents future JIT optimizations from assuming this final field is null.
        classLoader = loader;
    }

```

#### native 调用

    通过查看 openjdk 12 源码获得。

```cpp

```

