#include "misc.h"

struct sblock
{
    struct list_head s_list; //指向所有超级块的链表
    
    //基本
    unsigned long s_magic;//用于区分不同文件系统的类型
    struct file_system_type s_type; //表示该类型文件系统的特性
    void *s_fs_info; //兼容性体现，指向底层文件系统的特殊信息

    //挂载相关
    bool s_mounted_flag;
    dev_t            s_dev;//该超级块所代表的文件系统 在哪一块设备上挂载
    struct block_device *s_bdev;//该文件系统所挂载的块相关的块设备
    struct dentry * s_root;

    
    lock_t s_lock;
    int s_count;
    unsigned char    s_dirty; //是否需要回写到底层磁盘
    int s_space_used; //该文件系统的已用空间
    int s_space_capacity; //该文件系统的可用空间
    unsigned long    s_blocksize;//VFS层一次文件操作基本单位的抽象
    unsigned char    s_blocksize_bits;
    unsigned long long s_maxbytes;//单个文件大小上限
    
    struct list_head  s_inodes;
    struct list_head  s_files;
    struct list_head  s_dentry_lru;
    struct list_head  s_instance; //VFS运行挂载同类型多个文件系统
    
    struct super_operations *s_op;
    
};

struct super_operations{
    struct inode *(*alloc_inode)(struct sblock *);
    int (*detroy_inode)(struct inode *);
    int (*read_inode)(struct inode *);
    int (*write_inode)(struct inode *);

    int (*write_super)(struct sblock*);
    int (*put_super)(struct sblock*);

    int (*statfs)(struct sblock*,struct statfs*);

};


struct vFile
{
    int  f_type; 
    char* f_path;
    struct vNnode * f_vnode;
    lock_t f_lock;
    int f_count;
    int f_flags;
    int f_mode;
    int f_pos;
    struct file_operations *f_op;
    const struct cred *f_cred;
    struct file_ra_head *f_ra; //在打开的时候预读一个扇区
};

struct task_files
{
    long  hashID; 
    int next_fd;
    struct fdtable* fdtable;
};

struct file_operations
{
    int (*open)(struct vnode *, struct vFile *);
    int (*close)(struct vnode *, struct vFile *);
    
    int (*read)(struct vnode *, struct vFile *, char *,int);
    int (*write)(struct vnode *, struct vFile *, char *,int);

    int (*lseek) (struct vnode *, struct vFile *, int, int);
    int (*ioctl)(struct vnode *, struct vFile *,unsigned int,unsigned long);

};


struct vNode{
    //文件基本信息
    int vn_type;
    time_t vn_atime;
    time_t vn_mtime;
    time_t vn_ctime;
    int vn_mode;
    int vn_flag;

    //文件所有者信息
    int vn_uid;
    int vn_gid;


    //文件访问信息
    rwlock_t vn_rwlock;
    int  vn_refcnt;

    int vn_dirty;
    int vn_hash;


    struct list_head vn_list;//打开文件对应的索引节点链表
    struct vnode_operations *vn_vops;
    struct file_operations *vn_fops;

    int isMntPoint;
    struct vMount * vn_oldMnt;
    struct vMount * vn_newMnt;

    struct vNnode* parent;
    struct vNnode* child;
    struct vNnode* siblings;
    struct list_head* vn_parentPC;
    struct list_head* vn_childPC;
    struct list_entry* vn_hashEntry;
    struct list_entry* vn_statusEntry;
    void * vn_private;

    int vn_dev; //设备标识符号
};

//目录结构描述
struct vDirent {
    int     dd_fd;          /* 目录文件描述符 */
    long    dd_loc;         /* 当前读取的目录项位置 */
    long    dd_size;        /* 目录大小 */
    char    *dd_buf;        /* 目录项缓存 */
    int     dd_len;         /* 目录项缓存长度 */
} ;

struct vnode_operations{
    int (*create)(struct vNode *parent, const char *name, int mode, struct vNode **vnode);
    int (*lookup)(struct vNode *parent, const char *name, int len, struct vNode **vnode);
    int (*open)(struct vNode *vnode, int fd, int mode, int flags);
    int (*close)(struct vNode *vnode);

