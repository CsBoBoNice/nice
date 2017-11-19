#include<stdio.h>
#include<sys/types.h>
#include<regex.h>
#include<memory.h>
#include<stdlib.h>
#include<string.h>
#include<usar.h>
char buf[2*1024*1024];
char match_buf[10][2*1024*1024];

const char wind_name[8][64]=
{
	{"北风"},{"东北风"}, 
	{"东风"},{"东南风"},
	{"南风"},{"西南风"},
	{"西风"},{"西北风"},
};

const char weather_name[33][64]=
{
	{"晴"}, {"多云"}, {"阴"},
	{"阵雨"},{"雷阵雨"}, {"冰雹"},
	{"雨夹雪"}, {"小雨"},{"中雨"},
	{"大雨"},{"暴雨"},{"大暴雨"},
	{"特大暴雨"}, {"阵雪"}, {"小雪"},
	{"中雪"}, {"大雪"}, {"暴雪"},
	{"雾"},{"冻雨"} ,{"沙尘暴"},
	{"小到中雨"},{"中到大雨"},{"大到暴雨"},
	{"暴雨到大暴雨"}, {"大暴雨到特大暴雨"} ,{"小到中雪"},
	{"中到大雪"}, {"大到暴雪"},{"浮尘"},
	{"扬沙"},{"强沙尘暴"},{"霾"},
};

typedef struct weather_s
{
	int tod_weather;
	int tod_wind;
	int tod_pn;
	int tom_weather;
	int tom_wind;
	int tom_pn;
}s_weather;

s_weather s_weather_v;

char* http(void)
{
	system("curl http://tianqi.moji.com/ > /home/pi/code/nice/weather.text");
	return buf;
}
/*
char* http(void)
{
	FILE *fp=NULL;
	FILE *fp_text=NULL;
	fp = popen("curl http://tianqi.moji.com/", "r");
	if (NULL != fp)
	{
		fread(buf, 1024*1024, 1, fp);
		fp_text=fopen("/home/pi/code/nice/weather.text","w+");
		fprintf(fp_text,"%s",buf);
		pclose(fp);
		fclose(fp_text);
		return buf;
	}
	else 
	{
		printf("error:popen error\n");
	    //return 0;	
		//fprintf(stderr, "popen error...\n");
		//exit(1);
		//system("/home/pi/code/nice/bin/Nice_code");
		system("sudo reboot");
	}
	
	return 0;
}
*/
char* open_file()
{	 
	int len =0;
	FILE *file;
	file= fopen("/home/pi/code/nice/weather.text","r+");
	if(file==NULL)
	{
		return 0;
	}
	else
	{
		fseek(file,0,SEEK_END);
		len=ftell(file);
		fseek(file,0,SEEK_SET);
		fread(buf,1,len,file);
	}
	//printf("%s",buf);
	fclose(file);
	return buf;
}

int regular(char *bematch,char *pattern)
{
	int i;
	//char *bematch;//正则总数据数据
	char errbuf[1024];
	//char *pattern ="今天实况：(.*)。\"";//正则规则
	regex_t reg;
	int err,nm = 10;
	regmatch_t pmatch[nm];
	
	//bematch=http();
	//bematch=open_file();

	if(regcomp(&reg,pattern,REG_EXTENDED) < 0)
	{
		//regerror(err,&reg,errbuf,sizeof(errbuf));
		printf("err:%s\n",errbuf);
	}
	 
	err = regexec(&reg,bematch,nm,pmatch,0);

	if(err == REG_NOMATCH)
	{
		//printf("no match\n");
		//exit(-1);
		return 0;
	}
	else if(err)
	{
		regerror(err,&reg,errbuf,sizeof(errbuf));
		printf("err:%s\n",errbuf);
		//exit(-1);
		return 0;
	}

	for(i=0;i<10 && pmatch[i].rm_so!=-1;i++)
	{
		int len = pmatch[i].rm_eo-pmatch[i].rm_so;//计算出数据长度
		if(len)
		{
			//memset(match,'\0',sizeof(match));
			memcpy(&match_buf[i][0],bematch+pmatch[i].rm_so,len);
			//printf("************************************************\n");
			//printf("%s\n",&match[i]);
			//printf("len=%d\n",len);
		}
	}
	//printf("i=%d\n",i);
	
	return i; 
}

