

class Utils2{
    
    private Utils2(){

    }

    public void printf(){
        System.out.println("Utils2 new Instance");
    }

    // private inner class
    private class SingelInstace{
        private static Utils2 utils2 = new Utils2();
    }

    public static  Utils2 newInstance(){
        return SingelInstace.utils2;
    }
}