#include <string>
#include "cache/cache.hh"

using key_type = std::string;
using byte_type = char;

struct Request {
    key_type key_;
    Cache::val_type val_;
    std::string method_;
    Request(key_type key, Cache::val_type val, std::string method){
        key_ = key;
        val_ = val;
        method_ = method;
    }
};
