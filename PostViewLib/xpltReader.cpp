#include "XpltReader.h"
#include "FENodeFaceTable.h"
#include "FEDataManager.h"
#include "FEMeshData_T.h"
#include "FEState.h"
#include "FEMesh.h"
#include "FEMeshData_T.h"

extern GLCOLOR pal[];

XpltReader::XpltReader() : FEFileReader("FEBio plot")
{
	m_pstate = 0;
	m_read_state_flag = XPLT_READ_ALL_STATES;
}

XpltReader::~XpltReader()
{
}

//-----------------------------------------------------------------------------
void XpltReader::Clear()
{
	m_dic.Clear();
	m_Mat.clear();
	m_Node.clear();
	m_Dom.clear();
	m_Surf.clear();
	m_bHasDispl = false;
	m_bHasStress = false;
	m_bHasNodalStress = false;
	m_bHasShellThickness = false;
	m_bHasFluidPressure = false;
	m_bHasElasticity = false;
	m_nel = 0;
	m_pstate = 0;
}

//-----------------------------------------------------------------------------
bool XpltReader::Load(FEModel& fem, const char* szfile)
{
	// make sure all data is cleared
	Clear();

	// open the file
	if (Open(szfile, "rb") == false) return errf("Failed opening file.");

	// attach the file to the archive
	if (m_ar.Open(m_fp) == false) return errf("This is not a valid FEBio plot file.");

	// read the root section (no compression for this section)
	m_ar.SetCompression(0);
	if (m_ar.OpenChunk() == IO_OK)
	{
		if (m_ar.GetChunkID() != PLT_ROOT) return errf("Error while reading root section");
		if (ReadRootSection(fem) == false) return false;
		m_ar.CloseChunk();
	}
	else return errf("Error while reading root section");

	// Build the mesh
	if (BuildMesh(fem) == false) return false;

	// Clear the end-flag
	if (m_ar.OpenChunk() != IO_END) return false;

	// read the state sections (these could be compressed)
	m_ar.SetCompression(m_hdr.ncompression);
	int nstate = 0;
	try{
		while (true)
		{
			if (m_ar.OpenChunk() != IO_OK) break;

			if (m_ar.GetChunkID() == PLT_STATE)
			{
				if (m_pstate) { delete m_pstate; m_pstate = 0; }
				if (ReadStateSection(fem) == false) break;
				if (m_read_state_flag == XPLT_READ_ALL_STATES) { fem.AddState(m_pstate); m_pstate = 0; }
				else if (m_read_state_flag == XPLT_READ_STATES_FROM_LIST)
				{
					int n = (int) m_state_list.size();
					for (int i=0; i<n; ++i)
					{
						if (m_state_list[i] == nstate)
						{
							fem.AddState(m_pstate); 
							m_pstate = 0;
							break;
						}
					}
				}
			}
			else errf("Error while reading state data.");
			m_ar.CloseChunk();
		
			// clear end-flag
			if (m_ar.OpenChunk() != IO_END)
			{

				break;
			}

			++nstate;
		}
		if (m_read_state_flag == XPLT_READ_LAST_STATE_ONLY) { fem.AddState(m_pstate); m_pstate = 0; }
	}
	catch (...)
	{
		errf("An unknown exception has occurred.\nNot all data was read in.");
	}

	m_ar.Close();
	Close();

	return true;
}

//-----------------------------------------------------------------------------
bool XpltReader::ReadRootSection(FEModel& fem)
{
	while (m_ar.OpenChunk() == IO_OK)
	{
		int nid = m_ar.GetChunkID();
		switch (nid)
		{
		case PLT_HEADER    : if (ReadHeader    ()    == false) return false; break;
		case PLT_DICTIONARY: if (ReadDictionary(fem) == false) return false; break;
		case PLT_MATERIALS : if (ReadMaterials (fem) == false) return false; break;
		case PLT_GEOMETRY  : if (ReadMesh      (fem) == false) return false; break;
		default:
			return errf("Failed reading Root section");
		}
		m_ar.CloseChunk();
	}
	return true;
}

//-----------------------------------------------------------------------------
bool XpltReader::ReadHeader()
{
	m_hdr.nversion			= 0;
	m_hdr.nn				= 0;
	m_hdr.nmax_facet_nodes  = 4;	// default for version 0.1
	m_hdr.ncompression      = 0;	// default for version < 0.3
	while (m_ar.OpenChunk() == IO_OK)
	{
		int nid = m_ar.GetChunkID();
		switch (nid)
		{
		case PLT_HDR_VERSION        : m_ar.read(m_hdr.nversion); break;
		case PLT_HDR_NODES          : m_ar.read(m_hdr.nn); break;
		case PLT_HDR_MAX_FACET_NODES: m_ar.read(m_hdr.nmax_facet_nodes); break;
		case PLT_HDR_COMPRESSION    : m_ar.read(m_hdr.ncompression); break;
		default:
			return errf("Error while reading header.");
		}
		m_ar.CloseChunk();
	}
	if (m_hdr.nversion > 4) return false;
	if (m_hdr.nn == 0) return false;
	return true;
}

//-----------------------------------------------------------------------------
bool XpltReader::ReadDictItem(DICT_ITEM& it)
{
	char szname[64] = {0};
	while (m_ar.OpenChunk() == IO_OK)
	{
		int nid = m_ar.GetChunkID();
		switch(nid)
		{
		case PLT_DIC_ITEM_TYPE: m_ar.read(it.ntype); break;
		case PLT_DIC_ITEM_FMT : m_ar.read(it.nfmt ); break;
		case PLT_DIC_ITEM_NAME: 
			{
				m_ar.read(szname, DI_NAME_SIZE);
				char* sz = strchr(szname, '=');
				if (sz) *sz++ = 0; else sz = szname;
				strcpy(it.szname, sz);
			}
			break;
		default:
			return errf("Error while reading dictionary section");
		}
		m_ar.CloseChunk();
	}
	return true;
}

