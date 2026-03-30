/*!	\file mtypedef.h
	\brief Файл с объявлениями типов переменных и служебных макросов.
	\author Кириллов А.В.
	\date 13.04.2010 г.
*/
#ifndef __MY_TYPEDEF_
#define __MY_TYPEDEF_
#include "stdint.h"
#ifndef NULL
	#ifdef __cplusplus
		#define NULL    0
	#else
		#define NULL    ((void *)0)
	#endif
#endif

#ifndef FALSE
	#define FALSE	0
#endif

#ifndef TRUE
	#define TRUE	1
#endif

#ifndef IN
	#define IN
#endif	//	IN

#ifndef OUT
	#define OUT
#endif	//	OUT

#ifndef VAR
	#define VAR
#endif	//	VAR

	#if defined (_WIN32)||defined (_WIN64)	
		//	Visual C++, CodeGear and .... ( Development platform for Windows )
		#ifndef QWORD
			typedef unsigned __int64 QWORD;
		#endif	//	QWORD
		#ifndef LLONG
			typedef __int64 LLONG;
		#endif	//	LLONG
	#else // 	VisualDSP++, IAR, Keil and ...
		#ifndef BOOL
			typedef int BOOL;
		#endif	//	BOOL
		#ifndef BYTE
            typedef unsigned char BYTE;
		#endif	//	BYTE
		#ifndef WORD
            typedef unsigned short WORD;
		#endif	// WORD
		#ifndef DWORD
            typedef unsigned long DWORD;
		#endif	//	DWORD
		#ifndef QWORD
            typedef unsigned long long QWORD;
		#endif	//	QWORD
		#ifndef LLONG
            typedef long long LLONG;
		#endif	// LLONG
	#endif


//#define RFUNC(x)  { if ( x!=0 ) return x; }
#define RFUNC(x) x;

#endif	//	__MY_TYPEDEF_

