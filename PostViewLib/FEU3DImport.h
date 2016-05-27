#pragma once
#include "FEFileReader.h"
#include <assert.h>
#include <string>
using namespace std;

typedef unsigned char byte;
typedef unsigned __int16 uint16;
typedef __int32 int32;
typedef unsigned __int32 uint32;
typedef __int64 int64;
typedef unsigned __int64 uint64;

class FEU3DImport : public FEFileReader
{
public:
	// The following enums are not part of the U3D format
	// but defines by me to simplify some of the parsing
	enum { MAX_TEXTURE_LAYERS = 16 };

public:
	enum BlockType
	{
		// File Structure blocks
		ModifierChain    = 0xFFFFFF14,
		PriorityUpdate   = 0xFFFFFF15,

		// Node blocks
		GroupNode        = 0xFFFFFF21,
		ModelNode        = 0xFFFFFF22,
		LightNode        = 0xFFFFFF23,
		ViewNode         = 0xFFFFFF24,

		// geometry generator blocks
		CLODMeshDeclaration = 0xFFFFFF31,
		CLODBaseMesh        = 0xFFFFFF3b
	};

	enum ProfileFlags
	{
		Extensible    = 2,
		NoCompression = 4
	};

	enum ModifierChainType
	{
		NodeModifierChain    = 0,
		ModelResourceChain   = 1,
		TextureResourceChain = 2
	};

	enum ModifierChainAttribute
	{
		BoundingSphere = 1,
		BoundingBox    = 2
	};

	enum ViewNodeAttributes
	{
		ScreenPositionUnits    = 1,
		OrthographicProjection = 2,
		TwoPointPerspective    = 4,
		OnePointPerspective    = 8	// doc says 6 but I think that's a typo
	};

public:
	class BLOCK
	{
	public:
		uint32	blockType;		// identifies the object type
		uint32	dataSize;		// size of the data section in bytes (does not include padding)
		uint32	metaDataSize;	// size of meta data section in bytes (does not include padding)
		byte*	data;			// pointer to data block
		byte*	metaData;		// pointer to meta data block (or zero)

	public:
		BLOCK();
		~BLOCK();
	};

	struct FILE_HEADER
	{
		int32		version;			// file version. (can be less than zero)
		uint32		profile;			// profile identifier flags
		uint32		declarationSize;	// number of bytes in the Declaration block section of file (including padding)
		uint64		fileSize;			// file size. Includes size of all blocks.		
		uint32		encoding;			// character encoding (must be 106 = UTF-8)
	};

	struct BOUNDING_SPHERE
	{
		float	x, y, z;
		float	radius;
	};

	struct BOUNDING_BOX
	{
		float	xmin, ymin, zmin;
		float	xmax, ymax, zmax;
	};

	struct MODIFIER_CHAIN
	{
		string		name;		// modifier chain name
		uint32		type;		// modifier chain types
		uint32		attributes;	// modifier chain attributes. Indicate presence of bounding box information

		BOUNDING_SPHERE	boundSphere;
		BOUNDING_BOX	boundBox;

		uint32		modifierCount;	// number of modifiers in chain
	};

	struct TRANSFORM_MATRIX
	{
		float	m[4][4];	// note that this is row-order so the file actually reads the transpose (since it's column order)
	};

	struct VIEW_PORT
	{
		float	width;
		float	height;
		float	horizontalPosition;
		float	verticalPosition;
	};

	struct VIEW_NODE
	{
		string		name;			// view node name
		string		resourceName;	// view resource name
		uint32		attributes;		// view node attributes

		float	nearClip;
		float	farClip;

		VIEW_PORT	viewPort;
	};

	struct LIGHT_NODE
	{
		string		name;		// name of light node
		string		resource;	// name of resource
	};

	struct MAX_MESH_DESCRIPTION
	{
		uint32	attributes;
		uint32	faceCount;
		uint32	positionCount;
		uint32	normalCount;
		uint32	diffuseColorCount;
		uint32	specularColorCount;
		uint32	textureCoordCount;
		uint32	shadingCount;

		uint32	minResolution;
		uint32	maxResolution;
	};

	struct BASE_MESH_DESCRIPTION
	{
		uint32	faceCount;
		uint32	positionCount;
		uint32	normalCount;
		uint32	diffuseColorCount;
		uint32	specularColorCount;
		uint32	textureCoordCount;
	};

	struct SHADING_DESCRIPTION
	{
		uint32	attributes;
		uint32	textureLayerCount;
		uint32	textureCoordDimensions[MAX_TEXTURE_LAYERS];
		uint32	originalShadingId;
	};

public:
	FEU3DImport();

	bool Load(FEModel& fem, const char* szfile);

protected:
	bool readFileHeader(FILE_HEADER& fileHeader);
	bool readPriorityUpdateBlock(BLOCK& block);
	bool readModifierChainBlock(BLOCK& block);
	bool readCLODBaseMeshBlock  (BLOCK& block);

	bool readViewNodeBlock(BLOCK& block);
	bool readLightNodeBlock(BLOCK& block);
	bool readGroupNodeBlock(BLOCK& block);
	void readModelNodeBlock(BLOCK& block);
	void readCLODMeshDeclaration(BLOCK& block);

	bool readBlock(BLOCK& block, int ntype = -1);

	void readBytes(void* data, int nbytes)
	{
		fread(data, nbytes, 1, m_fp);
	}

private:
	uint32	m_priority;
};
