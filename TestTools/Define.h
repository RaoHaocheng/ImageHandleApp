#ifndef TEST_DEFINE_H
#define TEST_DEFINE_H

// standard lib
#include <fstream>   // file stream 
#include <stdarg.h>  // multiple parameter
#include <windows.h>

// ������뻷��
#ifdef  _DLL
#define  DLLS_PORT   __declspec(dllexport)      // ��̬���ӿ⵼��
#else
#define  DLLS_PORT
#endif

#define BUFFER_SIZE 1024


#endif // !DEFINE_H
