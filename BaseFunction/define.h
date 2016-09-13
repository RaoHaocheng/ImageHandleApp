#ifndef BASEFUNCTION_DEFINE_H
#define BASEFUNCTION_DEFINE_H
// 基本的数据结构和类型

// 包含标准库中的内容
#include <vector>    // 容器
#include <set>
#include <list>
#include <iostream>  // 标准输入输出

// windows基本库
#include <windows.h>


// 定义编译环境
#ifdef  _DLL
#define  DLLS_PORT   __declspec(dllexport)      // 动态链接库导出
#else
#define  DLLS_PORT
#endif

#endif // !DEFINE_H