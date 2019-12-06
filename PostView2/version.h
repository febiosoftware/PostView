// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_VERSION_H__1872313B_A23D_1120_1271_489743841BA1__INCLUDED_)
#define AFX_VERSION_H__1872313B_A23D_1120_1271_489743841BA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// VERSION INFORMATION
#define VERSION			2
#define SUBVERSION		5
#define SUBSUBVERSION	1

#ifdef SVN
#include "svnrev.h"
#else
#define SVNREVISION 0
#endif

// POSTVIEW FILE VERSION
#define POV_FILE_VERSION	3

#endif // !defined(AFX_VERSION_H__1872313B_A23D_1120_1271_489743841BA1__INCLUDED_)
