#include "stdafx.h"
#include "FEVTKImport.h"
#include "FEMeshData_T.h"

FEVTKimport::FEVTKimport() : FEFileReader("VTK")
{
}

FEVTKimport::~FEVTKimport(void)
{
}

bool FEVTKimport::Load(FEModel& fem, const char* szfile)
{
	fem.Clear();
	m_pfem = &fem;

	// add one material to the scene
	FEMaterial mat;
	m_pfem->AddMaterial(mat);

	if (!Open(szfile, "rt")) return errf("Failed opening file %s.", szfile);

	char szline[256] = {0}, *ch;
	int nread, i,j,k;
	char type[256];
	double temp[9];
	bool isASCII=false, isPOLYDATA = false, isUnstructuredGrid = false, isCellData = false,isScalar=false,isShellThickness=false;
	// read the header
	do
	{
		ch = fgets(szline, 255, m_fp);
		if (ch == 0) return errf("An unexpected error occured while reading the file data.");
		if (strstr(ch, "ASCII") != 0) isASCII = true;
		if (strstr(ch, "POLYDATA") != 0) isPOLYDATA = true;
		if (strstr(ch, "UNSTRUCTURED_GRID") != 0) isUnstructuredGrid = true;
	}
	while(strstr(ch, "POINTS") == 0);
	if (!isASCII)
		return errf("Only ASCII files are read");

	//get number of nodes
	int nodes = atoi(ch+6);
	
	int size = 0;
	int nparts = 0;
	int edges = 0;
	int elems = 0;
	if (nodes <= 0) return errf("Invalid number of nodes.");
	

	// create a new mesh
	FEMesh* pm = new FEMesh;
	fem.AddMesh(pm);
	pm->Create(nodes, 0);
	
	// read the nodes
	//Check how many nodes are there in each line
	ch = fgets(szline, 255, m_fp);
	if (ch == 0) return errf("An unexpected error occured while reading the file data.");

	nread = sscanf(szline, "%lg%lg%lg%lg%lg%lg%lg%lg%lg%s", &temp[0],&temp[1],&temp[2], &temp[3],&temp[4],&temp[5], &temp[6],&temp[7],&temp[8]);
	if (nread%3 != 0 && nread>9) 
		return errf("An error occured while reading the nodal coordinates.");
	int nodes_each_row = nread/3;
	double temp2 = double(nodes)/nodes_each_row;
	int rows = (int)ceil(temp2);
	for (i=0; i<rows; ++i)
	{	
		for (j=0,k=0;j<nodes_each_row && i*nodes_each_row+j <nodes;j++)
		{
			FENode& n= pm->Node(i*nodes_each_row+j);
			vec3f r;
			r.x = temp[k];
			r.y = temp[k+1];
			r.z = temp[k+2];
			n.m_r0 = n.m_rt = r;
			k +=3;
		}
		ch = fgets(szline, 255, m_fp);
		if (ch == 0) return errf("An unexpected error occured while reading the file data.");

		nread = sscanf(szline, "%lg%lg%lg%lg%lg%lg%lg%lg%lg%s", &temp[0],&temp[1],&temp[2], &temp[3],&temp[4],&temp[5], &temp[6],&temp[7],&temp[8]);
		if (nread%3 != 0 && nread != -1)
		{ 			
			if (i+1 != nodes/nodes_each_row)
				return errf("An error occured while reading the nodal coordinates.");
		}
	}
	//Reading element data
	while(1)
	{		
		if (ch == 0) return errf("An unexpected error occured while reading the file data.");
		if (strstr(ch, "POLYGONS") != 0 || strstr(ch, "CELLS") != 0 ) 
		{
			sscanf(szline, "%s %d %d",type,&elems,&size);
			break;
		}
		ch = fgets(szline, 255, m_fp);
	}

	if(elems == 0||size == 0)
		return errf("Only POLYGON/CELL dataset format is supported.");

	pm->Create(0, elems);

	// read the elements
	int n[9];
	for (i=0; i<elems; ++i)
	{	
		FEGenericElement& el = static_cast<FEGenericElement&>(pm->Element(i));
		ch = fgets(szline, 255, m_fp);
		if (ch == 0) return errf("An unexpected error occured while reading the file data.");
		nread = sscanf(szline, "%d%d%d%d%d%d%d%d%d", &n[0], &n[1], &n[2], &n[3], &n[4],&n[5],&n[6],&n[7],&n[8]);
		int min = 0;
		switch (n[0])
		{
		case 3: 
			el.SetType(FE_TRI3);
			el.m_node[0] = n[1]-min;
			el.m_node[1] = n[2]-min;
			el.m_node[2] = n[3]-min;
			break;
		case 4:			
			if(isPOLYDATA)
				el.SetType(FE_QUAD4);
			if(isUnstructuredGrid)
				el.SetType(FE_TET4);
			el.m_node[0] = n[1]-min;
			el.m_node[1] = n[2]-min;
			el.m_node[2] = n[3]-min;
			el.m_node[3] = n[4]-min;
			break;
		case 8:
			el.SetType(FE_HEX8);
			el.m_node[0] = n[1]-min;
			el.m_node[1] = n[2]-min;
			el.m_node[2] = n[3]-min;
			el.m_node[3] = n[4]-min;
			el.m_node[4] = n[5]-min;
			el.m_node[5] = n[6]-min;
			el.m_node[6] = n[7]-min;
			el.m_node[7] = n[8]-min;
			break;
		default:
			delete pm;
			return errf("Only triangular, quadrilateral and hexahedron polygons are supported.");
		}
	}
	ch = fgets(szline, 255, m_fp);

	// add a state
	FEState* ps = new FEState(0.f, m_pfem, m_pfem->GetFEMesh(0));
	m_pfem->AddState(ps);

	while (ch != NULL) //making sure the file doesn't ends here
	{
		//reading the point data
		do
		{	
			if (ch == NULL) break;
			if (ch == 0) return errf("An unexpected error occured while reading the file data.");
			if (strstr(ch, "POINT_DATA") != 0)
				size = atoi(ch+10);
			if(strstr(ch,"CELL_DATA")!=0)
			{
				size = atoi(ch + 9);
				isCellData = true;
			}
			if (strstr(ch, "SCALARS") != 0) isScalar = true;
			if (strstr(ch, "ShellThickness") != 0) isShellThickness = true;
			ch = fgets(szline, 255, m_fp);
		}
		while(ch == NULL || strstr(ch, "LOOKUP_TABLE") == 0);

		if(!isScalar && ch !=NULL)
			return errf("Only scalar data is supported.");	
		vector<double> data; 
		//reading shell thickness
		if(isShellThickness)
		{			
			data.reserve(size);
			//Check how many nodes are there in each line
			ch = fgets(szline, 255, m_fp);
			if (ch == 0) return errf("An unexpected error occured while reading the file data.");

			nodes_each_row = sscanf(szline, "%lg%lg%lg%lg%lg%lg%lg%lg%lg%s", &temp[0],&temp[1],&temp[2], &temp[3],&temp[4],&temp[5], &temp[6],&temp[7],&temp[8]);
			if (nodes_each_row>9) 
				return errf("An error occured while reading the nodal coordinates.");
			double temp2 = double(size)/nodes_each_row;
			rows = ceil(temp2);
			for (i=0; i<rows; ++i)
			{	
				for (j=0;j<nodes_each_row && i*nodes_each_row+j <size;j++)
				{
					data.push_back(temp[j]);
				}
				ch = fgets(szline, 255, m_fp);
				if (ch == 0 && i+1 != rows) 
					return errf("An unexpected error occured while reading the scalar data.");

				nread = sscanf(szline, "%lg%lg%lg%lg%lg%lg%lg%lg%lg%s", &temp[0],&temp[1],&temp[2], &temp[3],&temp[4],&temp[5], &temp[6],&temp[7],&temp[8]);
				if (nread > 9 && nread != -1)
				{ 			
					if (i+1 != rows)
						return errf("An error occured while reading the scalar.");
				}
			}

			FEDataManager& dm = *fem.GetDataManager();
			dm.AddDataField(new FEDataField_T<FENodeData<float> >("data", EXPORT_DATA));

			FENodeData<float>& df = dynamic_cast<FENodeData<float>&>(ps->m_Data[0]);
			for (int j=0; j<pm->Nodes(); ++j) df[j] = (float) data[j];
		}

		//reading cell data
		if(isCellData)
		{
			FEDataManager& dm = *fem.GetDataManager();
			dm.AddDataField(new FEDataField_T<FENodeData<float> >("data", EXPORT_DATA));

			FEElementData<float, DATA_ITEM>& ed = dynamic_cast<FEElementData<float, DATA_ITEM>&>(ps->m_Data[0]);

			for (i=0; i<size; ++i)
			{
				FEElement& el = pm->Element(i);
				ch = fgets(szline, 255, m_fp);
				if (ch == 0) return errf("An unexpected error occured while reading the file data.");
				nread = sscanf(szline, "%lg", &temp[0]);
				ed[i] = (float) temp[0];
			}
		}		
		break;
	}

	Close();

	// update the mesh
	pm->Update();
	m_pfem->UpdateBoundingBox();

	return true;
}
