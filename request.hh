#include <string>

using key_type = std::string;
using byte_type = char;
using val_type = const byte_type*;

struct Request {
    key_type key_;
    val_type val_;
    std::string method_;
    Request(key_type key, val_type val, std::string method){
        key_ = key;
        val_ = val;
        method_ = method;
    }
};
