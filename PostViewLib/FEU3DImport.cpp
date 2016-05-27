#include "stdafx.h"
#include "FEU3DImport.h"

//-----------------------------------------------------------------------------
// A helper class for reading blocks
class BlockDataStream
{
public:
	BlockDataStream(FEU3DImport::BLOCK& block) : m_block(block)
	{
		m_pd = block.data;
		m_dataRead = 0;
	}

	template <typename T> BlockDataStream& operator >> (T& v)
	{
		v = *((T*)m_pd);
		advance(sizeof(T));
		assert(m_dataRead <= m_block.dataSize);
		return *this;
	}

	void readPadding()
	{
		int npad = m_dataRead % 4;
		if (npad) advance(4 - npad);
	}

	void read(void* buffer, int bytes)
	{
		memcpy(buffer, m_pd, bytes);
		advance(bytes);
	}

	bool read(FEU3DImport::BLOCK& block);

	void advance(int n)
	{
		m_pd += n;
		m_dataRead += n;
	}

private:
	FEU3DImport::BLOCK&	m_block;
	byte*				m_pd;
	uint32				m_dataRead;
};

template <> BlockDataStream& BlockDataStream::operator >> <string>(string& s)
{
	uint16 size;
	this->operator>> <uint16>(size);
	char* buf = new char[size+1];
	if (size != 0) strncpy(buf, (const char*) m_pd, size);
	buf[size] = 0;
	s = string(buf);
	delete [] buf;
	advance(size);
	return *this;
}

bool BlockDataStream::read(FEU3DImport::BLOCK& block)
{
	BlockDataStream& ar = *this;

	ar >> block.blockType;
	ar >> block.dataSize;
	ar >> block.metaDataSize;

	if (block.dataSize > 0)
	{
		block.data = new byte[block.dataSize];
		ar.read(block.data, block.dataSize);
		ar.readPadding();
	}
	else block.data = 0;

	if (block.metaDataSize > 0)
	{
		block.metaData = new byte[block.metaDataSize];
		ar.read(block.metaData, block.metaDataSize);
		ar.readPadding();
	}

	return true;
}

//-----------------------------------------------------------------------------
FEU3DImport::BLOCK::BLOCK() 
{ 
	data = 0; 
	metaData = 0; 
}

FEU3DImport::BLOCK::~BLOCK()
{ 
	if (data) delete data; 
	if (metaData) delete metaData;
}

//-----------------------------------------------------------------------------
FEU3DImport::FEU3DImport() : FEFileReader("U3D")
{
	m_priority = 0;
}

bool FEU3DImport::Load(FEModel& fem, const char* szfile)
{
	if (Open(szfile, "rb") == false) return errf("Failed opening file");

	// read the file header block
	FILE_HEADER fileHeader;
	if (readFileHeader(fileHeader) == false) return errf("This is not a valid U3D file"); 

	// read the blocks
	while (!feof(m_fp) && !ferror(m_fp))
	{
		// read the next block
		BLOCK block;
		if (readBlock(block) == false) return errf("Error in reading file");

		// process the file structure blocks
		switch (block.blockType)
		{
		case PriorityUpdate: if (readPriorityUpdateBlock(block) == false) return errf("Error in Priority Update block"); break;
		case ModifierChain : if (readModifierChainBlock (block) == false) return errf("Error in Modifier Chain block"); break;
		case CLODBaseMesh  : if (readCLODBaseMeshBlock  (block) == false) return errf("Error in CLOD Base Mesh block"); break;
		}
	}

	// close the file
	Close();

	// all done
	return true;
}

bool FEU3DImport::readFileHeader(FILE_HEADER& fileHeader)
{
	BLOCK block;
	if (readBlock(block, 0x00443355) == false) return false;

	BlockDataStream ar(block);
	ar >> fileHeader.version;
	ar >> fileHeader.profile;
	ar >> fileHeader.declarationSize;
	ar >> fileHeader.fileSize;
	ar >> fileHeader.encoding;

	return true;
}

bool FEU3DImport::readPriorityUpdateBlock(BLOCK& block)
{
	if (block.blockType != PriorityUpdate) return false;

	BlockDataStream ar(block);
	uint32 newPriority;
	ar >> newPriority; 

	// The new priority value shall not be less than previous priority value
	if (newPriority < m_priority) return false;
	m_priority = newPriority;
	return true;
}

bool FEU3DImport::readModifierChainBlock(BLOCK& block)
{
	if (block.blockType != ModifierChain) return false;
	BlockDataStream ar(block);

	MODIFIER_CHAIN modChain;
	ar >> modChain.name;
	ar >> modChain.type;
	ar >> modChain.attributes;

	if (modChain.attributes & BoundingSphere)
	{
		ar >> modChain.boundSphere.x;
		ar >> modChain.boundSphere.y;
		ar >> modChain.boundSphere.z;
		ar >> modChain.boundSphere.radius;
	}

	if (modChain.attributes & BoundingBox)
	{
		ar >> modChain.boundBox.xmin;
		ar >> modChain.boundBox.ymin;
		ar >> modChain.boundBox.zmin;
		ar >> modChain.boundBox.xmax;
		ar >> modChain.boundBox.ymax;
		ar >> modChain.boundBox.zmax;
	}

	ar.readPadding();

	ar >> modChain.modifierCount;

	for (int i=0; i<modChain.modifierCount; ++i)
	{
		BLOCK subBlock;
		ar.read(subBlock);
		switch (subBlock.blockType)
		{
		case ViewNode : if (readViewNodeBlock (subBlock) == false) return errf("Failed reading View Node block"); break;
		case LightNode: if (readLightNodeBlock(subBlock) == false) return errf("Error in Light Node block"); break;
		case GroupNode: if (readGroupNodeBlock(subBlock) == false) return errf("Error in Group Node block"); break;
		case ModelNode: readModelNodeBlock(subBlock); break;
		case CLODMeshDeclaration: readCLODMeshDeclaration(subBlock); break;
		}
	}

	return true;
}

