#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/init.h>	    /* Needed for the macros */
#include <linux/fs.h>       /* This is where libfs is declared */
#include <linux/stat.h>     /* For S_IFDIR  */
#include <linux/errno.h>    /* For ENOMEM EINVAL*/
#include <linux/string.h>
#include <linux/vfs.h>
#include <linux/slab.h>


#include <linux/pagemap.h>  /*PAGE_CACHE_SIZE*/

#include <asm/atomic.h>
#include <asm/uaccess.h>    /*copy to user*/


MODULE_LICENSE("GPL");


//QIFS_MAGIC
#define QIFS_MAGIC 0x71696673 


struct qifs_sb_info
{
    unsigned int rsize;
    unsigned int wsize;
    int flags;
    struct nls_table *local_nls;
};


static struct super_operations qifs_ops;
static struct inode_operations qifs_dir_inode_ops;
static struct inode_operations qifs_file_inode_ops;
//static struct file_operations  qifs_dir_operations;
static struct file_operations  qifs_file_operations;
static struct address_space_operations qifs_aops;
static struct inode *qifs_get_inode(struct super_block *sb, int mode, dev_t dev);

static int qifs_mknod(struct inode *dir, struct dentry *dentry, int mode, dev_t dev);
static int qifs_mkdir(struct inode *dir, struct dentry *dentry, int mode);
/*
//qifs_read, return a pointer to a buffer containing at least LEN bytes of filesystem starting at byte offset OFFSET into the filesystem.

static void *qifs_read(struct super_block *sb, unsigned int offset, unsigned int len)
{

}
*/



//qifs_file_open
static int qifs_file_open(struct inode *inode, struct file *filp)
{
	    return generic_file_open(inode, filp);
}

/*
//qifs_read_file
static ssize_t qifs_read_file(struct file *filp, char *buf,
	 	                 size_t count, loff_t *offset)
{
	
}

//qifs_write_file
static ssize_t qifs_write_file(struct file *filp, const char *buf,
		size_t count, loff_t *offset)
{
	
}

*/



/*
//qifs_dentry_ops

static struct dentry_operations qifs_dentry_ops =
{

    .d_delete = simple_delete_dentry,
}    
*/

//qifs_lookup
//Lookup the data, if the dentry didn't already exist, it must be negative. 
static struct dentry *qifs_lookup(struct inode *dir, struct dentry *dentry,
                                struct nameidata *nd)
{
    printk(KERN_INFO "qifs: qifs_lookup\n");
    return simple_lookup(dir, dentry, nd);
/*    struct qifs_sb_info  *isb = dir->i_sb->s_fs_info;
    if(dentry->d.name.len > NAME_MAX)   // NAME_MAX, 255 chars in a file name 
        return ERR_PTR(-ENAMETOOLONG);
    dentry->d_op = &qifs_dentry_ops;    //need define

    d_add(dentry, NULL);
    return NULL;
*/

}



//qifs_create
static int qifs_create(struct inode *dir, struct dentry *dentry, int mode, struct nameidata *nd)
{
    printk(KERN_INFO "qifs: qifs_create\n");
    return qifs_mknod(dir, dentry, mode| S_IFREG, 0);
}    



//qifs_mknod

static int qifs_mknod(struct inode *dir, struct dentry *dentry, int mode, dev_t dev)
{
    struct inode *inode = qifs_get_inode(dir->i_sb, mode,dev); 
    int error = -ENOSPC;
    
    printk(KERN_INFO "qifs: mknod \n");

    if (inode)
    {
//        if(dir->i_mode & S_ISGID)
//            {
//                inode->i_gid = dir->i_gid;
//                if(S_ISDIR(mode))
//                    inode->i_mode |=S_ISGID;
//            }    
        d_instantiate(dentry, inode);
        dget(dentry);
        error = 0;
        dir->i_mtime =dir->i_ctime = CURRENT_TIME;
     }
    return error;
}


//qifs_mkdir

static int qifs_mkdir(struct inode *dir, struct dentry *dentry, int mode)
{
    printk(KERN_INFO "qifs: qifs_mkdir\n");
    int retval = 0;
    retval= qifs_mknod(dir, dentry, mode|S_IFDIR, 0);
    if(!retval)
      dir->__i_nlink++;
    return retval;

}    

static struct address_space_operations qifs_aops =
{
//    .readpage   = qifs_readpage,  //need to define
      .readpage       = simple_readpage,    
      .write_begin    = simple_write_begin,
      .write_end      = simple_write_end,
};



//file operations for file
static struct file_operations qifs_file_operations =
{
    .open       = qifs_file_open,
    .read       = do_sync_read,
    .aio_read   = generic_file_aio_read,
    .write      = do_sync_write,
    .aio_write  = generic_file_aio_write,
    .mmap       = generic_file_mmap,
    .llseek     = generic_file_llseek,            //all generic function

};    


//file operations for dir
/*
 /qifs_dir_operations
static struct file_operations qifs_dir_operations =
{   
    .llseek   = generic_file_llseek,
    .read     = generic_read_dir,

};
*/



//inode_operations for file
static struct inode_operations qifs_file_inode_ops =
{
    .getattr    = simple_getattr,
};    


//inode_operations for dir
static struct inode_operations qifs_dir_inode_ops =
{
    .create     = qifs_create,     //need define
//    .lookup     = simple_lookup,    
    .lookup     = qifs_lookup,      //need define
    .link       = simple_link,
    .unlink     = simple_unlink,
    .mkdir      = qifs_mkdir,        //need define     
//  .rmdir      = simple_rmdir,
    .mknod      = qifs_mknod,       //need define      
//  rename      = simple_rename,
};    


