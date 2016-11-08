

#include "WisBadgeCache.h"



std::map<std::string, int>  WisBadgeCache::badgeMap_;
std::mutex           WisBadgeCache::badgeLock_;


int WisBadgeCache::getBadge( const std::string& token )
{
    std::lock_guard<std::mutex> g(badgeLock_);
    std::map<std::string,int>::iterator it = badgeMap_.find(token);
    if( it != badgeMap_.end() ) {
        return it->second;
    }

    return 1;
}

void WisBadgeCache::addBadge( const std::string& token, int num )
{
    std::lock_guard<std::mutex> g(badgeLock_);
    std::map<std::string,int>::iterator it = badgeMap_.find(token);
    if( it != badgeMap_.end() ) {
        it->second = it->second + num;
    }else {
        badgeMap_[token] = num;
    }
}

void WisBadgeCache::setBadge( const std::string& token, int num )
{
    std::lock_guard<std::mutex> g(badgeLock_);
    badgeMap_[token] = num;
}

void WisBadgeCache::delBadge( const std::string& token )
{
	std::lock_guard<std::mutex> g(badgeLock_);
    badgeMap_.erase( token );
}