void readParentNodeData(BlockDataStream& ar)
{
	int parents;
	ar >> parents;
	for (int i=0; i<parents; ++i)
	{
		string parentName;
		ar >> parentName;

		FEU3DImport::TRANSFORM_MATRIX m;
		ar >> m;
	}
}

bool FEU3DImport::readCLODBaseMeshBlock(BLOCK& block)
{
	if (block.blockType != CLODBaseMesh) return false;

	BlockDataStream ar(block);

	string meshName;
	ar >> meshName;

	uint32 chainIndex;
	ar >> chainIndex;

	BASE_MESH_DESCRIPTION mesh;
	ar >> mesh.faceCount;
	ar >> mesh.positionCount;
	ar >> mesh.normalCount;
	ar >> mesh.diffuseColorCount;
	ar >> mesh.specularColorCount;
	ar >> mesh.textureCoordCount;

	for (int i=0; i<mesh.positionCount; ++i)
	{
		float x, y, z;
		ar >> x >> y >> z;
	}

	for (int i=0; i<mesh.normalCount; ++i)
	{
		float x, y, z;
		ar >> x >> y >> z;
	}

	for (int i=0; i<mesh.diffuseColorCount; ++i)
	{
		float r,g,b,a;
		ar >> r >> g >> g >> a;
	}

	for (int i=0; i<mesh.specularColorCount; ++i)
	{
		float r,g,b,a;
		ar >> r >> g >> b >> a;
	}

	for (int i=0; i<mesh.textureCoordCount; ++i)
	{
		float u, v, s, t;
		ar >> u >> v >> s >> t;
	}

	for (int i=0; i<mesh.faceCount; ++i)
	{

	}


	return true;
}


void FEU3DImport::readCLODMeshDeclaration(BLOCK& block)
{
	BlockDataStream ar(block);

	string meshName;
	ar >> meshName;

	uint32 chainIndex;
	ar >> chainIndex;

	// Max mesh description
	MAX_MESH_DESCRIPTION mesh;
	ar >> mesh.attributes;
	ar >> mesh.faceCount;
	ar >> mesh.positionCount;
	ar >> mesh.normalCount;
	ar >> mesh.diffuseColorCount;
	ar >> mesh.specularColorCount;
	ar >> mesh.textureCoordCount;
	ar >> mesh.shadingCount;

	for (int i=0; i<mesh.shadingCount; ++i)
	{
		SHADING_DESCRIPTION shade;
		ar >> shade.attributes;
		ar >> shade.textureLayerCount;
		for (int j=0; j<shade.textureLayerCount; ++j)
		{
			assert(j < MAX_TEXTURE_LAYERS);
			ar >> shade.textureCoordDimensions[j];
		}
		ar >> shade.originalShadingId;
	}

	// CLOD Description
	ar >> mesh.minResolution;
	ar >> mesh.maxResolution;

	// Resource description

	// Skeleton description


}

bool FEU3DImport::readLightNodeBlock(BLOCK& block)
{
	BlockDataStream ar(block);

	LIGHT_NODE lightNode;
	ar >> lightNode.name;

	// read parent node data
	readParentNodeData(ar);

	ar >> lightNode.resource;

	return true;
}

bool FEU3DImport::readGroupNodeBlock(BLOCK& block)
{
	BlockDataStream ar(block);

	string groupName;
	ar >> groupName;

	readParentNodeData(ar);

	return true;
}

void FEU3DImport::readModelNodeBlock(BLOCK& block)
{
	BlockDataStream ar(block);

	string modelName;
	ar >> modelName;

	readParentNodeData(ar);

	string resourceName;
	ar >> resourceName;

	uint32 visibility;
	ar >> visibility;
}

bool FEU3DImport::readViewNodeBlock(BLOCK& block)
{
	BlockDataStream ar(block);

	VIEW_NODE view;
	ar >> view.name;

	// parent noda data
	readParentNodeData(ar);

	// continue reading view node data
	ar >> view.resourceName;
	ar >> view.attributes;
	ar >> view.nearClip;
	ar >> view.farClip;

	// TODO: This can also be a vector.
	float viewProjection;
	ar >> viewProjection;

	ar >> view.viewPort.width;
	ar >> view.viewPort.height;
	ar >> view.viewPort.horizontalPosition;
	ar >> view.viewPort.verticalPosition;

	return true;
}

bool FEU3DImport::readBlock(BLOCK& block, int ntype)
{
	// read block type
	readBytes(&block.blockType, sizeof(int));
	if ((ntype != -1) && (block.blockType != ntype)) return false;

	// read data size
	readBytes(&block.dataSize, sizeof(int));

	// read meta data size
	readBytes(&block.metaDataSize, sizeof(int));

	// read the block data
	if (block.dataSize > 0)
	{
		block.data = new unsigned char[block.dataSize];
		readBytes(block.data, block.dataSize);
		
		// read padding
		int padding = block.dataSize % 4;
		if (padding > 0)
		{
			int dummy;
			readBytes(&dummy, 4 - padding);
		}
	} else block.data = 0;

	// read the meta data
	if (block.metaDataSize)
	{
		block.metaData = new unsigned char[block.metaDataSize];
		readBytes(block.metaData, block.metaDataSize);
		
		// read padding
		int padding = block.metaDataSize % 4;
		if (padding > 0)
		{
			int dummy;
			readBytes(&dummy, 4 - padding);
		}
	} else block.metaData = 0;

	return true;
}
