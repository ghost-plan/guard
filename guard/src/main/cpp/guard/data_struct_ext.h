//
// Created by hawks.jamesf on 2/12/21.
//

#ifndef ATTACK_ON_TITAN_DATA_STRUCT_EXT_H
#define ATTACK_ON_TITAN_DATA_STRUCT_EXT_H

#include <sys/types.h>
#include <string.h>
#include <map>
//[预处理宏](https://zh-google-styleguide.readthedocs.io/en/latest/google-cpp-styleguide/others/#preprocessor-macros)
/**
 *
 *  java    | jni     | 内存分配
 *  |---|---|---|---|
 *  boolean(1bit) | jboolean| 1byte  |无符号8位整型 uint8_t
 *  byte    | jbyte   | 1byte  |有符号8位整型 int8_t
 *  char    | jchar   | 2bytes |无符号16位整型 uint16_t
 *  short   | jshort  | 2bytes |有符号8位整型 int16_t
 *  int     | jint    | 4bytes |有符号32位整型 int32_t
 *  long    | jlong   | 8bytes |有符号64位整型 int64_t
 *  float   | jfloat  | 4bytes |32位浮点型    float
 *  double  | jdouble | 8bytes |64位浮点型    double
 *
 *  Object  | jobject
 *  Class   | jclass
 *  String  | jstring
 *  Object[]| jobjectArray
 *  boolean[]|jbooleanArray
 *  byte[]   |jbyteArray
 *  char[]   |jcharArray
 *  short[]  |jshortArray
 *  int[]    |jintArray
 *  long[]   |jlongArray
 *  float[]  |jfloatArray
 *  double[] |jdoubleArray
 *  void     | void
 *
 *## The Value Type
 *typedef union jvalue {
    jboolean z;
    jbyte    b;
    jchar    c;
    jshort   s;
    jint     i;
    jlong    j;
    jfloat   f;
    jdouble  d;
    jobject  l;
 * } jvalue;
 *
 */
//=========================================
/**
* 目前通用的以64位系统为主。
*
* 数据类型
* c++           |  c           | 内存分配
* bool          |              |
*           |              |
*  char         | char         | 1byte
* unsigned char |  unsigned char| 1byte
* signed char   | signed char  | 1byte
* int/signed int | int          | 2bytes or 4bytes
* unsigned int  | unsigned int | 2bytes or 4bytes
* short int/signed short int | short        | 2bytes
* unsigned short int | unsigned short | 2bytes
*
*
*               | long          |4bytes
*               | unsigned long |4bytes
* long int      |               | 8bytes
* signed long int|               | 8bytes
* unsigned long int|               | 8bytes
*
* float         | float         | 4bytes
* double        | double        | 8bytes
* long double   | long double   | 16bytes
* wchar_t       |               | 2 或 4 个字节
* void* 为对象地址
*
*/

//该声明将强制编译器确保（尽它所能）变量类 型为struct S 或者int32_t 的变量在分配空间时采用8字节对齐方式。
//aligned 属性使被设置的对象占用更多的空间，相反的，使用packed 可以减小对象占用的空间。
struct FakeData {
    short b[3];//2btes

} __attribute__((aligned (8)));
typedef int int32_t __attribute__((aligned (8)));

// 类型定义
//typedef hash_map<UrlTableProperties *, string> PropertiesMap;
//
// using 别名
//using PropertiesMap = hash_map<UrlTableProperties *, string>;
//enum UrlTableErrors {
//    kOK = 0,
//    kErrorOutOfMemory,
//    kErrorMalformedInput,
//};
//常量，所有具有静态存储类型的变量 (例如静态变量或全局变量) 都应当以此方式命名
//const int kDaysInAWeek = 7;


//使用宏时要非常谨慎, 尽量以内联函数, 枚举和常量代替之.
//#define MAX_NAME_LENGTH 256
//const int kMaxNameLength=265;
//typedef struct {
//    char name[MAX_NAME_LENGTH];
//    ulong start, end;
//} MemoryMap;
//class MemoryMap {
//public:
//    char name[MAX_NAME_LENGTH];
//    ulong start, end;
//    int size(){
//        return strlen(name);
//    }
//};
typedef struct {
    u_long  start, end;
} AddressRegion;
using MemoryMap = std::map<std::string, AddressRegion>;
using Pair = std::pair<std::string, AddressRegion>;
//using String = std::string;


#endif //ATTACK_ON_TITAN_DATA_STRUCT_EXT_H


