#include "regexcache.h"

std::unique_ptr<RegexCache> RegexCache::_instance = nullptr;
std::mutex RegexCache::_instance_locker;
