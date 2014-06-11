#include <linux/module.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/uaccess.h>

static ssize_t hello_read(struct file *fp, char __user *user, size_t size,
			loff_t *offs);

static ssize_t hello_write(struct file *fp, const char __user *user,
			size_t size, loff_t *offs);

#define MINOR_SIZE 3

static char minor[MINOR_SIZE] = "";


static const struct file_operations hello_fops = {
	.owner = THIS_MODULE,
	.read = hello_read,
	.write = hello_write
};

static struct miscdevice hello_misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "eudyptula",
	.fops = &hello_fops,
};

static int __init hello_init(void)
{
	int status = misc_register(&hello_misc);
	snprintf(minor, MINOR_SIZE, "%d", hello_misc.minor);
	return status;
}

module_init(hello_init);

static void __exit hello_exit(void)
{
	misc_deregister(&hello_misc);
}
module_exit(hello_exit);

ssize_t hello_read(struct file *fp, char __user *user, size_t size,
			loff_t *offs)
{
	return simple_read_from_buffer(user, size, offs, minor, strlen(minor));
}

ssize_t hello_write(struct file *fp, const char __user *user, size_t size,
			loff_t *offs)
{
	char tmp[MINOR_SIZE] = "";
	int result = 0;

	if (size == MINOR_SIZE) {
		result = simple_write_to_buffer(tmp, MINOR_SIZE, offs, user,
						size);
		tmp[MINOR_SIZE - 1] = '\0';
	} else
		result = -EINVAL;

	if ((*offs) == strlen(tmp))
		result = strncmp(tmp, minor, strlen(tmp)) ? -EINVAL : result;

	return result;
}

MODULE_LICENSE("GPL");
