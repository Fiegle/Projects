#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
MODULE_LICENSE("Dual BSD/GPL");

#define BUF_LEN 200

static struct proc_dir_entry* proc_entry;

static char msg[BUF_LEN];
static int procfs_buf_len;
struct timespec time1, time2, time3;

static ssize_t procfile_read(struct file* file, char * ubuf, size_t count, loff_t *ppos)
{
	printk(KERN_INFO "proc_timed\n");
	time1 = current_kernel_time();
	time3 = timespec_sub(time1,time2);
	if(time3.tv_sec == time1.tv_sec && time3.tv_nsec == time1.tv_nsec){
		time3.tv_sec = 0;
		time3.tv_nsec = 0;
		sprintf(msg, "current time: %d.%d\n",time1.tv_sec,time1.tv_nsec);

	}
	else{
        sprintf(msg, "current time: %d.%d\nelapsed time: %d.%d\n",time1.tv_sec,time1.tv_nsec,time3.tv_sec,time3.tv_nsec);
	}
        procfs_buf_len = strlen(msg);
	if (*ppos > 0 || count < procfs_buf_len){
		return 0;
	}

	if (copy_to_user(ubuf, msg, procfs_buf_len))
		return -EFAULT;

	*ppos = procfs_buf_len;
	printk(KERN_INFO "gave to user %s\n", msg);
	time2 = time1;
	return procfs_buf_len;
}


static ssize_t procfile_write(struct file* file, const char * ubuf, size_t count, loff_t* ppos)
{
	printk(KERN_INFO "proc_write\n");

	if (count > BUF_LEN)
		procfs_buf_len = BUF_LEN;
	else
		procfs_buf_len = count;

	copy_from_user(msg, ubuf, procfs_buf_len);

	printk(KERN_INFO "got from user: %s\n", msg);

	return procfs_buf_len;
}


static struct file_operations procfile_fops = {
	.owner = THIS_MODULE,
	.read = procfile_read,
	.write = procfile_write,
};

static int my_timer_init(void)
{
	proc_entry = proc_create("timed", 0666, NULL, &procfile_fops);

	if (proc_entry == NULL)
		return -ENOMEM;
	
	return 0;
}

static void my_timer_exit(void)
{
	proc_remove(proc_entry);
	return;
}



module_init(my_timer_init);
module_exit(my_timer_exit);
