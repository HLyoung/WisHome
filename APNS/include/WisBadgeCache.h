
#ifndef WIS_WISBADGECACHE_H
#define WIS_WISBADGECACHE_H

#include <string>
#include <map>
#include <mutex>

class WisBadgeCache {
public:
    static int getBadge( const std::string& token );
    static void addBadge( const std::string& token, int num );
    static void setBadge( const std::string& token, int num );
    static void delBadge( const std::string& token );
private:
    static std::map<std::string, int>  badgeMap_;
    static std::mutex     badgeLock_;
};


#endif //WIS_WISBADGECACHE_H