//-----------------------------------------------------------------------------
bool XpltReader::ReadDictionary(FEModel& fem)
{
	while (m_ar.OpenChunk() == IO_OK)
	{
		int nid = m_ar.GetChunkID();
		switch (nid)
		{
		case PLT_DIC_GLOBAL   : ReadGlobalDicItems  (); break;
		case PLT_DIC_MATERIAL : ReadMaterialDicItems(); break;
		case PLT_DIC_NODAL    : ReadNodeDicItems    (); break;
		case PLT_DIC_DOMAIN   : ReadElemDicItems    (); break;
		case PLT_DIC_SURFACE  : ReadFaceDicItems    (); break;
		default:
			return errf("Error while reading Dictionary.");
		}
		m_ar.CloseChunk();
	}

	// clear data manager
	FEDataManager* pdm = fem.GetDataManager();
	pdm->Clear();

	// read nodal variables
	int i;
	int nv = m_dic.m_Node.size();
	for (i=0; i<nv; ++i)
	{
		DICT_ITEM& it = m_dic.m_Node[i];

		// add nodal field
		switch (it.ntype)
		{
		case FLOAT  : pdm->AddDataField(new FEDataField_T<FENodeData<float  > >(it.szname)); break;
		case VEC3F  : pdm->AddDataField(new FEDataField_T<FENodeData<vec3f  > >(it.szname)); break;
		case MAT3FS : pdm->AddDataField(new FEDataField_T<FENodeData<mat3fs > >(it.szname)); break;
		case MAT3FD : pdm->AddDataField(new FEDataField_T<FENodeData<mat3fd > >(it.szname)); break;
        case TENS4FS: pdm->AddDataField(new FEDataField_T<FENodeData<tens4fs> >(it.szname)); break;
		case MAT3F  : pdm->AddDataField(new FEDataField_T<FENodeData<mat3f  > >(it.szname)); break;
		default:
			return errf("Error while reading dictionar");
		}
	}

	// read solid variables
	nv = m_dic.m_Elem.size();
	for (i=0; i<nv; ++i)
	{
		DICT_ITEM& it = m_dic.m_Elem[i];

		switch (it.nfmt)
		{
		case FMT_NODE:
			{
				switch (it.ntype)
				{
				case FLOAT  : pdm->AddDataField(new FEDataField_T<FEElementData<float  ,DATA_NODE> >(it.szname)); break;
				case VEC3F  : pdm->AddDataField(new FEDataField_T<FEElementData<vec3f  ,DATA_NODE> >(it.szname)); break;
				case MAT3FS : pdm->AddDataField(new FEDataField_T<FEElementData<mat3fs ,DATA_NODE> >(it.szname)); break;
				case MAT3FD : pdm->AddDataField(new FEDataField_T<FEElementData<mat3fd ,DATA_NODE> >(it.szname)); break;
                case TENS4FS: pdm->AddDataField(new FEDataField_T<FEElementData<tens4fs,DATA_NODE> >(it.szname)); break;
				case MAT3F  : pdm->AddDataField(new FEDataField_T<FEElementData<mat3f  ,DATA_NODE> >(it.szname)); break;
				default:
					assert(false);
					return false;
				}
			}
			break;
		case FMT_ITEM:
			{
				switch (it.ntype)
				{
				case FLOAT  : pdm->AddDataField(new FEDataField_T<FEElementData<float  ,DATA_ITEM> >(it.szname)); break;
				case VEC3F  : pdm->AddDataField(new FEDataField_T<FEElementData<vec3f  ,DATA_ITEM> >(it.szname)); break;
				case MAT3FS : pdm->AddDataField(new FEDataField_T<FEElementData<mat3fs ,DATA_ITEM> >(it.szname)); break;
				case MAT3FD : pdm->AddDataField(new FEDataField_T<FEElementData<mat3fd ,DATA_ITEM> >(it.szname)); break;
                case TENS4FS: pdm->AddDataField(new FEDataField_T<FEElementData<tens4fs,DATA_ITEM> >(it.szname)); break;
				case MAT3F  : pdm->AddDataField(new FEDataField_T<FEElementData<mat3f  ,DATA_ITEM> >(it.szname)); break;
				default:
					assert(false);
					return false;
				}
			}
			break;
		case FMT_MULT:
			{
				switch (it.ntype)
				{
				case FLOAT  : pdm->AddDataField(new FEDataField_T<FEElementData<float  ,DATA_COMP> >(it.szname)); break;
				case VEC3F  : pdm->AddDataField(new FEDataField_T<FEElementData<vec3f  ,DATA_COMP> >(it.szname)); break;
				case MAT3FS : pdm->AddDataField(new FEDataField_T<FEElementData<mat3fs ,DATA_COMP> >(it.szname)); break;
				case MAT3FD : pdm->AddDataField(new FEDataField_T<FEElementData<mat3fd ,DATA_COMP> >(it.szname)); break;
                case TENS4FS: pdm->AddDataField(new FEDataField_T<FEElementData<tens4fs,DATA_COMP> >(it.szname)); break;
				case MAT3F  : pdm->AddDataField(new FEDataField_T<FEElementData<mat3f  ,DATA_COMP> >(it.szname)); break;
				default:
					assert(false);
					return false;
				}
			}
			break;
		case FMT_REGION:
			{
				switch (it.ntype)
				{
				case FLOAT  : pdm->AddDataField(new FEDataField_T<FEElementData<float  ,DATA_REGION> >(it.szname)); break;
				case VEC3F  : pdm->AddDataField(new FEDataField_T<FEElementData<vec3f  ,DATA_REGION> >(it.szname)); break;
				case MAT3FS : pdm->AddDataField(new FEDataField_T<FEElementData<mat3fs ,DATA_REGION> >(it.szname)); break;
				case MAT3FD : pdm->AddDataField(new FEDataField_T<FEElementData<mat3fd ,DATA_REGION> >(it.szname)); break;
                case TENS4FS: pdm->AddDataField(new FEDataField_T<FEElementData<tens4fs,DATA_REGION> >(it.szname)); break;
				case MAT3F  : pdm->AddDataField(new FEDataField_T<FEElementData<mat3f  ,DATA_REGION> >(it.szname)); break;
				default:
					assert(false);
					return false;
				}
			}
			break;
		default:
			assert(false);
			return errf("Error while reading dictionary");
		}
	}

	// read face variables
	nv = m_dic.m_Face.size();
	for (i=0; i<nv; ++i)
	{
		DICT_ITEM& it = m_dic.m_Face[i];

		switch (it.nfmt)
		{
		case FMT_NODE:
			{
				switch (it.ntype)
				{
				case FLOAT  : pdm->AddDataField(new FEDataField_T<FEFaceData<float  ,DATA_NODE> >(it.szname)); break;
				case VEC3F  : pdm->AddDataField(new FEDataField_T<FEFaceData<vec3f  ,DATA_NODE> >(it.szname)); break;
				case MAT3FS : pdm->AddDataField(new FEDataField_T<FEFaceData<mat3fs ,DATA_NODE> >(it.szname)); break;
				case MAT3FD : pdm->AddDataField(new FEDataField_T<FEFaceData<mat3fd ,DATA_NODE> >(it.szname)); break;
                case TENS4FS: pdm->AddDataField(new FEDataField_T<FEFaceData<tens4fs,DATA_NODE> >(it.szname)); break;
				case MAT3F  : pdm->AddDataField(new FEDataField_T<FEFaceData<mat3f  ,DATA_NODE> >(it.szname)); break;
				default:
					assert(false);
				}
			}
			break;
		case FMT_ITEM:
			{
				switch (it.ntype)
				{
				case FLOAT  : pdm->AddDataField(new FEDataField_T<FEFaceData<float  ,DATA_ITEM> >(it.szname)); break;
				case VEC3F  : pdm->AddDataField(new FEDataField_T<FEFaceData<vec3f  ,DATA_ITEM> >(it.szname)); break;
				case MAT3FS : pdm->AddDataField(new FEDataField_T<FEFaceData<mat3fs ,DATA_ITEM> >(it.szname)); break;
				case MAT3FD : pdm->AddDataField(new FEDataField_T<FEFaceData<mat3fd ,DATA_ITEM> >(it.szname)); break;
                case TENS4FS: pdm->AddDataField(new FEDataField_T<FEFaceData<tens4fs,DATA_ITEM> >(it.szname)); break;
				case MAT3F  : pdm->AddDataField(new FEDataField_T<FEFaceData<mat3f  ,DATA_ITEM> >(it.szname)); break;
				default:
					assert(false);
				}
			}
			break;
		case FMT_MULT:
			{
				switch (it.ntype)
				{
				case FLOAT  : pdm->AddDataField(new FEDataField_T<FEFaceData<float  ,DATA_COMP> >(it.szname)); break;
				case VEC3F  : pdm->AddDataField(new FEDataField_T<FEFaceData<vec3f  ,DATA_COMP> >(it.szname)); break;
				case MAT3FS : pdm->AddDataField(new FEDataField_T<FEFaceData<mat3fs ,DATA_COMP> >(it.szname)); break;
				case MAT3FD : pdm->AddDataField(new FEDataField_T<FEFaceData<mat3fd ,DATA_COMP> >(it.szname)); break;
                case TENS4FS: pdm->AddDataField(new FEDataField_T<FEFaceData<tens4fs,DATA_COMP> >(it.szname)); break;
				case MAT3F  : pdm->AddDataField(new FEDataField_T<FEFaceData<mat3f  ,DATA_COMP> >(it.szname)); break;
				default:
					assert(false);
				}
			}
			break;
		case FMT_REGION:
			{
				switch (it.ntype)
				{
				case FLOAT  : pdm->AddDataField(new FEDataField_T<FEFaceData<float  ,DATA_REGION> >(it.szname)); break;
				case VEC3F  : pdm->AddDataField(new FEDataField_T<FEFaceData<vec3f  ,DATA_REGION> >(it.szname)); break;
				case MAT3FS : pdm->AddDataField(new FEDataField_T<FEFaceData<mat3fs ,DATA_REGION> >(it.szname)); break;
				case MAT3FD : pdm->AddDataField(new FEDataField_T<FEFaceData<mat3fd ,DATA_REGION> >(it.szname)); break;
                case TENS4FS: pdm->AddDataField(new FEDataField_T<FEFaceData<tens4fs,DATA_REGION> >(it.szname)); break;
				case MAT3F  : pdm->AddDataField(new FEDataField_T<FEFaceData<mat3f  ,DATA_REGION> >(it.szname)); break;
				default:
					assert(false);
				}
			}
			break;
		default:
			assert(false);
			return errf("Error reading dictionary");
		}
	}

	// add additional displacement fields
	if (m_bHasDispl) 
	{
		pdm->AddDataField(new FEDataField_T<FELagrangeStrain>("Lagrange strain"  ));
		pdm->AddDataField(new FEDataField_T<FENodePosition  >("position"         ));
		pdm->AddDataField(new FEDataField_T<FENodeInitPos   >("initial position" ));
	}

	// add additional stress fields
	if (m_bHasStress)
	{
		pdm->AddDataField(new FEDataField_T<FEElemPressure>("pressure"));
		
		if (m_bHasFluidPressure) {
			pdm->AddDataField(new FEDataField_T<FESolidStress>("solid stress"));
		}
	}

	// add additional stress fields
	if (m_bHasNodalStress)
	{
		pdm->AddDataField(new FEDataField_T<FEElemNodalPressure>("nodal pressure"));
	}

	return true;
}

//-----------------------------------------------------------------------------
bool XpltReader::ReadGlobalDicItems()
{
	while (m_ar.OpenChunk() == IO_OK)
	{
		int nid = m_ar.GetChunkID();
		if (nid == PLT_DIC_ITEM)
		{
			DICT_ITEM it;
			ReadDictItem(it);
			m_dic.m_Glb.push_back(it);
		}
		else 
		{
			assert(false);
			return errf("Error reading Global section in Dictionary");
		}
		m_ar.CloseChunk();
	}
	return true;
}

