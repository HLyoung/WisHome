//
// Created by derry on 15-12-1.
//

#ifndef WIS_WISIOSTOKENMANAGER_H
#define WIS_WISIOSTOKENMANAGER_H

#include <libSql.h>
#include <vector>
#include <string>
#include <set>

#include "libLog.h"
#include "libSql.h"
#include "CNVDataAccess.h"


class WisIOSTokenDao {
public:
    static int save( const std::string& uuid, const std::string& token );
    static std::string getToken( const std::string& uuid );
    static int getTokens( std::vector<std::string>& uuids, std::set<std::string>& tokens );
private:

};


#endif //WIS_WISIOSTOKENMANAGER_H
