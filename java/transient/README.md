#### 测试java关键字transient

    transient 主要用在java序列化的时候，对某些属性进行修饰，来达到节省内存的效果。

    1. Rectangle.java 实现序列化接口

        ```java
            private transient int area; // 对象被序列化之后这个值会被清空。
        ```

    2. Test.java 测试
#### 测试 结果

    ```java
    start : 
	    width : 3	height : 4	area : 12
    transfer : 
	    width : 3	height : 4	area : 0
    recover : 
	    width : 3	height : 4	area : 12

    ```
