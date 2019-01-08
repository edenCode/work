#include <iostream>
#include <pthread>

class IThread{
public:
    virtual void run() = 0;
};


class ThreadPool{
private:
    vector<IThread> threads;

public:
    
    
};
