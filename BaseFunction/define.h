#ifndef BASEFUNCTION_DEFINE_H
#define BASEFUNCTION_DEFINE_H
// ���������ݽṹ������

// ������׼���е�����
#include <vector>    // ����
#include <set>
#include <list>
#include <iostream>  // ��׼�������

// windows������
#include <windows.h>


// ������뻷��
#ifdef  _DLL
#define  DLLS_PORT   __declspec(dllexport)      // ��̬���ӿ⵼��
#else
#define  DLLS_PORT
#endif

#endif // !DEFINE_H