void memset_buf()
{
	memset(&match_buf[0],0,sizeof(match_buf));
}

void show_find()
{
	printf("%s\n",match_buf[0]);	
	printf("%s\n",match_buf[1]);	
}

int get_today_weater(char *all_buff)
{	
	int i;
	int fag; 
	int reg=0;
	int pn;
	char buff[1024];
	char weather[256];
	char wind[256];
	char pn25[256];
	char pattern[] =">今天</a>(.*)>明天</a>";//正则规则
	char weather_p[]="alt=\"(.*)\">.*</span>";
	char wind_p[]="<em>(.*)</em>";
	char pn25_p3[]="<strong class=\"level_[0-9]\">.*([0-9][0-9][0-9]).*</strong>"; 
	char pn25_p2[]="<strong class=\"level_[0-9]\">.*([0-9][0-9]).*</strong>"; 
	char pn25_p1[]="<strong class=\"level_[0-9]\">.*([0-9]).*</strong>"; 
	
	reg=regular(all_buff,pattern);//find matching
	if(reg==0){
		return 0;}
//	show_find();

	memcpy(buff,&match_buf[0][0],1024);	
	//printf("\n%s\n",buff);
	
	memset_buf();//clear buff

	reg=regular(buff,weather_p);//find matching
	if(reg==0){
		return 0;}

//	show_find();

	memcpy(weather,&match_buf[1][0],256);	
	//printf("\n%s\n",weather);
	
	memset_buf();//clear buff

	regular(buff,wind_p);//find matching
//	show_find();
	memcpy(wind,&match_buf[1][0],256);	
	//printf("\n%s\n",wind);

	memset_buf();//clear buff

	reg=regular(buff,pn25_p3);//find matching
	if(reg!=0)
	{
		//show_find();
		memcpy(pn25,&match_buf[1][0],256);	
		//printf("\n%s\n",pn25);
	}
	else if(reg==0)
	{
		reg=regular(buff,pn25_p2);//find matching
		if(reg!=0)
		{
			//show_find();
			memcpy(pn25,&match_buf[1][0],256);	
			//printf("\n%s\n",pn25);
		}
		else if(reg==0)
		{
			reg=regular(buff,pn25_p1);//find matching
			if(reg!=0)
			{
				//show_find();
				memcpy(pn25,&match_buf[1][0],256);	
				//printf("\n%s\n",pn25);
			}
			else if(reg==0)
			{
				return 0;
			}
		}
	}

	pn=atoi(pn25);
	
	//printf("pn=%d\n",pn);
	
	s_weather_v.tod_pn=pn;

	for(i=0;i<8;i++)
	{
		fag=strcmp(wind,&wind_name[i][0]);
		if(fag==0)
		{
			s_weather_v.tod_wind=i;
			//printf("%s=%d\n",&wind_name[i][0],i);
		}
	}
	for(i=0;i<33;i++)
	{
		fag=strcmp(weather,&weather_name[i][0]);
		if(fag==0)
		{
			s_weather_v.tod_weather=i;
			//printf("%s=%d\n",&weather_name[i][0],i);
		}
	}
	return 1;
}

