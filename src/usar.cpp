#include "usar.h"

const int speed_arr[] = { B38400, B19200, B4800, B2400, B1200, B300,
	B38400, B19200, B4800, B2400, B1200, B300, };
const int name_arr[] = { 38400, 19200, 4800, 2400, 1200, 300,
	38400, 19200, 4800, 2400, 1200, 300, };

USAR::USAR()
{
}

USAR::~USAR()
{
}

void USAR::close_uart()
{
	close(fp);
}
/*
int USAR::uart_read(char *data, int data_len)
{
	size_t nleft;
	ssize_t nread;
	char *ptr;

	fcntl(fp,F_SETFL,0);
	ptr=data;
	nleft=data_len;

	while(nleft > 0)
	{
		if((nread = read(fp,ptr,nleft)) < 0)
		{
	         if(errno == EINTR)//被信号中断
		         nread = 0;
			 else
				return -1;
		}
		else
			if(nread == 0)
				break;
			nleft -= nread;
			ptr += nread;
	}
	return (data_len-nleft);
}
*/

int USAR::uart_read(char *data, int data_len)
{
	int len, fs_sel;
	fd_set fs_read;

	struct timeval time;

	FD_ZERO(&fs_read);
	FD_SET(fp, &fs_read);

	time.tv_sec = 10;
	time.tv_usec = 0;

	fs_sel = select(fp + 1, &fs_read, NULL, NULL, &time);
	if (fs_sel)
	{
		len = read(fp, data, data_len);
		return len;
	}
	else
	{
		return FALSE;
	}
}

int USAR::uart_write(char *buff, int size)
{
	lseek(fp,0,SEEK_SET);
	if (write(fp, buff, size)<0)
	{
		//perror("write");
		//exit(EXIT_FAILURE);
	}
	return size;
}

int USAR::init_uart(char *tty_name, int speed, int databits, int stopbits, int parity)
{
	/*int fd;*/
/*
	char chmod_buf[1024] = "sudo chmod 777 ";
	strcat(chmod_buf, tty_name);
	system(chmod_buf);
*/

	chmod(tty_name,0777);

	if ((fp = open(tty_name, O_RDWR)) == -1)
	{
		//perror("open");
		printf("error:open\n");
		//exit(EXIT_FAILURE);
	}

	set_speed(fp, speed);

	if (set_Parity(fp, databits, stopbits, parity) == FALSE)
	{
		printf("error:set_Parity\n");
		//exit(EXIT_FAILURE);
	}

	//close(fd);
	return fp;
}

void USAR::set_speed(int fd, int speed)
{
	unsigned int i;
	int status;
	struct termios Opt;
	tcgetattr(fd, &Opt);
	for (i = 0; i<sizeof(speed_arr) / sizeof(int); i++)
	{
		if (speed == name_arr[i])
		{
			tcflush(fd, TCIOFLUSH);
			cfsetispeed(&Opt, speed_arr[i]);
			cfsetospeed(&Opt, speed_arr[i]);
			status = tcsetattr(fd, TCSANOW, &Opt);
			if (status != 0)
				//perror("tcsetattr fd1");
			return;

		}

	}
	tcflush(fd, TCIOFLUSH);
}

int USAR::set_Parity(int fd, int databits, int stopbits, int parity)
{
	struct termios options;
	if (tcgetattr(fd, &options) != 0)
	{
		perror("SetupSerial 1");
		return(FALSE);
	}
	options.c_cflag &= ~CSIZE;
	switch (databits)
	{
	case 7:
		options.c_cflag |= CS7;
		break;
	case 8:
		options.c_cflag |= CS8;
		break;
	default:
		fprintf(stderr, "Unsupported data size\n");
		return(FALSE);
	}

	switch (parity)
	{
	case 'n':
	case 'N':
		options.c_cflag &= ~PARENB;
		options.c_iflag &= ~INPCK;
		break;
	case 'o':
	case 'O':
		options.c_cflag |= (PARODD | PARENB);
		options.c_iflag |= INPCK;
		break;
	case 'e':
	case 'E':
		options.c_cflag |= PARENB;
		options.c_cflag &= ~PARODD;
		options.c_iflag |= INPCK;
		break;
	case 's':
	case 'S':
		options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;
		break;
	default:
		fprintf(stderr, "Unsupported parity\n");
		return (FALSE);
	}

	switch (stopbits)
	{
	case 1:
		options.c_cflag &= ~CSTOPB;
		break;
	case 2:
		options.c_cflag |= CSTOPB;
		break;
	default:
		fprintf(stderr, "Unsupported stop bits\n");
		return(FALSE);
	}

	if (parity != 'n')
		options.c_iflag |= INPCK;
	options.c_cc[VTIME] = 150;
	options.c_cc[VMIN] = 0;
	tcflush(fd, TCIFLUSH);
	if (tcsetattr(fd, TCSANOW, &options) != 0)
	{
		perror("SetupSerial 3");
		return (FALSE);
	}
	return (TRUE);

}
