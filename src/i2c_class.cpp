

#include <vector>
#include <list>

#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <sys/stat.h> 
#include <fcntl.h>

#include "log.h"
#include "i2c_class.hpp"


#if __GNUC_PREREQ(3,0) && !defined(__NO_INLINE__)
# define ALWAYS_INLINE __attribute__ ((always_inline)) inline
/* I've seen a toolchain where I needed __noinline__ instead of noinline */
# define NOINLINE      __attribute__((__noinline__))
# if !ENABLE_WERROR
#  define DEPRECATED __attribute__ ((__deprecated__))
#  define UNUSED_PARAM_RESULT __attribute__ ((warn_unused_result))
# else
#  define DEPRECATED
#  define UNUSED_PARAM_RESULT
# endif
#else
# define ALWAYS_INLINE inline
# define NOINLINE
# define DEPRECATED
# define UNUSED_PARAM_RESULT
#endif



#define I2CDUMP_NUM_REGS		256

#define I2CDETECT_MODE_AUTO		0
#define I2CDETECT_MODE_QUICK	1
#define I2CDETECT_MODE_READ		2

/* linux/i2c-dev.h from i2c-tools overwrites the one from linux uapi
 * and defines symbols already defined by linux/i2c.h.
 * Also, it defines a bunch of static inlines which we would rather NOT
 * inline. What a mess.
 * We need only these definitions from linux/i2c-dev.h:
 */
#define I2C_SLAVE			0x0703
#define I2C_SLAVE_FORCE		0x0706
#define I2C_FUNCS			0x0705
#define I2C_PEC				0x0708
#define I2C_SMBUS			0x0720
struct i2c_smbus_ioctl_data {
	__u8 read_write;
	__u8 command;
	__u32 size;
	union i2c_smbus_data *data;
};
/* end linux/i2c-dev.h */




static ALWAYS_INLINE void *itoptr(int i)
{
	return (void*)(intptr_t)i;
}




i2cscanner::i2cscanner()
{
	dev.reserve(75);
	count = 0;
}


std::shared_ptr<i2cscanner> i2cscanner::create()
{
	return std::shared_ptr<i2cscanner>(new i2cscanner());
}


/*
 * Opens the device file associated with given i2c bus.
 *
 * Upstream i2c-tools also support opening devices by i2c bus name
 * but we drop it here for size reduction.
 */
static int i2c_dev_open(int i2cbus)
{
	char filename[sizeof("/dev/i2c-%d") + sizeof(int)*3];
	int fd;

	sprintf(filename, "/dev/i2c-%d", i2cbus);
	fd = open(filename, O_RDWR);
	if (fd < 0) {
		if (errno == ENOENT) {
			filename[8] = '/'; /* change to "/dev/i2c/%d" */
			// fd = xopen(filename, O_RDWR);
			fd = open(filename, O_RDWR);
		} else {
			// bb_perror_msg_and_die("can't open '%s'", filename);
	        SPDLOG_LOGGER_CRITICAL(my_logger, "can't open '%s'", filename);
		}
	}

	return fd;
}


static int32_t i2c_smbus_access(int fd, char read_write, uint8_t cmd,
				int size, union i2c_smbus_data *data)
{
	struct i2c_smbus_ioctl_data args;

	args.read_write = read_write;
	args.command = cmd;
	args.size = size;
	args.data = data;

	return ioctl(fd, I2C_SMBUS, &args);
}


static int32_t i2c_smbus_read_byte(int fd)
{
	union i2c_smbus_data data;
	int err;

	err = i2c_smbus_access(fd, I2C_SMBUS_READ, 0, I2C_SMBUS_BYTE, &data);
	if (err < 0)
		return err;

	return data.byte;
}

static void get_funcs_matrix(int fd, unsigned long *funcs)
{
	ioctl(fd, I2C_FUNCS, funcs);
}



std::list<devdata> i2cscanner::scan(int bus_num)
{
	std::list<devdata> d;
	// for(int i=0; i<5; i++)
	// {
	// 	d[i].addr = i*7+1;
	// 	d[i].state = i+3;
	// }
	int status;
	int fd;
	unsigned long funcs;


	fd = i2c_dev_open(bus_num);
	get_funcs_matrix(fd, &funcs);


	for(int i=3; i<0x78; i++)
	{
		status = ioctl(fd, I2C_SLAVE, itoptr(i /*i + j*/));
		if (status < 0) 
		{
			if (errno == EBUSY) 
			{
				d.emplace_back(static_cast<unsigned char>(i & 0xff), static_cast<unsigned char>(DEV_BUSY));
				continue;
			}
			else
			{ // В i2c_tools в этом месте выход из программы с ошибкой
				d.emplace_back(i, DEV_ERROR);
				continue;
			}
		}
		status = i2c_smbus_read_byte(fd);
		if (status < 0)
			continue;
			// d.emplace_back(i, DEV_ABSENT);
		else
			d.emplace_back(i, DEV_PRESENT);
	}

	return d;
}




std::string devdata::state_str() const
{
	switch(state)
	{
	case DEV_NON:
		return std::string("Not scanned address");
	case DEV_ABSENT:
		return std::string("Device don't answer");
	case DEV_PRESENT:
		return std::string("Device is on the bus");
	case DEV_BUSY:
		return std::string("A driver is connected to the device");
	case DEV_ERROR:
		return std::string("Error while device scanning");
	}
	return std::string("Error state");
}
