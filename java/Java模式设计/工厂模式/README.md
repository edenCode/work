## 工厂模式

    设计思想就是利用了java 对象 和 方法可以分离。
    比如有多个不同对象，但是 他们都有相同的方法，这些相同的方法每个对象得到了不同的结果。
    所以把这些相同方法分离出来定义成Interface，这些个不同对象 自己实现这个接口。

    工厂模式 就是 不同工厂 实现相同的Interface 得到不同结果。这样可以在进行功能扩展的时候，不用修改原来的代码。自需要重新实现下这个Interface。

    jdk8 中interface 可以定义 default 方法 可以被重写、也可以重新定义为抽象abstract。也可以不改这个default 方法。

[接口和抽象](https://blog.csdn.net/wangxin1982314/article/details/70804793)
