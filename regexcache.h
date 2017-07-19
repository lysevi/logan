#pragma once

#include <QRegExp>
#include <QHash>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

class RegexCache
{
public:
    static std::unique_ptr<RegexCache> _instance;
    static std::mutex _instance_locker;

    static RegexCache *instance() {
        if (RegexCache::_instance == nullptr) {
            std::lock_guard<std::mutex> lg(_instance_locker);
            if (RegexCache::_instance == nullptr) {
                _instance = std::make_unique<RegexCache>();
            }
        }
        return _instance.get();
    }

    std::shared_ptr<QRegExp> compile(const QString &re) {
        std::lock_guard<std::mutex> lg(_locker);
        auto it = _cache.find(re);
        if (it == _cache.end()) {
            auto cre = std::make_shared<QRegExp>(re);
            _cache.insert(re, cre);
            return cre;
        }
        return *it;
    }

    QHash<QString, std::shared_ptr<QRegExp>> _cache;
    mutable std::mutex _locker;
};


