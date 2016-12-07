/*
 * wis.h  (c) 2013 wiimu.com liaods <dongsheng.liao@gmail.com>
 */

#ifndef __WIS_H__
#define __WIS_H__

#include <stdio.h>
#include <time.h>
#include <string.h>

#include <string>

#include <macro.h>
#define MAX_WIS_DATA_LEN (1048576)
#define UUID_LEN 33   //32 + 1
#define TOKEN_LEN 72
#define PASSWORD_LEN 25 // 24 + 1
#define DEVICE_NAME_LEN 32


// 0x8000 - 0x81ff is reserved for wis_server
#define WIS_CMD(x)        (0x8000 +(x))


enum WisCommandID {
    WIS_CMD_UNKNOWN         = -1,
    WIS_CMD_MIN             = WIS_CMD( 0x0 ),

    WIS_CMD_HEART_BEAT      = WIS_CMD( 0x0 ),
    WIS_CMD_LOGIN           = WIS_CMD( 0x1 ),
    WIS_CMD_POWEROFF        = WIS_CMD( 0x2 ),
    WIS_CMD_REBOOT          = WIS_CMD( 0x3 ),
    WIS_CMD_WEBSIGNAL       = WIS_CMD( 0x4 ),
    WIS_CMD_RESULT          = WIS_CMD( 0x5 ),
    WIS_CMD_TO_USER         = WIS_CMD( 0x6 ),

	
    
    /* update result content if exists */
    WIS_CMD_RESULT2         = WIS_CMD( 0x7 ),
    WIS_CMD_GET_USER_DATA   = WIS_CMD( 0x8 ),
    WIS_CMD_SET_USER_DATA   = WIS_CMD( 0x9 ),
    WIS_CMD_LOCK            = WIS_CMD( 0xa ),
    WIS_CMD_UNLOCK          = WIS_CMD( 0xb ),
    WIS_CMD_GET_LICENSE     = WIS_CMD( 0xc ),
    WIS_CMD_SAVE_LOG        = WIS_CMD( 0xd ),

    /* commands for device & wis_client */
    WIS_CMD_DEV_INFO = WIS_CMD( 0x100 ),

    /* commands for Android/PC/IOS */
    WIS_CMD_USER_HEART_BEAT     = WIS_CMD(0x80),
    WIS_CMD_USER_PRIVATE        = WIS_CMD(0x81),
    WIS_CMD_USER_AUTO_LOGIN     = WIS_CMD(0x82),
    WIS_CMD_USER_BIND           = WIS_CMD(0x83),
    WIS_CMD_USER_UNBIND         = WIS_CMD(0x84),
    WIS_CMD_USER_GET_DEVICES    = WIS_CMD(0X85),
    WIS_CMD_USER_GET_DEVICE_INFO= WIS_CMD(0x86),
    WIS_CMD_USER_LOGIN          = WIS_CMD(0x87),
    WIS_CMD_USER_REGISTER       = WIS_CMD(0x88),
    WIS_CMD_USER_DEV_LOGIN      = WIS_CMD(0x89),
    WIS_CMD_USER_DEV_LOGOUT     = WIS_CMD(0x90),

	WIS_CMD_USER_REGIST         = WIS_CMD(0x91),
	WIS_CMD_USER_MODIFY_PASSWORD = WIS_CMD(0x92),
	WIS_CMD_USER_RESET_PASSWORD = WIS_CMD(0x94),
	WIS_CMD_SERVICE_KICKOUT_USER= WIS_CMD(0x95),
	WIS_CMD_USER_QUIT           = WIS_CMD(0x96),

   
    /* SHOWHOME */
    WIS_CMD_LIST				= 0x2000,
    WIS_CMD_SWITCH				= 0x2001,
    WIS_CMD_THERM_SET			= 0x2004,
    WIS_CMD_THERM_GET			= 0x2005,
    WIS_CMD_STATE				= 0x2006,
    WIS_CMD_CAM					= 0x3000,
    WIS_CMD_URL					= 0x7000,
    /* Max */
    WIS_CMD_MAX                 = WIS_CMD( 0x1ff ),
} ;

extern std::string WIS_CMD_TO_STR( unsigned int cmdId );

#define CMD_FLAG_USER_ONE 			0
#define CMD_FLAG_USER_ALL 			1
#define CMD_FLAG_USER_IOS_PUSH 		2    // BIT1 0: send to devices directly, 1 send from apple push server
#define CMD_FLAG_USER_IOS_OFFLINE 	4 	 // BIT2 0: only send to online devices, 1 send to offline devices
#define CMD_FLAG_ISO_APNS_ALL_OFFLINE (CMD_FLAG_USER_IOS_OFFLINE | CMD_FLAG_USER_IOS_PUSH|CMD_FLAG_USER_ALL)

#define WIS_CMD_VAL_MASK  0x0000ffff
#define WIS_CMD_FLAG_MASK 0xffff0000
#define WIS_CMD_VAL(x)    (((x)&WIS_CMD_VAL_MASK))
#define WIS_CMD_FLAG(x)   (((x)&WIS_CMD_FLAG_MASK))


struct WisSocketPacket {
    unsigned int  checksum;
    unsigned int  cmdId;
    unsigned int  cmdDataLength;
    unsigned char cmdData[];
};

struct WisDeviceLoginInfo {
    char uuid[32];
    char name[32];
    char data[128];
};


struct WisUserLoginInfo {
    char uuid[UUID_LEN];
	char password[PASSWORD_LEN];
    char token[TOKEN_LEN];
};

struct WisUserQuitInfo{
	char uuid[UUID_LEN];
	char token[TOKEN_LEN];
};

struct WisUserRegistInfo{
	char uuid[UUID_LEN];
	char password[PASSWORD_LEN];
};

struct WisUserGetVeriCode{
	char uuid[UUID_LEN];
};

struct WisUserResetPassword{
	char uuid[UUID_LEN];
	char newPassword[PASSWORD_LEN];
};
struct WisUserModifyPassword{
	char newPassword[PASSWORD_LEN];
};

struct WisUserBindMail{
	char uuid[UUID_LEN];
	int virificationCode;
};
struct WisUserDeviceInfo {
    char uuid[32];
    char name[32];
    char bind_time[20];
    int  status;
};

struct WisUserDeviceList {
    int nums;
    WisUserDeviceInfo devs[0];
};

struct WisResult {
    char uuid[32];
    char data[0];
};

std::string WIS_CMD_TO_STR( UINT32 cmdId );
const int nMaxPkgLen = 4096;
typedef struct{
	int result;
	char uuid[32];
}logResult,*plogResult;




enum {TYPE_DEVICE=1,TYPE_USER=2};

#endif

