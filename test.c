#include <stdio.h>
#include <stdlib.h>
//该柔性数组大小为4 int array[]当作0处理
typedef struct _soft_array 
{
    int len;
    int array[];
}SoftArray;

int nr_files = 1;

//使用举例
int main(){
    int i=3;
    
   


    nr_files+=i= 4096/sizeof(SoftArray);
    printf("nr_files = %d\n",nr_files);
    return 0;
}


int vfs_normalize_path(const char *directory, const char *filename, char **pathname)
{
    char *fullpath = NULL;
    int namelen;

    namelen = vfs_normalize_path_parame_check(filename, pathname);
    if (namelen < 0) {
        return namelen;
    }

    if ((directory == NULL) && (filename[0] != '/')) {
        PRINT_ERR("NO_WORKING_DIR\n");
        *pathname = NULL;
        return -EINVAL;
    }

    /* 2: The position of the path character: / and the end character /0 */

    if ((filename[0] != '/') && (strlen(directory) + namelen + 2 > TEMP_PATH_MAX)) {

        return -ENAMETOOLONG;
    }

    fullpath = vfs_normalize_fullpath(directory, filename, pathname, namelen);

    if (fullpath == NULL) {
        return -get_errno();
    }

    (void)str_path(fullpath);
    (void)str_normalize_path(fullpath);
    if (strlen(fullpath) >= PATH_MAX) {
        *pathname = NULL;
        free(fullpath);
        return -ENAMETOOLONG;
    }

    *pathname = fullpath;
    return ENOERR;
}