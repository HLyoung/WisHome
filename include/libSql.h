//初始化数据访问模块
extern "C" int DbaModule_Initialize(const char *pcHost,
												const char *pcDbname, 
												 const char *pcUsername, 
												 const char *pcPwd
												 );


//取得数据库访问对象
extern "C" void *DbaModule_GetNVDataAccess();

//释放数据库访问对象
extern "C" void DbaModule_ReleaseNVDataAccess(void * pNVDataAccess);

//退出
extern "C" int DbaModule_UnInitialize();
