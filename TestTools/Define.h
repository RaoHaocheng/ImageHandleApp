#ifndef TEST_DEFINE_H
#define TEST_DEFINE_H

// standard lib
#include <fstream>   // file stream 
#include <stdarg.h>  // multiple parameter
#include <windows.h>

// 定义编译环境
#ifdef  _DLL
#define  DLLS_PORT   __declspec(dllexport)      // 动态链接库导出
#else
#define  DLLS_PORT
#endif

#define BUFFER_SIZE 1024


#endif // !DEFINE_H
