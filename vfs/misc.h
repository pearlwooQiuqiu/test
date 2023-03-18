#ifndef __MISC_H
#define __MISC_H

typedef int lock_t;

typedef int dev_t;
typedef int loff_t;
typedef int size_t;
typedef int time_t;
typedef int rwlock_t;

struct addr_t{
    int addr;
};



struct list_head {
	struct list_head *next, *prev;
};

struct file_system_type{
    const char* name;
};




#endif