    int (*mkdir)(struct vNode* parent, const char *dirName, int mode, struct Vnode **vnode);
    int (*rmdir)(struct vNode* parent,struct Vnode *vnode, const char *dirName);
    int (*rename)(struct vNode* src,struct Vnode *dstParent, const char *srcName, const char *dstName);
    int (*getattr)(struct vNode *vnode, struct stat *st);
    int (*setattr)(struct vNode *vnode, struct stat *st);

    
    int (*opendir)(struct vNode *vnode, struct vDir ** dir); //返回一个目录项结构体
    int (*readdir)(struct vNode *vnode, struct vDirent ** dir); //返回一个目录项列表
    int (*rewinddir)(struct vNode *vnode, struct vDirent *dir, int pos,int whence);
    int (*closedir)(struct vNode *vnode, struct vDirent *dir);
    

    void (*truncate)(struct vNode* vnode,int len); //要truncate的文件长度
    

//符号链接还没有完善
    int (*Unlink)(struct Vnode *parent, struct Vnode *vnode, const char *fileName);
    int (*Link)(struct Vnode *src, struct Vnode *dstParent, struct Vnode **dst, const char *dstName);
    int (*Symlink)(struct Vnode *parentVnode, struct Vnode **newVnode, const char *path, const char *target);
    ssize_t (*Readlink)(struct Vnode *vnode, char *buffer, size_t bufLen);
};


struct list_entry{
  int a;
};


struct vPathComponent
{
    int namelen; 
    struct vNode *upVnode;
    struct vNode *downVnode;
    struct list_entry upEntry;
    struct list_entry downEntry;
    struct list_entry hashEntry;
    char name[0];
    
};


struct Mount {
    LIST_ENTRY mountList;              /* 挂载链表由双向链表全局统一管理 */
    const struct MountOps *ops;        /* operations of mount */
    struct Vnode *vnodeBeCovered;      /* 记录挂载在根文件系统中的哪个节点上 */
    struct Vnode *vnodeCovered;        /* syncer vnode */
    struct Vnode *vnodeDev;            /* dev vnode */
    LIST_HEAD vnodeList;               /* list of vnodes */
    int vnodeSize;                     /* size of vnode list */
    LIST_HEAD activeVnodeList;         /* list of active vnodes */
    int activeVnodeSize;               /* szie of active vnodes list */
    void *data;                        /* private data */
    uint32_t hashseed;                 /* Random seed for vfshash */
    unsigned long mountFlags;          /* Flags for mount */
    char pathName[PATH_MAX];           /* path name of mount point */
    char devName[PATH_MAX];            /* path name of dev point */
};

#define PATH_MAX 256

struct vMount {
    struct list_entry mntList; //挂载链表
    struct vMount * parent;
    struct vNode * oldMount; //old被隐藏
    struct vNode * newMount;//挂载点对应的根目录
    
    struct list_head vnodeList; 
    int num_of_vnodeList;
    struct list_head activeVnodeList; 
    int num_of_activeVnodeList;

    struct mountpoint_operations *ops;
    void *data;                        /* private data */
    unsigned int hashseed;                 /* Random seed for vfshash */
    unsigned long mountFlags;          /* Flags for mount */
    char pathName[PATH_MAX];           /* path name of mount point */
    char devName[PATH_MAX];            /* path name of dev point */
};

struct mountpoint_operations {
    int (*Mount)(struct Mount *mount, struct Vnode *vnode, const void *data);
    int (*Unmount)(struct Mount *mount, struct Vnode **blkdriver);
    int (*Statfs)(struct Mount *mount, struct statfs *sbp);
    int (*Sync)(struct Mount *mount);
};


// 根据题目要求，定义角色Role的结构体
struct Role {
  string name;  // 角色名称
  vector<Role*> children;  // 角色的孩子节点
  // 其他角色相关的属性和方法...
};

// 根据题目要求，定义vDentry的结构体
struct vDentry {
  string path;  // 路径
  Role* role;   // 角色
  // 其他vDentry相关的属性和方法...
};

// 定义一个函数，实现添加角色的功能
void addRole(Role* Radmin, Role* Rx, vDentry* vden_Radmin, vDentry* vden_Rx) {
  Role* Ry = Radmin;  // 设置当前角色Ry为Radmin
  while (true) {
    // 求vden_Radmin和vden_Rx的最小公共祖先
    vDentry* vden_ancestor = findMinCommonAncestor(vden_Radmin, vden_Rx);
    if (vden_ancestor == vden_Ry) {
      // 如果最小公共祖先是当前角色Ry，说明Ry是Rx的祖先
      break;
    } else {
      // 否则，说明Rx是Ry的祖先，更新当前角色Ry为Rx
      Ry = Rx;
      vden_Ry = vden_Rx;
    }
  }

  // 遍历Ry的孩子节点
  for (Role* child : Ry->children) {
    vDentry* vden_child = findVdentry(child, vden_Ry->path);
    vDentry* vden_ancestor = findMinCommonAncestor(vden_child, vden_Rx);
    if (vden_ancestor == vden_child) {
      // 如果最小公共祖先是Ry的子节点，则继续递归查找
      addRole(child, Rx, vden_child, vden_Rx);
      return;
    }
  }

  // 如果Rx和Ry的所有子节点都不是其祖先，则Rx和Ry为兄弟节点，将Rx插入Ry的孩子节点中
  Ry->children.push_back(Rx);
}


