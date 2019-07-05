

// 该单例模式在多线程中使用会存在问题
class Utils{

    public void printf(String str){
        System.out.println(" " + str);
    }

    // 开放一个获得实例的接口
    public static Utils newInstance(){
        
        if(utils == null){
            utils = new Utils();
        }
        return utils;
    }
    
    // 私有化 构造函数
    private Utils(){
        
    }

    private static Utils utils;
}