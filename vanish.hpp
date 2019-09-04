#include <sys/shm.h>
#include <sys/stat.h>

#define INPUT_MAX 1000
#define SHM_KEY   (key_t)1094
#define SHM_PERM  (S_IRUSR|S_IWUSR |S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)   
