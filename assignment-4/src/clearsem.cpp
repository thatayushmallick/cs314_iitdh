#include <iostream>
#include <semaphore.h>
#include <fcntl.h>

using namespace std;

int main(){
    sem_t * smooth_sem = sem_open("/smooth_semaphore", O_CREAT, 0666, 1);
    if(smooth_sem == SEM_FAILED) {
        perror("smooth sem_open");
        exit(EXIT_FAILURE);
    }

    sem_t * detail_sem = sem_open("/detail_semaphore", O_CREAT, 0666, 1);
    if(detail_sem == SEM_FAILED) {
        perror("detail sem_open");
        exit(EXIT_FAILURE);
    }

    int smooth_val;
    sem_getvalue(smooth_sem, &smooth_val);
    cout << "Smooth Semaphore value: " << smooth_val << endl;

    int detail_val;
    sem_getvalue(detail_sem, &detail_val);
    cout << "Detail Semaphore value: " << detail_val << endl;
    
    sem_close(smooth_sem);
    sem_unlink("/smooth_semaphore");

    sem_close(detail_sem);
    sem_unlink("/detail_semaphore");
}