//-----------------------------------------------------------------------------
bool XpltReader::ReadMaterialDicItems()
{
	while (m_ar.OpenChunk() == IO_OK)
	{
		int nid = m_ar.GetChunkID();
		if (nid == PLT_DIC_ITEM)
		{
			DICT_ITEM it;
			ReadDictItem(it);
			m_dic.m_Mat.push_back(it);
		}
		else 
		{
			assert(false);
			return errf("Error reading Material section in Dictionary");
		}
		m_ar.CloseChunk();
	}
	return true;
}

//-----------------------------------------------------------------------------
bool XpltReader::ReadNodeDicItems()
{
	while (m_ar.OpenChunk() == IO_OK)
	{
		int nid = m_ar.GetChunkID();
		if (nid == PLT_DIC_ITEM)
		{
			DICT_ITEM it;
			ReadDictItem(it);
			if (strcmp(it.szname, "displacement") == 0) m_bHasDispl = true;
			m_dic.m_Node.push_back(it);
		}
		else 
		{
			assert(false);
			return errf("Error reading Node section in Dictionary");
		}
		m_ar.CloseChunk();
	}
	return true;
}

//-----------------------------------------------------------------------------
bool XpltReader::ReadElemDicItems()
{
	while (m_ar.OpenChunk() == IO_OK)
	{
		int nid = m_ar.GetChunkID();
		if (nid == PLT_DIC_ITEM)
		{
			DICT_ITEM it;
			ReadDictItem(it);
			if (strcmp(it.szname, "stress"         ) == 0) m_bHasStress         = true;
			if (strcmp(it.szname, "nodal stress"   ) == 0) m_bHasNodalStress    = true;
			if (strcmp(it.szname, "shell thickness") == 0) m_bHasShellThickness = true;
			if (strcmp(it.szname, "fluid pressure" ) == 0) m_bHasFluidPressure  = true;
			if (strcmp(it.szname, "elasticity"     ) == 0) m_bHasElasticity     = true;
			m_dic.m_Elem.push_back(it);
		}
		else 
		{
			assert(false);
			return errf("Error reading Element section in Dictionary");
		}
		m_ar.CloseChunk();
	}
	return true;
}

//-----------------------------------------------------------------------------
bool XpltReader::ReadFaceDicItems()
{
	while (m_ar.OpenChunk() == IO_OK)
	{
		int nid = m_ar.GetChunkID();
		if (nid == PLT_DIC_ITEM)
		{
			DICT_ITEM it;
			ReadDictItem(it);
			m_dic.m_Face.push_back(it);
		}
		else 
		{
			assert(false);
			return errf("Error reading Face section in Dictionary");
		}
		m_ar.CloseChunk();
	}
	return true;
}

//-----------------------------------------------------------------------------
bool XpltReader::ReadMaterials(FEModel& fem)
{
	while (m_ar.OpenChunk() == IO_OK)
	{
		int nid = m_ar.GetChunkID();
		if (nid == PLT_MATERIAL)
		{
			MATERIAL m;
			char sz[DI_NAME_SIZE] = {0};
			while (m_ar.OpenChunk() == IO_OK)
			{
				switch (m_ar.GetChunkID())
				{
				case PLT_MAT_ID  : m_ar.read(m.nid); break;
				case PLT_MAT_NAME: m_ar.read(sz, DI_NAME_SIZE); break;
				}
				m_ar.CloseChunk();
			}
			strcpy(m.szname, sz);
			m_Mat.push_back(m);
		}
		else
		{
			assert(false);
			return errf("Error while reading materials");
		}
		m_ar.CloseChunk();
	}
	CreateMaterials(fem);
	return true;
}

//-----------------------------------------------------------------------------
void XpltReader::CreateMaterials(FEModel& fem)
{
	// initialize material properties
	fem.ClearMaterials();
	int nmat = m_Mat.size();
	for (int i=0; i<nmat; i++)
	{
		FEMaterial m;
		m.diffuse = pal[i%32];
		m.ambient = pal[i%32];
		m.specular = GLCOLOR(128,128,128);
		m.emission = GLCOLOR(0,0,0);
		m.shininess = 0.5f;
		m.transparency = 1.f;
		m.benable = true;
		m.bvisible = true;
		m.bmesh = true;
		m.bcast_shadows = true;
		m.SetName(m_Mat[i].szname);
		fem.AddMaterial(m);
	}
}

//-----------------------------------------------------------------------------
bool XpltReader::ReadMesh(FEModel &fem)
{
	while (m_ar.OpenChunk() == IO_OK)
	{
		switch (m_ar.GetChunkID())
		{
		case PLT_NODE_SECTION   : if (ReadNodeSection   (fem) == false) return false; break;
		case PLT_DOMAIN_SECTION : if (ReadDomainSection (fem) == false) return false; break;
		case PLT_SURFACE_SECTION: if (ReadSurfaceSection(fem) == false) return false; break;
		case PLT_NODESET_SECTION: if (ReadNodeSetSection(fem) == false) return false; break;
		default:
			assert(false);
			return errf("Error while reading mesh");
		}
		m_ar.CloseChunk();
	}

	return true;
}

//-----------------------------------------------------------------------------
bool XpltReader::ReadNodeSection(FEModel &fem)
{
	vector<float> a(3*m_hdr.nn);
	while (m_ar.OpenChunk() == IO_OK)
	{
		if (m_ar.GetChunkID() == PLT_NODE_COORDS) m_ar.read(a);
		else
		{
			assert(false);
			return errf("Error while reading Node section");
		}
		m_ar.CloseChunk();
	}

	m_Node.resize(m_hdr.nn);
	for (int i=0; i<m_hdr.nn; ++i)
	{
		NODE& n = m_Node[i];
		n.r.x = a[3*i  ];
		n.r.y = a[3*i+1];
		n.r.z = a[3*i+2];
	}

	return true;
}

//-----------------------------------------------------------------------------
bool XpltReader::ReadDomainSection(FEModel &fem)
{
	int nd = 0, index = 0;
	while (m_ar.OpenChunk() == IO_OK)
	{
		if (m_ar.GetChunkID() == PLT_DOMAIN)
		{
			Domain D;
			while (m_ar.OpenChunk() == IO_OK)
			{
				int nid = m_ar.GetChunkID();
				if (nid == PLT_DOMAIN_HDR)
				{
					// read the domain header
					while (m_ar.OpenChunk() == IO_OK)
					{
						switch (m_ar.GetChunkID())
						{
						case PLT_DOM_ELEM_TYPE: m_ar.read(D.etype); break;
						case PLT_DOM_MAT_ID   : m_ar.read(D.mid); break;
						case PLT_DOM_ELEMS    : m_ar.read(D.ne); break;
						case PLT_DOM_NAME     : m_ar.read(D.szname); break;
						default:
							assert(false);
							return errf("Error while reading Domain section");
						}
						m_ar.CloseChunk();
					}
				}
				else if (nid == PLT_DOM_ELEM_LIST)
				{
					assert(D.ne > 0);
					D.elem.reserve(D.ne);
					D.elist.reserve(D.ne);
					int ne = 0;
					switch (D.etype)
					{
					case PLT_ELEM_HEX8 : ne =  8; break;
					case PLT_ELEM_PENTA: ne =  6; break;
					case PLT_ELEM_TET  : ne =  4; break;
					case PLT_ELEM_QUAD : ne =  4; break;
					case PLT_ELEM_TRI  : ne =  3; break;
					case PLT_ELEM_TRUSS: ne =  2; break;
					case PLT_ELEM_HEX20: ne = 20; break;
					case PLT_ELEM_HEX27: ne = 27; break;
					case PLT_ELEM_TET10: ne = 10; break;
					case PLT_ELEM_TET15: ne = 15; break;
                    case PLT_ELEM_TRI6 : ne =  6; break;
                    case PLT_ELEM_QUAD8: ne =  8; break;
                    case PLT_ELEM_QUAD9: ne =  9; break;
					default:
						assert(false);
						return errf("Error while reading Domain section");
					}
					assert((ne > 0)&&(ne <= FEElement::MAX_NODES));
					int n[FEElement::MAX_NODES + 1];
					while (m_ar.OpenChunk() == IO_OK)
					{
						if (m_ar.GetChunkID() == PLT_ELEMENT)
						{
							ELEM e;
							m_ar.read(n, ne+1);
							e.index = index++;
							e.eid = n[0];
							for (int i=0; i<ne; ++i) { e.node[i] = n[i+1]; assert(e.node[i] < (int)m_Node.size()); }
							D.elem.push_back(e);
							D.elist.push_back(e.index);
						}
						else
						{
							assert(false);
							return errf("Error while reading Domain section");
						}
						m_ar.CloseChunk();
					}
				}
				else
				{
					assert(false);
					return errf("Error while reading Domain section");
				}
				m_ar.CloseChunk();
			}
			assert(D.ne == D.elem.size());
			D.nid = nd++;
			m_Dom.push_back(D);
		}
		else
		{
			assert(false);
			return errf("Error while reading Domain section");
		}
		m_ar.CloseChunk();
	}
	return true;
}

