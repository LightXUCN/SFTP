#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "tsc_sftp.h"

int main(int argc , char** argv)
{
	int ret = 0;
	int i = 0;
	void* handle;
	tsc_sftp_dir_info_t dir_info;
	//char* path = "/data/em1_001.pcap";
	struct timeval cur_time;
	struct timeval old_time;
	memset(&dir_info , 0 ,sizeof(tsc_sftp_dir_info_t));

	if(argc < 2)
		return -1;
	tsc_log_init(0xff,0,NULL,NULL, 10, 1);
	handle = tsc_sftp_client_create("root", "tescomm", "10.95.3.153", 22, NULL);
	if(!handle)
	{
		tsc_error("client create fail!");
		return -1;
	}
	tsc_info("client created!");
	
	ret = tsc_sftp_isdir(handle, argv[1]);
	if(ret == 1)
	{
		printf("%s is dir!\n",argv[1]);

		ret = tsc_sftp_readdir(handle, argv[1], &dir_info);
		if(ret < 0)
		{
			tsc_error("readdir fail!");		
			tsc_sftp_client_destory(handle);
			return -1;
		}
		for(i = 0;i<ret;i++)
		{
			tsc_info("file         : %s" , dir_info.info[i].filename);
			tsc_info("path         : %s" , dir_info.info[i].filepath);
			tsc_info("filesize     : %u" , dir_info.info[i].attr.filesize);
			tsc_info("permissions  : %u" , dir_info.info[i].attr.permissions);
			tsc_info("uid          : %u" , dir_info.info[i].attr.uid);
			tsc_info("gid          : %u" , dir_info.info[i].attr.gid);
			tsc_info("flags        : %u" , dir_info.info[i].attr.flags);
			
		}
	}
	else if(ret == 0)
	{
		printf("%s is file!\n",argv[1]);
		
		gettimeofday(&old_time, NULL);
		ret = tsc_sftp_download(handle, argv[1], "/home/xxz/123123.a");
		gettimeofday(&cur_time, NULL);
		
		if(ret <0)
			tsc_info("down load fail!");
		else
		{
			tsc_info("download file_size : %u\n",ret);
			tsc_info("download time      : %10u s\n",cur_time.tv_sec-old_time.tv_sec);
			tsc_info("download speed     : %10d k/s\n",ret/1024/(cur_time.tv_sec-old_time.tv_sec));
		}
		
		gettimeofday(&old_time, NULL);
		ret = tsc_sftp_upload(handle,"/home/xxz/787878.a", "/home/xxz/123123.a");
		gettimeofday(&cur_time, NULL);
		if(ret < 0)
			tsc_info("up load fail!");
		else
		{
			tsc_info("upload file_size : %u\n",ret);
			tsc_info("upload time      : %10u s\n",cur_time.tv_sec-old_time.tv_sec);
			tsc_info("upload speed     : %10d k/s\n",ret/1024/(cur_time.tv_sec-old_time.tv_sec));
		}
	}
	else
	{
		printf("%s open error!\n" , argv[1]);
	}


	tsc_sftp_client_destory(handle);
	
	return 0;
}

