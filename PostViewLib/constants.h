#if !defined(AFX_CONSTANTS_H__H2985EG6_B1B0_A89C_B529_B78AD1452AA1__INCLUDED_)
#define AFX_CONSTANTS_H__H2985EG6_B1B0_A89C_B529_B78AD1452AA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//-----------------------------------------------------------------------------
// the following macros can be used to identifiy the type of the field
#define IS_NODE_FIELD(m) ((m) & 0x00010000? true:false )
#define IS_FACE_FIELD(m) ((m) & 0x00020000? true:false )
#define IS_ELEM_FIELD(m) ((m) & 0x00040000? true:false )
#define IS_EDGE_FIELD(m) ((m) & 0x00080000? true:false )

#define IS_VALID(m) ((IS_NODE_FIELD(m))||(IS_FACE_FIELD(m))||(IS_ELEM_FIELD(m))||(IS_EDGE_FIELD(m)))

#define FIELD_CODE(n) (((n) & 0xFF00) >> 8)
#define FIELD_COMP(n) ((n) & 0x00FF)
#define BUILD_FIELD(a,b,c) (((a)<<16)|((b)<<8)|(c))

// glyph types
#define GLYPH_ARROW		0
#define GLYPH_CONE		1
#define GLYPH_CYLINDER	2
#define GLYPH_SPHERE	3
#define GLYPH_BOX		4
#define GLYPH_LINE		5

// glyph color types
#define GLYPH_COL_SOLID		0
#define GLYPH_COL_LENGTH	1
#define GLYPH_COL_ORIENT	2


#endif // AFX_CONSTANTS_H__H2985EG6_B1B0_A89C_B529_B78AD1452AA1__INCLUDED_
