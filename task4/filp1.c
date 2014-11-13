#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/gfp.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/timer.h>
unsigned char *buff;
static char* devname;
struct timeval tv;
long start_time,end_time;
module_param(devname,charp,0644);//用户向内核传参数的函数

static long my_function(void){//计算时间
	printk("Hello World!\n");
	do_gettimeofday(&tv);
	return (long)tv.tv_sec;	
}
static int print(char *filename,char *buff){
	struct file* fp;
	mm_segment_t fs;
	long count = 0;
	loff_t pos;
	fp = filp_open(filename,O_RDWR|O_CREAT,0644);//打开文件
	if(IS_ERR(fp)){
		printk("open fail");
		return -1;
	}
	fs = get_fs();
	set_fs(KERNEL_DS);//获得内核态
	pos = 0;
	for(count = 0;count < 32*1024; count ++)
		fp->f_op->write(fp,buff,strlen(buff),&fp->f_pos);//写入文件
		printk("<1>buffer is %s\n",buff);
	filp_close(fp,NULL);//关文件
	set_fs(fs);//恢复用户态
	return 0;
}
static int __init
init_file(void)
{
	long minus,avg=0,mul=0;	
	buff = (unsigned char* )kmalloc(64*1024,GFP_KERNEL);//分配每次的64kb大小
	if(buff != NULL)
	{
		printk("allocate 64kb address space %p\n",buff);
	}
	else
		printk("error\n");
	memset(buff,'A',1024*64);//填充buff区域
	start_time = my_function();
	print(devname,buff);
	end_time = my_function();
	minus = end_time - start_time;//时间差
	mul = 2*1024;
	avg = mul/minus;	
	printk("minus :%ld\n",minus);
	printk("The rate is %ld M/s\n",avg);//计算得速率		
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