//-----------------------------------------------------------------------------
bool XpltReader::ReadSurfaceSection(FEModel &fem)
{
	int nodes_per_facet = m_hdr.nmax_facet_nodes;

	// in previous versions there was a bug in the number
	// of nodes written so we need to make an adjustment.
	if (m_hdr.nversion < 0x04) nodes_per_facet -= 2;

	while (m_ar.OpenChunk() == IO_OK)
	{
		if (m_ar.GetChunkID() == PLT_SURFACE)
		{
			Surface S;
			while (m_ar.OpenChunk() == IO_OK)
			{
				int nid = m_ar.GetChunkID();
				if (nid == PLT_SURFACE_HDR)
				{
					// read the surface header
					while (m_ar.OpenChunk() == IO_OK)
					{
						switch(m_ar.GetChunkID())
						{
						case PLT_SURFACE_ID   : m_ar.read(S.sid); break;
						case PLT_SURFACE_FACES: m_ar.read(S.nf); break;
						case PLT_SURFACE_NAME : m_ar.read(S.szname); break;
						default:
							assert(false);
							return errf("Error while reading Surface section");
						}
						m_ar.CloseChunk();
					}
				}
				else if (nid == PLT_FACE_LIST)
				{
					if (m_hdr.nversion == 1)
					{
						assert(S.nf > 0);
						S.face.reserve(S.nf);
						int n[5];
						while (m_ar.OpenChunk() == IO_OK)
						{
							if (m_ar.GetChunkID() == PLT_FACE)
							{
								m_ar.read(n, 5);
								FACE f;
								f.nid = n[0];
								f.node[0] = n[1];
								f.node[1] = n[2];
								f.node[2] = n[3];
								f.node[3] = n[4];
								f.nn = (f.node[3] == f.node[2] ? 3 : 4);
								S.face.push_back(f);
							}
							else 
							{
								assert(false);
								return errf("Error while reading Surface section");
							}
							m_ar.CloseChunk();
						}
					}
					else
					{
						assert(S.nf > 0);
						S.face.reserve(S.nf);
						int n[11];
						assert(m_hdr.nmax_facet_nodes <= 9);
						while (m_ar.OpenChunk() == IO_OK)
						{
							if (m_ar.GetChunkID() == PLT_FACE)
							{
								m_ar.read(n, nodes_per_facet+2);
								FACE f;
								f.nid = n[0];
								f.nn = n[1];
								for (int i=0; i<f.nn; ++i) f.node[i] = n[2+i];
								S.face.push_back(f);
							}
							else 
							{
								assert(false);
								return errf("Error while reading Surface section");
							}
							m_ar.CloseChunk();
						}
					}
				}
				m_ar.CloseChunk();
			}
			assert(S.nf == S.face.size());
			m_Surf.push_back(S);
		}
		else
		{
			assert(false);
			return errf("Error while reading Surface section");
		}
		m_ar.CloseChunk();
	}
	return true;
}

//-----------------------------------------------------------------------------
bool XpltReader::ReadNodeSetSection(FEModel& fem)
{
	while (m_ar.OpenChunk() == IO_OK)
	{
		if (m_ar.GetChunkID() == PLT_NODESET)
		{
			NodeSet S;
			while (m_ar.OpenChunk() == IO_OK)
			{
				int nid = m_ar.GetChunkID();
				if (nid == PLT_NODESET_HDR)
				{
					// read the nodeset header
					while (m_ar.OpenChunk() == IO_OK)
					{
						switch(m_ar.GetChunkID())
						{
						case PLT_NODESET_ID   : m_ar.read(S.nid); break;
						case PLT_NODESET_SIZE : m_ar.read(S.nn); break;
						case PLT_NODESET_NAME : m_ar.read(S.szname); break;
						default:
							assert(false);
							return errf("Error while reading NodeSet section");
						}
						m_ar.CloseChunk();
					}
				}
				else if (nid == PLT_NODESET_LIST)
				{
					S.node.assign(S.nn, 0);
					m_ar.read(S.node);
				}
				else
				{
					assert(false);
					return errf("Error while reading NodeSet section");
				}
				m_ar.CloseChunk();
			}
			m_NodeSet.push_back(S);
		}
		else
		{
			assert(false);
			return errf("Error while reading NodeSet section");
		}
		m_ar.CloseChunk();
	}

	return true;
}

//-----------------------------------------------------------------------------
bool XpltReader::BuildMesh(FEModel &fem)
{
	int i, j, k, n;

	// get the mesh
	FEMesh& mesh = *fem.GetMesh();

	// clear the state data
	fem.ClearStates();

	// count all nodes
	int NN = m_hdr.nn;

	// count all elements
	int ND = m_Dom.size();
	int NE = 0;
	for (i=0; i<ND; ++i) NE += m_Dom[i].ne;

	// allocate storage
	mesh.Create(NN, NE);

	// read the nodal coordinates
	for (i=0; i<m_hdr.nn; i++)
	{
		FENode& n = mesh.Node(i);
		NODE& N = m_Node[i];

		// assign coordinates
		n.m_r0 = N.r;

		n.m_rt = n.m_r0;
		n.m_state = FE_VISIBLE | FE_ENABLED;
	}

	// read the element connectivity
	int nmat = fem.Materials();
	for (i=0; i<ND; i++)
	{
		Domain& D = m_Dom[i];
		for (j=0; j<D.ne; ++j)
		{
			ELEM& E = D.elem[j];
			FEElement& el = mesh.Element(E.index);
			el.m_MatID = D.mid - 1;
			el.m_nId = E.eid;
			int ne = 0;

			switch (D.etype)
			{
			case PLT_ELEM_HEX8 : el.m_ntype = FE_HEX8  ; ne =  8; break;
			case PLT_ELEM_PENTA: el.m_ntype = FE_PENTA6; ne =  6; break;
			case PLT_ELEM_TET  : el.m_ntype = FE_TET4  ; ne =  4; break;
			case PLT_ELEM_QUAD : el.m_ntype = FE_QUAD4 ; ne =  4; break;
			case PLT_ELEM_TRI  : el.m_ntype = FE_TRI3  ; ne =  3; break;
			case PLT_ELEM_TRUSS: el.m_ntype = FE_TRUSS2; ne =  2; break;
			case PLT_ELEM_HEX20: el.m_ntype = FE_HEX20 ; ne = 20; break;
			case PLT_ELEM_HEX27: el.m_ntype = FE_HEX27 ; ne = 27; break;
			case PLT_ELEM_TET10: el.m_ntype = FE_TET10 ; ne = 10; break;
			case PLT_ELEM_TET15: el.m_ntype = FE_TET15 ; ne = 15; break;
            case PLT_ELEM_TRI6 : el.m_ntype = FE_TRI6  ; ne =  6; break;
            case PLT_ELEM_QUAD8: el.m_ntype = FE_QUAD8 ; ne =  8; break;
            case PLT_ELEM_QUAD9: el.m_ntype = FE_QUAD9 ; ne =  9; break;
			}

			for (k=0; k<ne; ++k) el.m_node[k] = E.node[k];

			el.m_state = FE_VISIBLE | FE_ENABLED;
		}
	}

	// set the enabled-ness of the elements and the nodes
	for (i=0; i<mesh.Elements(); ++i)
	{
		FEElement& el = mesh.Element(i);
		FEMaterial* pm = fem.GetMaterial(el.m_MatID);
		if (pm->benable) el.Enable(); else el.Disable();
	}

	for (i=0; i<mesh.Nodes(); ++i) mesh.Node(i).Disable();
	for (i=0; i<mesh.Elements(); ++i)
	{
		FEElement& el = mesh.Element(i);
		if (el.IsEnabled())
		{
			int n = el.Nodes();
			for (j=0; j<n; ++j) mesh.Node(el.m_node[j]).Enable();
		}
	}

	// Update the mesh
	// This will also build the faces
	mesh.Update();

	// Next, we'll build a Node-Face lookup table
	FENodeFaceTable NFT(&mesh);

	// next, we reindex the surfaces
	for (n=0; n<(int) m_Surf.size(); ++n)
	{
		Surface& s = m_Surf[n];
		for (i=0; i<s.nf; ++i)
		{
			FACE& f = s.face[i];
			f.nid = NFT.FindFace(f.node[0], f.node, f.nn);
			assert(f.nid >= 0);
		}
	}

	// let's create the nodesets
	char szname[128]={0};
	for (n=0; n<(int)m_NodeSet.size(); ++n)
	{
		NodeSet& s = m_NodeSet[n];
		FENodeSet* ps = new FENodeSet(&mesh);
		if (s.szname[0]==0) { sprintf(szname, "nodeset%02d",n+1); ps->SetName(szname); }
		else ps->SetName(s.szname);
		ps->m_Node = s.node;
		mesh.AddNodeSet(ps);
	}

	// let's create the FE surfaces
	for (n=0; n<(int) m_Surf.size(); ++n)
	{
		Surface& s = m_Surf[n];
		FESurface* ps = new FESurface(&mesh);
		if (s.szname[0]==0) { sprintf(szname, "surface%02d",n+1); ps->SetName(szname); }
		else ps->SetName(s.szname);
		ps->m_Face.reserve(s.nf);
		for (i=0; i<s.nf; ++i) ps->m_Face.push_back(s.face[i].nid);
		mesh.AddSurface(ps);
	}

	// let's create the parts
	for (n=0; n<(int) m_Dom.size(); ++n)
	{
		Domain& s = m_Dom[n];
		FEPart* pg = new FEPart(&mesh);
		if (s.szname[0]==0) { sprintf(szname, "part%02d",n+1); pg->SetName(szname); }
		else pg->SetName(s.szname);
		pg->m_Elem.resize(s.ne);
		pg->m_Elem = s.elist;
		mesh.AddPart(pg);
	}

	fem.UpdateBoundingBox();

	return true;
}