//qifs_get_inode
//When we make a file or directory, we need an inode to represent it.
//The parameter "mode", tells the it is dirctory or file

static struct inode* qifs_get_inode(struct super_block *sb, int mode, dev_t dev)
{
    
    
    struct inode *ret = new_inode (sb);     //generic function
    if (ret)
    {
        ret->i_ino = get_next_ino();
         
        ret->i_mode = mode;
//        ret->i_uid = current->fsuid;
//        ret->i_gid = current->fsgid;
        ret->i_uid = 0;
        ret->i_gid = 0;
        ret->i_blocks = 0;
        ret->i_atime = ret->i_mtime =ret->i_ctime = CURRENT_TIME;

        printk(KERN_INFO   "qifs: to set inode operations \n");

        ret->i_mapping->a_ops = &qifs_aops;     //need to define;

        switch(mode & S_IFMT)
        {
            case S_IFREG:
                printk(KERN_INFO "qifs: file inode \n");
                ret->i_op = &qifs_file_inode_ops;              //need define
                ret->i_fop =&qifs_file_operations;             //need define
                break;

            case S_IFDIR:
                printk(KERN_INFO "qifs: directory inode sb->s_fs_info: %p \n", sb->s_fs_info);
                ret->i_op = &qifs_dir_inode_ops;//need define
                ret->i_fop = &simple_dir_operations;
                ret->__i_nlink++;//link= 2, initial ".." and "."
                break;

            default:
                init_special_inode(ret, mode, dev);

        }// end of switch
    } //end of if

    return ret;

}


//qifs_put_super
static void qifs_put_super(struct super_block *sb)
{
    kfree(sb->s_fs_info);
    sb->s_fs_info = NULL;
}


//superblock operations, both of which are generic kernel ops
//qifs_ops
static struct super_operations qifs_ops = 
{     
//  .alloc_inode
//  .destory_inode  
	.drop_inode      = generic_delete_inode, //generic function
    .statfs          = simple_statfs,       //generic function
    .put_super       = qifs_put_super,       //need to define
};


//qifs_fill_super to fill a superblock

static int qifs_fill_super (struct super_block *sb, 
                               void *data, int silent)
{
	struct inode *root;
    struct qifs_sb_info *sbi;
	//basic parameters	
	sb->s_maxbytes          = MAX_LFS_FILESIZE;
    sb->s_blocksize 		= PAGE_CACHE_SIZE;
	sb->s_blocksize_bits 	= PAGE_CACHE_SHIFT;
	
	sb->s_magic 			= QIFS_MAGIC; //"magic number" to recognize superblocks 
	
	sb->s_op 				= &qifs_ops;  // need to define

    printk(KERN_INFO  "qifs: fill superblock\n");   //For debug

//	sb->s_flags             = MS_RDONLY;         

    sb->s_fs_info = kzalloc(sizeof(struct qifs_sb_info), GFP_KERNEL);
                              //kzalloc - allocate memory, set to zero
    if(!sb->s_fs_info)
    {

//      printk(KERN_INFO  "qifs: qifs_sb_info allocate error\n");   //For debug
        return -ENOMEM;
    }    
    printk(KERN_INFO  "qifs:sb->s _fs_info allocate good\n");   //For debug

    sbi = sb->s_fs_info; 

//conjure up an inode to represent the root directory of FS.
	root = qifs_get_inode(sb, S_IFDIR | 0755, 0);        // need to define
    if (!root)
    {
        printk(KERN_INFO  "qifs: get_inode error\n");   //For debug
        goto out;
    }

    printk(KERN_INFO "qifs: to alloc root inode \n");

//    root->i_op = & simple_dir_inode_operations;
    root->i_op  = & qifs_dir_inode_ops;
    root->i_fop = & simple_dir_operations;
    
    //Get a dentry to represent the directory   
    sb->s_root = d_alloc_root(root); 
    if(!sb->s_root)
    {
        iput(root);
        goto out;
    }

    printk(KERN_INFO "qifs: alloc root inode is good.  \n");

	return 0;
	
out:
    kfree(sbi);
    sb->s_fs_info = NULL;
    return -ENOMEM;  //out of memeory
}	

//qi_mount
static struct dentry *qi_mount(struct file_system_type *fs_type,
		int flags, const char *dev_name, void *data )
{
	printk(KERN_INFO  "qifs: begin to mount  \n");
    printk(KERN_INFO "qifs: fs_type: %p \n", fs_type );
    return mount_nodev(fs_type, flags, data, qifs_fill_super);  
	 								//mount_nodev is generic function
									//qifs_fill_super need to define
}


//define file_system_type, qi_fs_type
static struct file_system_type qi_fs_type =
{
//	.onwer     = THIS_MODULE,
    .name      = "qifs",	         // name to use at mount time		
	.mount     =  qi_mount,          // at mount time, to initializes and sets up a superblock
	.kill_sb   =  kill_block_super,  //generic function, at umount time to clean up
//    .fs_flags  =  FS_REQUIRES_DEV    //public flags for file_system_type  =1
};

//init module
static int __init init_qi_fs(void)
{
    printk(KERN_INFO "qifs: init qifs  \n");
    return register_filesystem(& qi_fs_type);     //qi_fs_type need to define
}

//exit module
static void __exit exit_qi_fs(void)
{
    printk(KERN_INFO "qifs: exit  qifs  \n");
    unregister_filesystem(& qi_fs_type);
}

module_init(init_qi_fs);
module_exit(exit_qi_fs);

