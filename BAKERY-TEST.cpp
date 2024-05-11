#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
using namespace std;


const int Tnum=5;
pthread_t threads[Tnum];
int values[Tnum];
pthread_mutex_t busy[Tnum];

void bakery_lock(int num){
    pthread_mutex_lock(&busy[num]);
    int maxe=-1;
    for(int i=0;i<Tnum;i++){
        if(values[i]>maxe){
            maxe=values[i];
        }
    }
    maxe++;
    values[num]=maxe;
    pthread_mutex_unlock(&busy[num]);

    for(int i=0;i<Tnum;i++){
        //seeing it thread is busy assigning number
        pthread_mutex_lock(&busy[i]);
        pthread_mutex_unlock(&busy[i]);

        while((values[i]!=0)&& (values[i]<values[num])){
            //waiting for others turn to happen
        }
    }
}

void bakery_unlock(int num){
    values[num]=0;
}

void* thread_function(void*arg){
    int c=0;
    int *num=(int*)arg;
    while(c<2){
        c++;

        bakery_lock(*num);

        cout<<"Thread "<<*num<<" has entered the critical region\n";
        int t=rand()%3+1;
        while(t>=0){
            cout<<"\t==>Thread "<<*num<<" has "<<t<<" seconds remaining\n";
            sleep(1);
            t--;
        }
        cout<<"Thread "<<*num<<" is leaving the critical region\n\n";
        bakery_unlock(*num);

    }
    pthread_exit(NULL);
}


int main(){
    srand(time(0));
    cout<<"======Starting=====\n\n";
    for(int i=0;i<Tnum;i++){
        pthread_mutex_init(&busy[i],NULL);
        values[i]=0;
    }
    for(int i=0;i<Tnum;i++){
        pthread_create(&threads[i],NULL,thread_function,(void*)(new int(i)));
    }
    
    for(int i=0;i<Tnum;i++){
        pthread_join(threads[i],NULL);
    }
    cout<<"\n=====Ending=====\n";


}