
#include "string.h"
#include "iostream"
#include "cstdlib"
#include <vector>
#include <pthread.h>
#include "unistd.h"

char to62Letter(int pos) {
    if(pos<=10) {
        return '0' + pos-1;
    } else if (pos>10 && pos <=36){
        return 'A' + pos-11;
    } else {
        return 'a' + pos-36;
    }
}

static volatile bool IsRunning = true;
void chRun(){
    IsRunning = false;
}

//pthread_mutex_t lock;
//pthread_cond_t cond;
//
//void* fun1(void* arg){
//    pthread_cond_signal();
//}
//
//void* fun2(void* arg){
//    while(1) {
//        sleep(100);
//        pthread_cond_wait(&cond, &lock);
//    }
//}

int main() {
    char t1[10];
    char t2[8] = "testhah";
    strncpy(t1,(char*)t2+1, 9);
    std::cout<<"t1 is:"<<t1<<std::endl;
    uint64_t value = 658031483289; 
    long part1 = (long) (value/1000000ULL);
    long part2 = (long) (value - part1*1000000ULL);
    std::cout<<"part1:"<<part1<<std::endl;
    std::cout<<"part2:"<<part2<<std::endl;
    char buf[20];
    int part3;
    char *pos = (char*) buf + 19;
    *pos-- = 0;
    *pos-- = (char) ('0' + (char) (part2%10)); part2/=10;
    *pos-- = (char) ('0' + (char) (part2%10)); part3 = (int)(part2/ 10);
    *pos-- = ':';
	*pos--= (char) ('0'+(char) (part3%10)); part3/=10;
	*pos--= (char) ('0'+(char) (part3%10)); part3/=10;
	*pos--= ':';
	*pos--= (char) ('0'+(char) (part3%10)); part3/=10;
	*pos--= (char) ('0'+(char) part3);
	*pos--= ' ';
	*pos--= (char) ('0'+(char) (part1%10)); part1/=10;
	*pos--= (char) ('0'+(char) (part1%10)); part1/=10;
	*pos--= '-';
	*pos--= (char) ('0'+(char) (part1%10)); part1/=10;
	*pos--= (char) ('0'+(char) (part1%10)); part3= (int) (part1/10);
	*pos--= '-';
	*pos--= (char) ('0'+(char) (part3%10)); part3/=10;
	*pos--= (char) ('0'+(char) (part3%10)); part3/=10;
	*pos--= (char) ('0'+(char) (part3%10)); part3/=10;
	*pos=(char) ('0'+(char) part3);
    for(size_t i =0;i<sizeof(buf);i++){
        std::cout<<"pos:"<<i<<" ,value:"<<buf[i]<<std::endl;
    }
    std::cout<<"int value"<<'a'<<std::endl;
    std::cout<<"int value"<<'8'-'0'<<std::endl;
    std::cout<<"hash value"<<std::hash<std::string>{}("test1test2")<<std::endl;
    std::string pk;
    pk.push_back('0');
    pk.push_back('1');
    std::string st1 = "jajjajaj";
    std::cout<<"pk"<<pk<<"size:"<<pk.size()<<std::endl;

    std::vector<int> vect(10, 1);
    for(std::vector<int>::iterator loop = vect.begin(); loop != vect.end(); loop++){
        std::cout<<"vect:"<<*loop<<std::endl;
        *loop = 2;
    }
    for(auto loop = vect.begin(); loop != vect.end(); loop++){
        std::cout<<"vect:"<<*loop<<std::endl;
    }
    for(std::vector<int>::const_iterator loop = vect.begin(); loop != vect.end(); loop++){
        std::cout<<"vect:"<<*loop<<std::endl;
    }
    std::vector<int>::iterator it1 = vect.begin();
    std::vector<int>::iterator it2 = vect.end();
    std::cout<<it2-it1<<std::endl;
    int *aa = NULL;
    std::cout<<sizeof(*it1)<<std::endl;
    std::cout<<sizeof(aa)<<std::endl;

    std::cout<<(~(unsigned int)077)<<std::endl;
    std::string test_str = "";
    //注意有&和没有&的区别
    std::string& test_str1 = test_str;
    test_str1.push_back('2');
    std::cout<<"str: "<<test_str<<" ,str1: "<<test_str1<<std::endl;

    static int a = 10;
    static int& a1 = a;
    a1++;
    std::cout<<"a is:"<<a<<std::endl;
}
