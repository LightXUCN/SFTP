#ifndef _SFTP_CLIENT_COMMON_H_2017_01_10_
#define _SFTP_CLIENT_COMMON_H_2017_01_10_


#define MAX_PATH_CNT 8
#define MAX_SUFFIX_CNT	8
enum STR_LEN{
	STR_LEN_16=16,
	STR_LEN_32=32,
	STR_LEN_64=64,
	STR_LEN_128=128,
	STR_LEN_256=256,
	STR_LEN_512=512,
	STR_LEN_1024=1024,
	STR_LEN_2048=2048,
	STR_LEN_8192=8192,
};
enum{
	DIRECTION_NONE=0,
	DIRECTION_UPLOAD,
	DIRECTION_DOWNLOAD,
};

enum{
	NO=0,
	YES
};

enum{
	CONFIRM_NONE=0,
	CONFIRM_AUXILIARY,
	CONFIRM_TEMP
};

enum log_mode
{
	LOG_MODE_TERMINAL=0,
	LOG_MODE_FILE,
};

typedef struct tsc_file_list
{
	char file_name[128];
	char confirm_name[128];
	char src_path[256];
	char dst_path[256];
	char confirm_path[256];
	tsc_list_t list;
}tsc_file_list_t;

typedef struct tsc_sftp_cfg
{
	uint8_t run;
	FILE*   log_fp;

	//上传下载配置项
	uint8_t 	direction;
	
	//后台运行配置项
	uint8_t		back_ground;
	
	//日志配置项
	uint16_t	log_mode;
	uint16_t	level_mask;
	uint32_t	log_file_size;
	uint32_t	log_file_num;
	char		log_dir[STR_LEN_128];
	char 		log_prefix[STR_LEN_64];
	//客户端目录
	uint8_t		client_path_cnt;
	char*		client_path[MAX_PATH_CNT];
	//服务端目录
	uint8_t		server_path_cnt;
	char*		server_path[MAX_PATH_CNT];
	
	//鉴权配置
	char		server_ip[STR_LEN_32];
	uint16_t	server_port;
	char		user_name[STR_LEN_64];
	char		password[STR_LEN_64];
	
	//上传配置
	uint8_t		upload_suffix_cnt;
	char*		upload_suffix[MAX_SUFFIX_CNT];
	uint8_t		upload_confirm;
	char		upload_confirm_suffix[STR_LEN_16];
	uint8_t		upload_complete;
	char		upload_complete_suffix[STR_LEN_16];
	uint8_t		upload_remove;
	char		upload_complete_dir[STR_LEN_64];

	//下载配置
	uint8_t		download_suffix_cnt;
	char*		download_suffix[MAX_SUFFIX_CNT];
	uint8_t		download_confirm;
	char		download_confirm_suffix[STR_LEN_16];
	uint8_t		download_complete;
	char		download_complete_suffix[STR_LEN_16];
	uint8_t		download_remove;
	char		download_complete_dir[STR_LEN_64];

}tsc_sftp_cfg_t;

#endif   /* ifndef _SFTP_CLIENT_COMMON_H_2017_01_10_*/