//-----------------------------------------------------------------------------
bool XpltReader::ReadStateSection(FEModel& fem)
{
	// get the mesh
	FEMesh& mesh = *fem.GetMesh();

	// add a state
	FEState* ps = 0;
	
	try 
	{
		ps = m_pstate = new FEState(0.f, &fem);
	}
	catch (...)
	{
		m_pstate = 0;
		return errf("Error allocating memory for state data");
	}

	while (m_ar.OpenChunk() == IO_OK)
	{
		int nid = m_ar.GetChunkID();
		if (nid == PLT_STATE_HEADER)
		{
			while (m_ar.OpenChunk() == IO_OK)
			{
				if (m_ar.GetChunkID() == PLT_STATE_HDR_TIME) m_ar.read(ps->m_time);
				m_ar.CloseChunk();
			}
		}
		else if (nid == PLT_STATE_DATA)
		{
			while (m_ar.OpenChunk() == IO_OK)
			{
				switch (m_ar.GetChunkID())
				{
				case PLT_GLOBAL_DATA  : if (ReadGlobalData  (fem, ps) == false) return false; break;
				case PLT_MATERIAL_DATA: if (ReadMaterialData(fem, ps) == false) return false; break;
				case PLT_NODE_DATA    : if (ReadNodeData    (fem, ps) == false) return false; break;
				case PLT_ELEMENT_DATA : if (ReadElemData    (fem, ps) == false) return false; break;
				case PLT_FACE_DATA    : if (ReadFaceData    (fem, ps) == false) return false; break;
				default:
					assert(false);
					return errf("Invalid chunk ID");
				}
				m_ar.CloseChunk();
			}
		}
		else return errf("Invalid chunk ID");
		m_ar.CloseChunk();
	}

	// Assign shell thicknesses
	if (m_bHasShellThickness)
	{
		FEDataManager& dm = *fem.GetDataManager();
		int n = dm.FindDataField("shell thickness");
		FEElementData<float,DATA_COMP>& df = dynamic_cast<FEElementData<float,DATA_COMP>&>(ps->m_Data[n]);
		FEMesh& mesh = *fem.GetMesh();
		int NE = mesh.Elements();
		float h[FEElement::MAX_NODES] = {0.f};
		for (int i=0; i<NE; ++i)
		{
			ELEMDATA& d = ps->m_ELEM[i];
			if (df.active(i))
			{
				df.eval(i, h);
				int n = mesh.Element(i).Nodes();
				for (int j=0; j<n; ++j) d.m_h[j] = h[j];
			}
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
bool XpltReader::ReadGlobalData(FEModel& fem, FEState* pstate)
{
	return false;
}

//-----------------------------------------------------------------------------
bool XpltReader::ReadMaterialData(FEModel& fem, FEState* pstate)
{
	return false;
}

//-----------------------------------------------------------------------------
bool XpltReader::ReadNodeData(FEModel& fem, FEState* pstate)
{
	FEDataManager& dm = *fem.GetDataManager();
	while (m_ar.OpenChunk() == IO_OK)
	{
		if (m_ar.GetChunkID() == PLT_STATE_VARIABLE)
		{
			int nv = -1;
			while (m_ar.OpenChunk() == IO_OK)
			{
				int nid = m_ar.GetChunkID();
				if (nid == PLT_STATE_VAR_ID) m_ar.read(nv);
				else if (nid ==	PLT_STATE_VAR_DATA)
				{
					nv--;
					assert((nv>=0)&&(nv<(int)m_dic.m_Node.size()));
					if ((nv<0) || (nv >= (int)m_dic.m_Node.size())) return errf("Failed reading node data");

					DICT_ITEM it = m_dic.m_Node[nv];
					int nfield = dm.FindDataField(it.szname);
					int ndata = 0;
					int NN = fem.GetMesh()->Nodes();
					while (m_ar.OpenChunk() == IO_OK)
					{
						int ns = m_ar.GetChunkID();
						assert(ns == 0);

						if (it.ntype == FLOAT)
						{
							vector<float> a(NN);
							m_ar.read(a);

							FENodeData<float>& df = dynamic_cast<FENodeData<float>&>(pstate->m_Data[nfield]);
							for (int j=0; j<NN; ++j) df[j] = a[j];
						}
						else if (it.ntype == VEC3F)
						{
							vector<vec3f> a(NN);
							m_ar.read(a);

							FENodeData<vec3f>& dv = dynamic_cast<FENodeData<vec3f>&>(pstate->m_Data[nfield]);
							for (int j=0; j<NN; ++j) dv[j] = a[j];
						}
						else if (it.ntype == MAT3FS)
						{
							vector<mat3fs> a(NN);
							m_ar.read(a);
							FENodeData<mat3fs>& dv = dynamic_cast<FENodeData<mat3fs>&>(pstate->m_Data[nfield]);
							for (int j=0; j<NN; ++j) dv[j] = a[j];
						}
						else if (it.ntype == TENS4FS)
						{
							vector<tens4fs> a(NN);
							m_ar.read(a);
							FENodeData<tens4fs>& dv = dynamic_cast<FENodeData<tens4fs>&>(pstate->m_Data[nfield]);
							for (int j=0; j<NN; ++j) dv[j] = a[j];
						}
						else if (it.ntype == MAT3F)
						{
							vector<mat3f> a(NN);
							m_ar.read(a);
							FENodeData<mat3f>& dv = dynamic_cast<FENodeData<mat3f>&>(pstate->m_Data[nfield]);
							for (int j=0; j<NN; ++j) dv[j] = a[j];
						}
						else
						{
							assert(false);
							return errf("Error while reading node data");;
						}
						m_ar.CloseChunk();
					}
				}
				else
				{
					assert(false);
					return errf("Error while reading node data");
				}
				m_ar.CloseChunk();
			}
		}
		else
		{
			assert(false);
			return errf("Error while reading node data");
		}
		m_ar.CloseChunk();
	}
	return true;
}

//-----------------------------------------------------------------------------
bool XpltReader::ReadElemData(FEModel &fem, FEState* pstate)
{
	FEMesh& m = *fem.GetMesh();
	FEDataManager& dm = *fem.GetDataManager();
	while (m_ar.OpenChunk() == IO_OK)
	{
		if (m_ar.GetChunkID() == PLT_STATE_VARIABLE)
		{
			int nv = -1;
			while (m_ar.OpenChunk() == IO_OK)
			{
				int nid = m_ar.GetChunkID();
				if (nid == PLT_STATE_VAR_ID) m_ar.read(nv);
				else if (nid ==	PLT_STATE_VAR_DATA)
				{
					nv--;
					assert((nv>=0)&&(nv<(int)m_dic.m_Elem.size()));
					if ((nv < 0) || (nv >= (int) m_dic.m_Elem.size())) return errf("Failed reading all state data");
					DICT_ITEM it = m_dic.m_Elem[nv];
					while (m_ar.OpenChunk() == IO_OK)
					{
						int nd = m_ar.GetChunkID() - 1;
						assert((nd >= 0)&&(nd < (int)m_Dom.size()));
						if ((nd < 0) || (nd >= (int) m_Dom.size())) return errf("Failed reading all state data");

						int nfield = dm.FindDataField(it.szname);

						Domain& dom = m_Dom[nd];
						FEElemItemData& ed = dynamic_cast<FEElemItemData&>(pstate->m_Data[nfield]);
						switch (it.nfmt)
						{
						case FMT_NODE: ReadElemData_NODE(*fem.GetMesh(), dom, ed, it.ntype); break;
						case FMT_ITEM: ReadElemData_ITEM(dom, ed, it.ntype); break;
						case FMT_MULT: ReadElemData_MULT(dom, ed, it.ntype); break;
						case FMT_REGION: ReadElemData_REGION(dom, ed, it.ntype); break;
						default:
							assert(false);
							return errf("Error reading element data");
						}
						m_ar.CloseChunk();
					}
				}
				else
				{
					assert(false);
					return errf("Error while reading element data");
				}
				m_ar.CloseChunk();
			}
		}
		else
		{
			assert(false);
			return errf("Error while reading element data");
		}
		m_ar.CloseChunk();
	}
	return true;
}


//-----------------------------------------------------------------------------
bool XpltReader::ReadElemData_NODE(FEMesh& m, XpltReader::Domain &d, FEMeshData &data, int ntype)
{
	int ne = 0;
	switch (d.etype)
	{
	case PLT_ELEM_HEX8 : ne =  8; break;
	case PLT_ELEM_PENTA: ne =  6; break;
	case PLT_ELEM_TET  : ne =  4; break;
	case PLT_ELEM_QUAD : ne =  4; break;
	case PLT_ELEM_TRI  : ne =  3; break;
	case PLT_ELEM_TRUSS: ne =  2; break;
	case PLT_ELEM_HEX20: ne = 20; break;
	case PLT_ELEM_HEX27: ne = 27; break;
	case PLT_ELEM_TET10: ne = 10; break;
	case PLT_ELEM_TET15: ne = 15; break;
    case PLT_ELEM_TRI6 : ne =  6; break;
    case PLT_ELEM_QUAD8: ne =  8; break;
    case PLT_ELEM_QUAD9: ne =  9; break;
	default:
		assert(false);
		return errf("Error while reading element data");
	}

	int i, j;

	// set nodal tags to local node number
	int NN = m.Nodes();
	for (i=0; i<NN; ++i) m.Node(i).m_ntag = -1;

	int n = 0;
	for (i=0; i<d.ne; ++i)
	{
		ELEM& e = d.elem[i];
		for (j=0; j<ne; ++j)
			if (m.Node(e.node[j]).m_ntag == -1) m.Node(e.node[j]).m_ntag = n++;
	}

	// create the element list
	vector<int> e(d.ne);
	for (i=0; i<d.ne; ++i) e[i] = d.elem[i].index;

	// create the local node index list
	vector<int> l(ne*d.ne);
	for (i=0; i<d.ne; ++i)
	{
		ELEM& e = d.elem[i];
		for (j=0; j<ne; ++j) l[ne*i+j] = m.Node(e.node[j]).m_ntag;
	}

	// get the data
	switch (ntype)
	{
	case FLOAT:
		{
			FEElementData<float,DATA_NODE>& df = dynamic_cast<FEElementData<float,DATA_NODE>&>(data);
			vector<float> a(n);
			m_ar.read(a);
			df.add(a, e, l, ne);
		}
		break;
	case VEC3F:
		{
			FEElementData<vec3f,DATA_NODE>& df = dynamic_cast<FEElementData<vec3f,DATA_NODE>&>(data);
			vector<vec3f> a(n);
			m_ar.read(a);
			df.add(a, e, l, ne);
		}
		break;
	case MAT3F:
		{
			FEElementData<mat3f,DATA_NODE>& df = dynamic_cast<FEElementData<mat3f,DATA_NODE>&>(data);
			vector<mat3f> a(n);
			m_ar.read(a);
			df.add(a, e, l, ne);
		}
		break;
	case MAT3FS:
		{
			FEElementData<mat3fs,DATA_NODE>& df = dynamic_cast<FEElementData<mat3fs,DATA_NODE>&>(data);
			vector<mat3fs> a(n);
			m_ar.read(a);
			df.add(a, e, l, ne);
		}
		break;
	case MAT3FD:
		{
			FEElementData<mat3fd,DATA_NODE>& df = dynamic_cast<FEElementData<mat3fd,DATA_NODE>&>(data);
			vector<mat3fd> a(n);
			m_ar.read(a);
			df.add(a, e, l, ne);
		}
		break;
    case TENS4FS:
		{
			FEElementData<tens4fs,DATA_NODE>& df = dynamic_cast<FEElementData<tens4fs,DATA_NODE>&>(data);
			vector<tens4fs> a(n);
			m_ar.read(a);
			df.add(a, e, l, ne);
		}
        break;
	default:
		assert(false);
		return errf("Error while reading element data");
	}

	return true;
}

//-----------------------------------------------------------------------------
bool XpltReader::ReadElemData_ITEM(XpltReader::Domain& dom, FEMeshData& s, int ntype)
{
	int NE = dom.ne;
	switch (ntype)
	{
	case FLOAT:
		{
			vector<float> a(NE);
			m_ar.read(a);
			FEElementData<float,DATA_ITEM>& df = dynamic_cast<FEElementData<float,DATA_ITEM>&>(s);
			for (int i=0; i<NE; ++i) df.add(dom.elem[i].index, a[i]);
		}
		break;
	case VEC3F:
		{
			vector<vec3f> a(NE);
			m_ar.read(a);
			FEElementData<vec3f,DATA_ITEM>& dv = dynamic_cast<FEElementData<vec3f,DATA_ITEM>&>(s);
			for (int i=0; i<NE; ++i) dv.add(dom.elem[i].index, a[i]);
		}
		break;
	case MAT3FS:
		{
			vector<mat3fs> a(NE);
			m_ar.read(a);
			FEElementData<mat3fs,DATA_ITEM>& dm = dynamic_cast<FEElementData<mat3fs,DATA_ITEM>&>(s);
			for (int i=0; i<NE; ++i) dm.add(dom.elem[i].index, a[i]);
		}
		break;
	case MAT3FD:
		{
			vector<mat3fd> a(NE);
			m_ar.read(a);
			FEElementData<mat3fd,DATA_ITEM>& dm = dynamic_cast<FEElementData<mat3fd,DATA_ITEM>&>(s);
			for (int i=0; i<NE; ++i) dm.add(dom.elem[i].index, a[i]);
		}
		break;
    case TENS4FS:
		{
			vector<tens4fs> a(NE);
			m_ar.read(a);
			FEElementData<tens4fs,DATA_ITEM>& dm = dynamic_cast<FEElementData<tens4fs,DATA_ITEM>&>(s);
			for (int i=0; i<NE; ++i) dm.add(dom.elem[i].index, a[i]);
		}
        break;
	case MAT3F:
		{
			vector<mat3f> a(NE);
			m_ar.read(a);
			FEElementData<mat3f,DATA_ITEM>& dm = dynamic_cast<FEElementData<mat3f,DATA_ITEM>&>(s);
			for (int i=0; i<NE; ++i) dm.add(dom.elem[i].index, a[i]);
		}
		break;
	default:
		assert(false);
		return errf("Error while reading element data");
	}

	return true;
}

//-----------------------------------------------------------------------------
bool XpltReader::ReadElemData_MULT(XpltReader::Domain& dom, FEMeshData& s, int ntype)
{
	int NE = dom.ne;
	int ne = 0;
	switch (dom.etype)
	{
	case PLT_ELEM_HEX8 : ne =  8; break;
	case PLT_ELEM_PENTA: ne =  6; break;
	case PLT_ELEM_TET  : ne =  4; break;
	case PLT_ELEM_QUAD : ne =  4; break;
	case PLT_ELEM_TRI  : ne =  3; break;
	case PLT_ELEM_TRUSS: ne =  2; break;
	case PLT_ELEM_HEX20: ne = 20; break;
	case PLT_ELEM_HEX27: ne = 27; break;
	case PLT_ELEM_TET10: ne = 10; break;
	case PLT_ELEM_TET15: ne = 15; break;
    case PLT_ELEM_TRI6 : ne =  6; break;
    case PLT_ELEM_QUAD8: ne =  8; break;
    case PLT_ELEM_QUAD9: ne =  9; break;
	default:
		assert(false);
		return errf("Error while reading element data");
	}

	int nsize = NE*ne;

	switch (ntype)
	{
	case FLOAT:
		{
			vector<float> a(nsize), d(NE);
			m_ar.read(a);

			FEElementData<float,DATA_COMP>& df = dynamic_cast<FEElementData<float,DATA_COMP>&>(s);
			for (int i=0; i<NE; ++i) df.add(dom.elem[i].index, ne, &a[i*ne]);
		}
		break;
	case VEC3F:
		{
			vector<vec3f> a(nsize), d(NE);
			m_ar.read(a);

			FEElementData<vec3f,DATA_COMP>& df = dynamic_cast<FEElementData<vec3f,DATA_COMP>&>(s);
			for (int i=0; i<NE; ++i) df.add(dom.elem[i].index, ne, &a[i*ne]);
		};
		break;
	case MAT3FS:
		{
			vector<mat3fs> a(nsize), d(NE);
			m_ar.read(a);

			FEElementData<mat3fs,DATA_COMP>& df = dynamic_cast<FEElementData<mat3fs,DATA_COMP>&>(s);
			for (int i=0; i<NE; ++i) df.add(dom.elem[i].index, ne, &a[i*ne]);
		};
		break;
	case MAT3FD:
		{
			vector<mat3fd> a(nsize), d(NE);
			m_ar.read(a);

			FEElementData<mat3fd,DATA_COMP>& df = dynamic_cast<FEElementData<mat3fd,DATA_COMP>&>(s);
			for (int i=0; i<NE; ++i) df.add(dom.elem[i].index, ne, &a[i*ne]);
		};
		break;
	case MAT3F:
		{
			vector<mat3f> a(nsize), d(NE);
			m_ar.read(a);

			FEElementData<mat3f,DATA_COMP>& df = dynamic_cast<FEElementData<mat3f,DATA_COMP>&>(s);
			for (int i=0; i<NE; ++i) df.add(dom.elem[i].index, ne, &a[i*ne]);
		};
		break;
    case TENS4FS:
		{
			vector<tens4fs> a(nsize), d(NE);
			m_ar.read(a);
            
			FEElementData<tens4fs,DATA_COMP>& df = dynamic_cast<FEElementData<tens4fs,DATA_COMP>&>(s);
			for (int i=0; i<NE; ++i) df.add(dom.elem[i].index, ne, &a[i*ne]);
		};
        break;
	default:
		assert(false);
		return errf("Error while reading element data");
	}

	return true;
}

//-----------------------------------------------------------------------------
bool XpltReader::ReadElemData_REGION(XpltReader::Domain& dom, FEMeshData& s, int ntype)
{
	int NE = dom.ne;
	switch (ntype)
	{
	case FLOAT:
		{
			float a;
			m_ar.read(a);
			FEElementData<float,DATA_REGION>& df = dynamic_cast<FEElementData<float,DATA_REGION>&>(s);
			df.set(a);
			for (int i=0; i<NE; ++i) df.add(dom.elem[i].index);
		}
		break;
	case VEC3F:
		{
			vec3f a;
			m_ar.read(a);
			FEElementData<vec3f,DATA_REGION>& df = dynamic_cast<FEElementData<vec3f,DATA_REGION>&>(s);
			df.set(a);
			for (int i=0; i<NE; ++i) df.add(dom.elem[i].index);
		};
		break;
	case MAT3FS:
		{
			mat3fs a;
			m_ar.read(a);
			FEElementData<mat3fs,DATA_REGION>& df = dynamic_cast<FEElementData<mat3fs,DATA_REGION>&>(s);
			df.set(a);
			for (int i=0; i<NE; ++i) df.add(dom.elem[i].index);
		};
		break;
	case MAT3FD:
		{
			mat3fd a;
			m_ar.read(a);
			FEElementData<mat3fd,DATA_REGION>& df = dynamic_cast<FEElementData<mat3fd,DATA_REGION>&>(s);
			df.set(a);
			for (int i=0; i<NE; ++i) df.add(dom.elem[i].index);
		};
		break;
	case MAT3F:
		{
			mat3f a;
			m_ar.read(a);
			FEElementData<mat3f,DATA_REGION>& df = dynamic_cast<FEElementData<mat3f,DATA_REGION>&>(s);
			df.set(a);
			for (int i=0; i<NE; ++i) df.add(dom.elem[i].index);
		};
		break;
    case TENS4FS:
		{
			tens4fs a;
			m_ar.read(a);
			FEElementData<tens4fs,DATA_REGION>& df = dynamic_cast<FEElementData<tens4fs,DATA_REGION>&>(s);
			df.set(a);
			for (int i=0; i<NE; ++i) df.add(dom.elem[i].index);
		};
        break;
	default:
		assert(false);
		return errf("Error while reading element data");
	}

	return true;
}

//-----------------------------------------------------------------------------
bool XpltReader::ReadFaceData(FEModel& fem, FEState* pstate)
{
	FEMesh& m = *fem.GetMesh();
	FEDataManager& dm = *fem.GetDataManager();
	while (m_ar.OpenChunk() == IO_OK)
	{
		if (m_ar.GetChunkID() == PLT_STATE_VARIABLE)
		{
			int nv = -1;
			while (m_ar.OpenChunk() == IO_OK)
			{
				int nid = m_ar.GetChunkID();
				if (nid == PLT_STATE_VAR_ID) m_ar.read(nv);
				else if (nid ==	PLT_STATE_VAR_DATA)
				{
					nv--;
					assert((nv>=0)&&(nv<(int)m_dic.m_Face.size()));
					if ((nv < 0) || (nv >= (int)m_dic.m_Face.size())) return errf("Failed reading all state data");
					DICT_ITEM it = m_dic.m_Face[nv];
					while (m_ar.OpenChunk() == IO_OK)
					{
						int ns = m_ar.GetChunkID() - 1;
						assert((ns >= 0)&&(ns < (int)m_Surf.size()));
						if ((ns < 0) || (ns >= (int)m_Surf.size())) return errf("Failed reading all state data");

						int nfield = dm.FindDataField(it.szname);

						Surface& s = m_Surf[ns];
						switch (it.nfmt)
						{
						case FMT_NODE  : if (ReadFaceData_NODE  (m, s, pstate->m_Data[nfield], it.ntype) == false) return errf("Failed reading face data"); break;
						case FMT_ITEM  : if (ReadFaceData_ITEM  (s, pstate->m_Data[nfield], it.ntype   ) == false) return errf("Failed reading face data"); break;
						case FMT_MULT  : if (ReadFaceData_MULT  (m, s, pstate->m_Data[nfield], it.ntype) == false) return errf("Failed reading face data"); break;
						case FMT_REGION: if (ReadFaceData_REGION(m, s, pstate->m_Data[nfield], it.ntype) == false) return errf("Failed reading face data"); break;
						default:
							return errf("Failed reading face data");
						}
						m_ar.CloseChunk();
					}
				}
				else
				{
					return errf("Failed reading face data");
				}
				m_ar.CloseChunk();
			}
		}
		else 
		{
			return errf("Failed reading face data");
		}
		m_ar.CloseChunk();
	}
	return true;
}

//-----------------------------------------------------------------------------
bool XpltReader::ReadFaceData_MULT(FEMesh& m, XpltReader::Surface &s, FEMeshData &data, int ntype)
{
	// It is possible that the node ordering of the FACE's are different than the FEFace's
	// so we setup up an array to unscramble the nodal values
	int NF = s.nf;
	vector<int> tag;
	tag.assign(m.Nodes(), -1);
	const int NFM = m_hdr.nmax_facet_nodes;
	vector<vector<int> > l(NF, vector<int>(NFM));
	for (int i=0; i<NF; ++i)
	{
		FACE& f = s.face[i];
		FEFace& fm = m.Face(f.nid);
		for (int j=0; j<f.nn; ++j) tag[f.node[j]] = j;
		for (int j=0; j<f.nn; ++j) l[i][j] = tag[fm.node[j]];
	}

	switch (ntype)
	{
	case FLOAT:
		{
			FEFaceData<float,DATA_COMP>& df = dynamic_cast<FEFaceData<float,DATA_COMP>&>(data);
			vector<float> a(NFM*NF);
			m_ar.read(a);
			float v[9];
			for (int i=0; i<NF; ++i)
			{
				FACE& f = s.face[i];
				vector<int>& li = l[i];
				for (int j=0; j<f.nn; ++j) v[j] = a[NFM*i + li[j]];
				df.add(f.nid, v, f.nn);
			}
		}
		break;
	case VEC3F:
		{
			FEFaceData<vec3f,DATA_COMP>& df = dynamic_cast<FEFaceData<vec3f,DATA_COMP>&>(data);
			vector<vec3f> a(NFM*NF);
			m_ar.read(a);
			vec3f v[9];
			for (int i=0; i<NF; ++i)
			{
				FACE& f = s.face[i];
				vector<int>& li = l[i];
				for (int j=0; j<f.nn; ++j) v[j] = a[NFM*i + li[j]];
				df.add(f.nid, v, f.nn);
			}
		}
		break;
	case MAT3FS:
		{
			FEFaceData<mat3fs,DATA_COMP>& df = dynamic_cast<FEFaceData<mat3fs,DATA_COMP>&>(data);
			vector<mat3fs> a(4*NF);
			m_ar.read(a);
			mat3fs v[9];
			for (int i=0; i<NF; ++i)
			{
				FACE& f = s.face[i];
				vector<int>& li = l[i];
				for (int j=0; j<f.nn; ++j) v[j] = a[NFM*i + li[j]];
				df.add(f.nid, v, f.nn);
			}
		}
		break;
	case MAT3F:
		{
			FEFaceData<mat3f,DATA_COMP>& df = dynamic_cast<FEFaceData<mat3f,DATA_COMP>&>(data);
			vector<mat3f> a(4*NF);
			m_ar.read(a);
			mat3f v[9];
			for (int i=0; i<NF; ++i)
			{
				FACE& f = s.face[i];
				vector<int>& li = l[i];
				for (int j=0; j<f.nn; ++j) v[j] = a[NFM*i + li[j]];
				df.add(f.nid, v, f.nn);
			}
		}
		break;
	case MAT3FD:
		{
			FEFaceData<mat3fd,DATA_COMP>& df = dynamic_cast<FEFaceData<mat3fd,DATA_COMP>&>(data);
			vector<mat3fd> a(4*NF);
			m_ar.read(a);
			mat3fd v[9];
			for (int i=0; i<NF; ++i)
			{
				FACE& f = s.face[i];
				vector<int>& li = l[i];
				for (int j=0; j<f.nn; ++j) v[j] = a[NFM*i + li[j]];
				df.add(f.nid, v, f.nn);
			}
		}
		break;	
    case TENS4FS:
		{
			FEFaceData<tens4fs,DATA_COMP>& df = dynamic_cast<FEFaceData<tens4fs,DATA_COMP>&>(data);
			vector<tens4fs> a(4*NF);
			m_ar.read(a);
			tens4fs v[9];
			for (int i=0; i<NF; ++i)
			{
				FACE& f = s.face[i];
				vector<int>& li = l[i];
				for (int j=0; j<f.nn; ++j) v[j] = a[NFM*i + li[j]];
				df.add(f.nid, v, f.nn);
			}
		}
        break;
	default:
		return errf("Failed reading face data");
	}

	return true;
}

//-----------------------------------------------------------------------------
bool XpltReader::ReadFaceData_REGION(FEMesh& m, XpltReader::Surface &s, FEMeshData &data, int ntype)
{
	int NF = s.nf;
	switch (ntype)
	{
	case FLOAT:
		{
			FEFaceData<float,DATA_REGION>& df = dynamic_cast<FEFaceData<float,DATA_REGION>&>(data);
			float a;
			m_ar.read(a);
			df.set(a);
			for (int i=0; i<NF; ++i)
			{
				FACE& f = s.face[i];
				df.add(f.nid);
			}
		}
		break;
	case VEC3F:
		{
			FEFaceData<vec3f,DATA_REGION>& df = dynamic_cast<FEFaceData<vec3f,DATA_REGION>&>(data);
			vec3f a;
			m_ar.read(a);
			df.set(a);
			for (int i=0; i<NF; ++i)
			{
				FACE& f = s.face[i];
				df.add(f.nid);
			}
		}
		break;
	case MAT3FS:
		{
			FEFaceData<mat3fs,DATA_REGION>& df = dynamic_cast<FEFaceData<mat3fs,DATA_REGION>&>(data);
			mat3fs a;
			m_ar.read(a);
			df.set(a);
			for (int i=0; i<NF; ++i)
			{
				FACE& f = s.face[i];
				df.add(f.nid);
			}
		}
		break;
	case MAT3F:
		{
			FEFaceData<mat3f,DATA_REGION>& df = dynamic_cast<FEFaceData<mat3f,DATA_REGION>&>(data);
			mat3f a;
			m_ar.read(a);
			df.set(a);
			for (int i=0; i<NF; ++i)
			{
				FACE& f = s.face[i];
				df.add(f.nid);
			}
		}
		break;
	case MAT3FD:
		{
			FEFaceData<mat3fd,DATA_REGION>& df = dynamic_cast<FEFaceData<mat3fd,DATA_REGION>&>(data);
			mat3fd a;
			m_ar.read(a);
			df.set(a);
			for (int i=0; i<NF; ++i)
			{
				FACE& f = s.face[i];
				df.add(f.nid);
			}
		}
		break;	
    case TENS4FS:
		{
			FEFaceData<tens4fs,DATA_REGION>& df = dynamic_cast<FEFaceData<tens4fs,DATA_REGION>&>(data);
			tens4fs a;
			m_ar.read(a);
			df.set(a);
			for (int i=0; i<NF; ++i)
			{
				FACE& f = s.face[i];
				df.add(f.nid);
			}
		}
        break;
	default:
		return errf("Failed reading face data");
	}

	return true;
}

//-----------------------------------------------------------------------------
bool XpltReader::ReadFaceData_ITEM(XpltReader::Surface &s, FEMeshData &data, int ntype)
{
	int NF = s.nf;
	switch (ntype)
	{
	case FLOAT:
		{
			FEFaceData<float,DATA_ITEM>& df = dynamic_cast<FEFaceData<float,DATA_ITEM>&>(data);
			vector<float> a(NF);
			m_ar.read(a);
			for (int i=0; i<NF; ++i) df.add(s.face[i].nid, a[i]);
		}
		break;
	case VEC3F:
		{
			vector<vec3f> a(NF);
			m_ar.read(a);
			FEFaceData<vec3f,DATA_ITEM>& dv = dynamic_cast<FEFaceData<vec3f,DATA_ITEM>&>(data);
			for (int i=0; i<NF; ++i) dv.add(s.face[i].nid, a[i]);
		}
		break;
	case MAT3FS:
		{
			vector<mat3fs> a(NF);
			m_ar.read(a);
			FEFaceData<mat3fs,DATA_ITEM>& dm = dynamic_cast<FEFaceData<mat3fs,DATA_ITEM>&>(data);
			for (int i=0; i<NF; ++i) dm.add(s.face[i].nid, a[i]);
		}
		break;
	case MAT3F:
		{
			vector<mat3f> a(NF);
			m_ar.read(a);
			FEFaceData<mat3f,DATA_ITEM>& dm = dynamic_cast<FEFaceData<mat3f,DATA_ITEM>&>(data);
			for (int i=0; i<NF; ++i) dm.add(s.face[i].nid, a[i]);
		}
		break;
	case MAT3FD:
		{
			vector<mat3fd> a(NF);
			m_ar.read(a);
			FEFaceData<mat3fd,DATA_ITEM>& dm = dynamic_cast<FEFaceData<mat3fd,DATA_ITEM>&>(data);
			for (int i=0; i<NF; ++i) dm.add(s.face[i].nid, a[i]);
		}
		break;
    case TENS4FS:
		{
			vector<tens4fs> a(NF);
			m_ar.read(a);
			FEFaceData<tens4fs,DATA_ITEM>& dm = dynamic_cast<FEFaceData<tens4fs,DATA_ITEM>&>(data);
			for (int i=0; i<NF; ++i) dm.add(s.face[i].nid, a[i]);
		}
        break;
	default:
		return errf("Failed reading face data");
	}

	return true;
}

//-----------------------------------------------------------------------------
bool XpltReader::ReadFaceData_NODE(FEMesh& m, XpltReader::Surface &s, FEMeshData &data, int ntype)
{
	// set nodal tags to local node number
	int NN = m.Nodes();
	for (int i=0; i<NN; ++i) m.Node(i).m_ntag = -1;

	int n = 0;
	for (int i=0; i<s.nf; ++i)
	{
		FACE& f = s.face[i];
		int nf = f.nn;
		for (int j=0; j<nf; ++j)
			if (m.Node(f.node[j]).m_ntag == -1) m.Node(f.node[j]).m_ntag = n++;
	}

	// create the face list
	vector<int> f(s.nf);
	for (int i=0; i<s.nf; ++i) f[i] = s.face[i].nid;

	// create the local node index list
	vector<int> l(4*s.nf);
	for (int i=0; i<s.nf; ++i)
	{
		FEFace& f = m.Face(s.face[i].nid);
		l[4*i  ] = m.Node(f.node[0]).m_ntag; assert(l[4*i  ] >= 0);
		l[4*i+1] = m.Node(f.node[1]).m_ntag; assert(l[4*i+1] >= 0);
		l[4*i+2] = m.Node(f.node[2]).m_ntag; assert(l[4*i+2] >= 0);
		l[4*i+3] = m.Node(f.node[2]).m_ntag; assert(l[4*i+3] >= 0);
	}

	// get the data
	switch (ntype)
	{
	case FLOAT:
		{
			FEFaceData<float,DATA_NODE>& df = dynamic_cast<FEFaceData<float,DATA_NODE>&>(data);
			vector<float> a(n);
			m_ar.read(a);
			df.add(a, f, l);
		}
		break;
	case VEC3F:
		{
			FEFaceData<vec3f,DATA_NODE>& df = dynamic_cast<FEFaceData<vec3f,DATA_NODE>&>(data);
			vector<vec3f> a(n);
			m_ar.read(a);
			df.add(a, f, l);
		}
		break;
	case MAT3FS:
		{
			FEFaceData<mat3fs,DATA_NODE>& df = dynamic_cast<FEFaceData<mat3fs,DATA_NODE>&>(data);
			vector<mat3fs> a(n);
			m_ar.read(a);
			df.add(a, f, l);
		}
		break;
	case MAT3F:
		{
			FEFaceData<mat3f,DATA_NODE>& df = dynamic_cast<FEFaceData<mat3f,DATA_NODE>&>(data);
			vector<mat3f> a(n);
			m_ar.read(a);
			df.add(a, f, l);
		}
		break;
	case MAT3FD:
		{
			FEFaceData<mat3fd,DATA_NODE>& df = dynamic_cast<FEFaceData<mat3fd,DATA_NODE>&>(data);
			vector<mat3fd> a(n);
			m_ar.read(a);
			df.add(a, f, l);
		}
		break;
    case TENS4FS:
		{
			FEFaceData<tens4fs,DATA_NODE>& df = dynamic_cast<FEFaceData<tens4fs,DATA_NODE>&>(data);
			vector<tens4fs> a(n);
			m_ar.read(a);
			df.add(a, f, l);
		}
        break;
	default:
		return errf("Failed reading face data");
	}

	return true;
}
