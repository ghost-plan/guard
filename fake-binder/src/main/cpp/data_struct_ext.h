//
// Created by jamesfchen on 2022/3/27.
//

#ifndef ATTACK_ON_TITAN_DATA_STRUCT_EXT_H
#define ATTACK_ON_TITAN_DATA_STRUCT_EXT_H

#include <stddef.h>
#include <cstdint>
#include <cstdio>
namespace jamesfchen {
#ifdef _WIN32
typedef int status_t;
#else
typedef int32_t status_t;
#endif

class String16 {

};

class String8 {
 public:
  String8();

//        explicit                    String8(StaticLinkage);
  String8(const String8 &o);

  explicit String8(const char *o);

  explicit String8(const char *o, size_t numChars);

  explicit String8(const String16 &o);

  explicit String8(const char16_t *o);

  explicit String8(const char16_t *o, size_t numChars);

  explicit String8(const char32_t *o);

  explicit String8(const char32_t *o, size_t numChars);

  ~String8();

  const char *string() const;
};

}
#endif //ATTACK_ON_TITAN_DATA_STRUCT_EXT_H