struct wait_queue_head_t{
  int a;
};


struct file_rwlock{
  struct file_lock* fl_next; //指向下一个锁
  struct list_head fl_block; //指向等待锁的进程列表的指针
  struct files_struct* fl_owner;
  unsigned int fl_taskID;
  unsigned int fl_type;
  wait_queue_head_t fl_wait;
  struct vFile* file;
  struct file_lock_operations* fl_ops;


};

struct file_lock_operations{

};


//关于目录
struct dirent {
    ino_t d_ino;           // 文件的 inode 号
    off_t d_off;           // 目录项的偏移量
    unsigned short d_reclen;// 目录项的长度
    unsigned char d_type;  // 文件的类型
    char d_name[256];      // 文件的名称
};


typedef struct {
    int      dd_fd;       // 目录的文件描述符
    long     dd_loc;      // 目录中下一个目录项的偏移量
    unsigned dd_seek;     // 目录项在目录中的位置
    void     *dd_dirp;    // 目录指针
} DIR;



struct fs_dirent_s
{
  /* This is the node that was opened by opendir.  The type of the vnode
   * determines the way that the readdir() operations are performed. For the
   * pseudo root pseudo-file system, it is also used to support rewind.
   *
   * We hold a reference on this vnode so we know that it will persist until
   * closedir() is called (although vnodes linked to this vnode may change).
   */

  struct Vnode *fd_root;

  /* At present, only mountpoints require special handling flags */

  unsigned int fd_flags;

  /* This keeps track of the current directory position for telldir */
  off_t fd_position; //当前目录位置

  /* This keeps track of the internal offset for some FSs */
  off_t fd_int_offset;


  /* Retained control information depends on the type of file system that
   * provides is provides the mountpoint.  Ideally this information should
   * be hidden behind an opaque, file-system-dependent void *, but we put
   * the private definitions in line here for now to reduce allocations.
   */

  struct
    {
      /* Private data used by the built-in pseudo-file system */
      struct fs_pseudodir_s pseudo;

      /* Private data used by other file systems */
      fs_dir_s fs_dir;
#ifdef CONFIG_FS_FAT
      struct fs_fatdir_s     fat;
#endif

#ifdef CONFIG_FS_SMARTFS
      struct fs_smartfsdir_s smartfs;
#endif


#ifdef LOSCFG_FS_ZPFS
      void                   *zpfs;
#endif
    } u;

  /* In any event, this the actual struct dirent that is returned by readdir */
#ifdef LOSCFG_ENABLE_READ_BUFFER
  struct dirent fd_dir[MAX_DIRENT_NUM];              /* Populated when readdir is called */
#else
  struct dirent fd_dir[1];              /* Populated when readdir is called */
#endif
  int16_t       cur_pos;
  int16_t       end_pos;
  int32_t       read_cnt;
  int           fd_status;           /* Express the dirent is been opened or no */
};


struct vnodeOps fatfs_vops = {
    /* file ops */
    .Unlink = fatfs_unlink,
    .Truncate = fatfs_truncate,
    .Getattr = fatfs_stat,
    .Chattr = fatfs_chattr,
    .Lookup = fatfs_lookup,
    .Rename = fatfs_rename,
    .Create = fatfs_create,

    /* dir ops */
    .Opendir = fatfs_opendir,
    .Readdir = fatfs_readdir,
    .Rewinddir = fatfs_rewinddir,
    .Closedir = fatfs_closedir,
    .Mkdir = fatfs_mkdir,
    .Rmdir = fatfs_rmdir,
    .link = fatfs_link,
    .Symlink = fatfs_symlink,
    .Readlink = fatfs_readlink,
};

struct MountOps fatfs_mops = {
    .Mount = fatfs_mount,
    .Unmount = fatfs_umount,
    .Statfs = fatfs_statfs,
    .Sync = fatfs_sync_adapt,
    .Format = fatfs_format,
};

struct vfileOps fatfs_fops = {
    .open = fatfs_open,
    .close = fatfs_close,
    .read = fatfs_read,
    .write = fatfs_write,
    .lseek = fatfs_lseek,
    .ioctl = fatfs_ioctl,
};