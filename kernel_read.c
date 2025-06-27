static int read_file(char * filename, char *buf, int buffer_size)
{
	struct file *fp;
	loff_t pos = 0;
	int size = 0;

	fp = filp_open(filename, O_RDONLY, 0);
	if(IS_ERR(fp)){
		return -1;
	}
	
	size = kernel_read(fp, buf, buffer_size, &pos);
	//printk("===== size = %d read: %s ======/n", size, buf);

	filp_close(fp,NULL);
	
	return 0;
}


static int write_file(char * filename, char *buf, int buffer_size)
{
	struct file *fp;
	loff_t pos = 0;
	int size = 0;

	fp = filp_open(filename, O_RDONLY, 0);
	if(IS_ERR(fp)){
		return -1;
	}
	
	size = kernel_write(fp, buf, buffer_size, &pos);
	//printk("===== size = %d read: %s ======/n", size, buf);

	filp_close(fp,NULL);
	
	return 0;
}

