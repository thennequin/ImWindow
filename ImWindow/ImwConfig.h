
#ifndef __IM_CONFIG_H__
#define __IM_CONFIG_H__

#include <stdlib.h>

#ifndef NULL
#define NULL 0
#endif // NULL

#ifndef ImwList
#include <list>
#define ImwList std::list
#endif // ImwList

#ifndef ImwMap
#include <map>
#define ImwMap std::map
#endif // ImwMap

#ifndef ImwString
#include <string>
#define ImwString std::string
#endif // ImwString

#ifndef ImwChar
#define ImwChar char
#endif // ImwChar

#define ImwVerify(bCondition) { if ((bCondition) == false) { IM_ASSERT(false); } }

#define ImwSafeDelete(pObj) { if (NULL != pObj) { delete pObj; pObj = NULL; } }
#define ImwSafeRelease(pObj) { if (NULL != pObj) { pObj->Release(); pObj = NULL; } }
#define ImwIsSafe(pObj) if (NULL != pObj) pObj

#define ImwMalloc(iSize) malloc(iSize)
#define ImwFree(pObj) free(pObj)
#define ImwSafeFree(pObj) { if (pObj != NULL) { free(pObj); pObj = NULL; } }

// Define IMW_INHERITED_BY_IMWWINDOW when you want ImwWindow inherit from one of your class
//#define IMW_INHERITED_BY_IMWWINDOW MyInheritedClass

// Define IMW_CUSTOM_DECLARE_IMWWINDOW and/or IMW_CUSTOM_IMPLEMENT_IMWWINDOW for custom declaration and implementation for ImwWindow
//#define IMW_CUSTOM_BEFORE_DECLARE_IMWWINDOW 
//#define IMW_CUSTOM_DECLARE_IMWWINDOW
//#define IMW_CUSTOM_IMPLEMENT_IMWWINDOW

// Define IMW_BEFORE_WINDOW_PAINT for calling function of instancing object before painting window
//#define IMW_BEFORE_WINDOW_PAINT(pName) printf("Painting %s\n", pName);

//////////////////////////////////////////////////////////////////////////
// Include here imgui.h
//////////////////////////////////////////////////////////////////////////
#include "imgui.h"
#include "imgui_internal.h"

#endif // __IM_CONFIG_H__