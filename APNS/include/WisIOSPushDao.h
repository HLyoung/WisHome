
#ifndef WIS_WISIOSPUSHDAO_H
#define WIS_WISIOSPUSHDAO_H

#include <stdio.h>

#include "libSql.h"
#include "libLog.h"
#include "CNVDataAccess.h"


class WisIOSPushDao {
public:
    static bool save(const std::string& device, int flag, int len, const char* msg);
};

#endif //WIS_WISIOSPUSHDAO_H
