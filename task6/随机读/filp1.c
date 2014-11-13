#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/gfp.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <asm/div64.h>
#include <linux/random.h>
unsigned char *buff;
static char* devname;
struct timeval tv;
struct timeval start_time,end_time;
module_param(devname,charp,0644);//用户向内核传参数的函数

struct timeval my_function(void){//计算时间
	printk("Hello World!\n");
	do_gettimeofday(&tv);
	return tv;	
}
static int fileread(const char *filename)
{	
	struct file *fp;
	struct inode *inode;
	mm_segment_t fs;
	loff_t npos;
	off_t fsize;
	unsigned long magic;
	long count=0;
	printk("<1>start...\n");
	fp = filp_open(filename,O_RDONLY,0);//打开文件
	inode = fp->f_dentry->d_inode;
	magic = inode->i_sb->s_magic;
	printk("<1>file system magic:%li\n",magic);
	printk("<1>superblocksize:%li\n",inode->i_sb->s_blocksize);
	printk("<1>inode%li\n",inode->i_ino);
	fsize = inode->i_size;//得到文件的大小
	printk("<1>file size:%i\n",(int)fsize);
	buff = (char *)kmalloc(64*1024,GFP_ATOMIC);//分配block_size倍数的空间
	fs = get_fs();
	set_fs(KERNEL_DS);//获得内核态
	for(count = 0,npos = 0;count < 32*1024;count++){
		while(1){			
			get_random_bytes(&npos,sizeof(npos));//得到随机的偏移指针npos
			if(npos<0) npos=npos*(-1);
			if(npos>fsize)  npos=npos%(fsize);//不能超出文件大小的最大值
			break;
			}		
			vfs_read(fp,buff,64*1024,&npos);//读取文件
	}
	set_fs(fs);//恢复用户态
	buff[64*1024] = '\0';
	printk("<1>The file content si:\n");
	printk("<1>%s",buff);	
	filp_close(fp,NULL);//关文件
	return 0;
}	
static int __init
init_file(void)
{
	long minus=0,mul=0;	
	start_time = my_function();
	fileread(devname);
	end_time = my_function();
	if(start_time.tv_sec == end_time.tv_sec)//判断超出秒数
		minus = (long)(end_time.tv_usec - start_time.tv_usec);
	else{
		minus = (long)((end_time.tv_sec - start_time.tv_sec)*1000000 + end_time.tv_usec - start_time.tv_usec);		
	}		
	mul = 2*1024*1024;
	do_div(mul,(minus/1000));
	printk("The rate is %ld Kb/ms,minus %ld\n",mul,minus);	//计算得到速率	
	return 0;
}
static void __exit
exit_file(void)
{
	kfree(buff);//释放buff空间
	printk("GoodBye kernel\n");
}
module_init(init_file);
module_exit(exit_file);
MODULE_LICENSE("GPL");