int get_tomorrow_weater(char *all_buff)
{	
	int i;
	int fag; 
	int reg=0;
	int pn;
	char buff[1024];
	char weather[256];
	char wind[256];
	char pn25[256];
	char pattern[] =">明天</a>(.*)>后天</a>";//正则规则
	char weather_p[]="alt=\"(.*)\">.*</span>";
	char wind_p[]="<em>(.*)</em>";
	char pn25_p3[]="<strong class=\"level_[0-9]\">.*([0-9][0-9][0-9]).*</strong>"; 
	char pn25_p2[]="<strong class=\"level_[0-9]\">.*([0-9][0-9]).*</strong>"; 
	char pn25_p1[]="<strong class=\"level_[0-9]\">.*([0-9]).*</strong>"; 
	
	reg=regular(all_buff,pattern);//find matching
	if(reg==0){
		return 0;}

//	show_find();

	memcpy(buff,&match_buf[0][0],1024);	
	//printf("\n%s\n",buff);
	
	memset_buf();//clear buff

	reg=regular(buff,weather_p);//find matching
	if(reg==0){
		return 0;}

//	show_find();

	memcpy(weather,&match_buf[1][0],256);	
	//printf("\n%s\n",weather);
	
	memset_buf();//clear buff

	regular(buff,wind_p);//find matching
//	show_find();
	memcpy(wind,&match_buf[1][0],256);	
	//printf("\n%s\n",wind);

	memset_buf();//clear buff

	reg=regular(buff,pn25_p3);//find matching
	if(reg!=0)
	{
		//show_find();
		memcpy(pn25,&match_buf[1][0],256);	
		//printf("\n%s\n",pn25);
	}
	else if(reg==0)
	{
		reg=regular(buff,pn25_p2);//find matching
		if(reg!=0)
		{
			//show_find();
			memcpy(pn25,&match_buf[1][0],256);	
			//printf("\n%s\n",pn25);
		}
		else if(reg==0)
		{
			reg=regular(buff,pn25_p1);//find matching
			if(reg!=0)
			{
				//show_find();
				memcpy(pn25,&match_buf[1][0],256);	
				//printf("\n%s\n",pn25);
			}
			else if(reg==0)
			{
				return 0;
			}
		}
	}

	pn=atoi(pn25);
	
	//printf("pn=%d\n",pn);
	
	s_weather_v.tom_pn=pn;

	for(i=0;i<8;i++)
	{
		fag=strcmp(wind,&wind_name[i][0]);
		if(fag==0)
		{
			s_weather_v.tom_wind=i;
			//printf("%s=%d\n",&wind_name[i][0],i);
		}
	}
	for(i=0;i<33;i++)
	{
		fag=strcmp(weather,&weather_name[i][0]);
		if(fag==0)
		{
			s_weather_v.tom_weather=i;
			//printf("%s=%d\n",&weather_name[i][0],i);
		}
	}
	return 1;
}

void show_num()
{
	printf("*******************************************\n");
	printf("tod %s=%d\n",&weather_name[s_weather_v.tod_weather][0],s_weather_v.tod_weather);
	printf("tod %s=%d\n",&wind_name[s_weather_v.tod_wind][0],s_weather_v.tod_wind);
	printf("tod pn25=%d\n",s_weather_v.tod_pn);

	printf("*******************************************\n");
	printf("tom %s=%d\n",&weather_name[s_weather_v.tom_weather][0],s_weather_v.tom_weather);
	printf("tom %s=%d\n",&wind_name[s_weather_v.tom_wind][0],s_weather_v.tom_wind);
	printf("tom pn25=%d\n",s_weather_v.tom_pn);
	printf("*******************************************\n");
}

void init_wheather()
{
	s_weather_v.tod_weather=0;
	s_weather_v.tod_wind=0;
	s_weather_v.tod_pn=0;
	s_weather_v.tom_weather=0;
	s_weather_v.tom_wind=0;
	s_weather_v.tom_pn=0;

}

int get_time()
{
	FILE *fp=NULL;
	char buf[1024];
	int i=0;
	fp=popen("date | awk '{print $5}'","r");
	fread(buf, 32, 1, fp);

	printf("%s",buf);
	
	i=atoi(buf);	

	fclose(fp);	
	return i;
}

int main(void)
{
	int i=0;
	//int time=0;
	char *p_bematch;
	char head[2]={0xff,0xff};
	//char head1[2]={0xfe,0xfe};
	char dev_buf[] = "/dev/ttyAMA0";
	USAR usar;
	
	//usar.init_uart(dev_buf, 9600, 8, 1, 'N');
	
	sleep(20);
	while(1)
	{
		init_wheather();	
		usar.init_uart(dev_buf, 9600, 8, 1, 'N');
		//time=get_time();	
		if(i==0)
		{
			p_bematch=http();// www.xxx.com
		}
		else
		{
			p_bematch=open_file();
		}
		if(p_bematch!=0)
		{
			get_today_weater(p_bematch);
			get_tomorrow_weater(p_bematch);
			usar.uart_write(head, 2);
			usar.uart_write((char *)&s_weather_v, sizeof(s_weather_v));
			//usar.uart_write(head1, 2);
			//usar.uart_write((char *)&time, sizeof(time));	
			i++;
		}
	
		usar.close_uart();	
		//show_num();	
		sleep(3);
		if(i==2400)
		{
			i=0;
		}
	}
	return 0;

}
