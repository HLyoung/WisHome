//
// Created by derry on 15-11-28.
//

#ifndef WIS_WISLOGMANAGER_H
#define WIS_WISLOGMANAGER_H

#include <string>
#include <stdio.h>
#include "libLog.h"
#include "libSql.h"
#include "CNVDataAccess.h"


class WisLogDao {
public:
    enum LogSource {
        LOG_SRC_USER    = 1,        // 用户日志
        LOG_SRC_DEVICE  = 2,        // 设备日志
        LOG_SRC_SERVER  = 3,        // 服务日志
    };
    enum LogUserType {
        LOG_TYPE_USER_LOGIN_WEB = 0,    // 用户登录WEB页面
        LOG_TYPE_USER_REG       = 1,    // 用户注册
        LOG_TYPE_USER_LOGIN     = 2,    // 用户登录
        LOG_TYPE_USER_LOGOUT    = 3,    // 用户登出
        LOG_TYPE_USER_BIND      = 4,    // 用户关联设备
        LOG_TYPE_USER_UNBIND    = 5,    // 用户控制设备
        LOG_TYPE_USER_CTRL      = 6,    // 用户修改信息
        LOG_TYPE_USER_ACCOUNT   = 7,
    };
    enum LogDeviceType {
        LOG_TYPE_DEV_REG        = 1,    // 注册并登录
        LOG_TYPE_DEV_LOGIN      = 2,    // 连接服务器
        LOG_TYPE_DEV_LOGOUT     = 3,    // 断开连接
        LOG_TYPE_DEV_REBOOT     = 4,    // 设备重启
        LOG_TYPE_DEV_POWEROFF   = 5     // 设备关机
    };
    enum LogServerType {
        LOG_TYPE_SRV_START      = 0,    // 服务重启
        LOG_TYPE_SRV_REG        = 1,    // 服务注册
        LOG_TYPE_SRV_UPDATE     = 2     // 服务更新
    };

    static void saveUserLog(int logType, const std::string& srcId, int argLen, const char* arg){
        saveLog( LOG_SRC_USER, logType, srcId, argLen, arg );
    }
    static void saveDeviceLog(int logType, const std::string& srcId, int argLen, const char* arg){
        saveLog( LOG_SRC_DEVICE, logType, srcId, argLen, arg );
    }
    static void saveServerLog(int logType, const std::string& srcId, int argLen, const char* arg) {
        saveLog( LOG_SRC_SERVER, logType, srcId, argLen, arg );
    }

    // User
    static void saveUserLoginWebLog(const std::string& srcId, int argLen, const char* arg) {
        saveUserLog(LOG_TYPE_USER_LOGIN_WEB, srcId, argLen, arg);
    }
    static void saveUserRegLog(const std::string& srcId, int argLen, const char* arg) {
        saveUserLog(LOG_TYPE_USER_REG, srcId, argLen, arg);
    }
    static void saveUserLoginLog(const std::string& uuid, int addrLen, const char* addr) {
        saveUserLog(LOG_TYPE_USER_LOGIN, uuid, addrLen, addr);
    }
    static void saveUserLogoutLog(const std::string& srcId, int argLen, const char* arg) {
        saveUserLog(LOG_TYPE_USER_LOGOUT, srcId, argLen, arg);
    }
    static void saveUserBindLog(const std::string& srcId, int argLen, const char* arg) {
        saveUserLog(LOG_TYPE_USER_BIND, srcId, argLen, arg);
    }
    static void saveUserUnBindLog(const std::string& srcId, int argLen, const char* arg) {
        saveUserLog(LOG_TYPE_USER_UNBIND, srcId, argLen, arg);
    }
    static void saveUserCtrlLog(const std::string& srcId, int argLen, const char* arg) {
        saveUserLog(LOG_TYPE_USER_CTRL, srcId, argLen, arg);
    }
    static void saveUserAccountLog(const std::string& srcId, int argLen, const char* arg) {
        saveUserLog(LOG_TYPE_USER_ACCOUNT, srcId, argLen, arg);
    }

    // device
    static void saveDeviceRegLog(const std::string& srcId, int argLen, const char* arg) {
        saveDeviceLog(LOG_TYPE_DEV_REG, srcId, argLen, arg);
    }
    static void saveDeviceLoginLog(const std::string& srcId, int argLen, const char* arg) {
        saveDeviceLog(LOG_TYPE_DEV_LOGIN, srcId, argLen, arg);
    }
    static void saveDeviceLogoutLog(const std::string& srcId, int argLen, const char* arg) {
        saveDeviceLog(LOG_TYPE_DEV_LOGOUT, srcId, argLen, arg);
    }
    static void saveDeviceRebootLog(const std::string& srcId, int argLen, const char* arg) {
        saveDeviceLog(LOG_TYPE_DEV_REBOOT, srcId, argLen, arg);
    }
    static void saveDevicePowerOffLog(const std::string& srcId, int argLen, const char* arg) {
        saveDeviceLog(LOG_TYPE_DEV_POWEROFF, srcId, argLen, arg);
    }

    // server
    static void saveServerStartLog(const std::string& srcId, int argLen, const char* arg) {
        saveServerLog(LOG_TYPE_SRV_START, srcId, argLen, arg );
    }
    static void saveServerRegLog(const std::string& srcId, int argLen, const char* arg) {
        saveServerLog(LOG_TYPE_SRV_REG, srcId, argLen, arg );
    }
    static void saveServerUpdateLog(const std::string& srcId, int argLen, const char* arg) {
        saveServerLog(LOG_TYPE_SRV_UPDATE, srcId, argLen, arg );
    }

    static void saveLog(int src, int logType, const std::string& srcId, int argLen, const char* arg);
};


#endif //WIS_WISLOGMANAGER_H
