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
long count = 0;
static char* devname;
struct timeval tv;
struct timeval start_time,end_time;
module_param(devname,charp,0644);//用户向内核传参数的函数
int num[32*1024];
struct timeval my_function(void){//计算时间
	printk("Hello World!\n");
	do_gettimeofday(&tv);
	return tv;	
}
static void junk(int count,loff_t pos)//将得到的pos放到数组里
{
	num[count] = pos;
}
static int judge(loff_t ppos){//查找是否已存在pos值
        int j = 0,flag = 1;
        for(;j <= count;j++){
                if((loff_t)num[j] == ppos){
                        flag = 0;
                        break;
                 }
        }
        return flag;
}
static int fileread(const char *filename,char *buff)
{	
	struct file *fp;
	mm_segment_t fs;
	loff_t npos;
	int flag;
	printk("<1>start...\n");
	fp = filp_open(filename,O_RDWR|O_CREAT,0644);//打开文件
	if(IS_ERR(fp)){
		printk("open fail");
		return -1;
	}
	fs = get_fs();
	set_fs(KERNEL_DS);//获得内核态
	npos = 0;
	junk(0,npos);
	for(count = 1;count < 32*1024;count++){
		npos = npos*64*1024;
		vfs_write(fp,buff,64*1024,&npos);//写入文件
		get_random_bytes(&npos,sizeof(npos));//得到随机的偏移指针npos							
		npos = npos%(32*1024);//不能超出分配的块数
		junk(count,npos);
		flag = judge(npos);
		while(flag == 0){
	             get_random_bytes(&npos, sizeof(npos));
	             npos = npos%(32*1024);
	             flag = judge(npos);  
	        }
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
	buff = (unsigned char* )kmalloc(64*1024,GFP_KERNEL);//分配每次的64kb大小
	if(buff != NULL)
	{
		printk("allocate 64kb address space %p\n",buff);
	}
	else
		printk("error\n");
	memset(buff,'A',1024*64);//填充buff区域
	start_time = my_function();
	fileread(devname,buff);
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

