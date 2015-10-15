# Linux 下文件的读写模块
    Linu下内核文件的打开,读写,关闭的测试.
    关键函数:
```
struct file *fp;
mm_segment_t fs;
loff_t npos;
fp = filp_open(filename,O_RDWR|O_CREAT,0644);//打开文件
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
filp_close(fp,NULL);//关文件
