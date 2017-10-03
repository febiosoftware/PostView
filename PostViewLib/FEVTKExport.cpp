#include "FEVTKExport.h"
#include <stdio.h>
#include "FEModel.h"
#include "FEMeshData_T.h"

#define VTK_VERTEX                  1
#define VTK_POLY_VERTEX             2
#define VTK_LINE                    3
#define VTK_POLY_LINE               4
#define VTK_TRIANGLE                5
#define VTK_TRIANGLE_STRIP          6
#define VTK_POLYGON                 7
#define VTK_PIXEL                   8
#define VTK_QUAD                    9
#define VTK_TETRA                   10
#define VTK_VOXEL                   11
#define VTK_HEXAHEDRON              12
#define VTK_WEDGE                   13
#define VTK_PYRAMID                 14
#define VTK_QUADRATIC_EDGE          21
#define VTK_QUADRATIC_TRIANGLE      22
#define VTK_QUADRATIC_QUAD          23
#define VTK_QUADRATIC_TETRA         24
#define VTK_QUADRATIC_HEXAHEDRON    25
#define VTK_QUADRATIC_WEDGE         26

FEVTKExport::FEVTKExport(void)
{
}

FEVTKExport::~FEVTKExport(void)
{
}

bool FEVTKExport::Save(FEModel& fem, const char* szfile)
{
    FEState* ps;
    int ns = fem.GetStates();
    if (ns == 0) return false;
    
    char szroot[256], szname[256], szext[16];
    const char* sz;
    sz = strrchr(szfile, '.');
    if (sz == 0) {
        strcpy(szroot, szfile);
        strcat(szroot,".");
        strcpy(szext,".vtk");
    }
    else {
        strncpy(szroot, szfile,sz-szfile+1);
        strcpy(szext, sz);
    }
    
    FEMeshBase* pm = fem.GetFEMesh(0);
    if (pm == 0) return false;
    
    FEMeshBase& m = *pm;
    
    int nodes = m.Nodes();
    
    // save each state in a separate file
    int l0 = (int) log10((double)ns) + 1;
    for (int is=0; is<ns; ++is) {
        ps = fem.GetState(is);
        if (sprintf(szname, "%st%0*d%s",szroot,l0,is,szext) < 0) return false;
        
        FILE* fp = fopen(szname, "wt");
        if (fp == 0) return false;
        
        // --- H E A D E R ---
        fprintf(fp, "%s\n"       ,"# vtk DataFile Version 3.0");
        fprintf(fp, "%s %g\n"    ,"vtk output at time",ps->m_time);
        fprintf(fp, "%s\n"       ,"ASCII");
        fprintf(fp, "%s\n"       ,"DATASET UNSTRUCTURED_GRID");
        fprintf(fp, "%s %d %s\n" ,"POINTS", nodes ,"float");
        
        // --- N O D E S ---
        for (int j=0; j<nodes; )
        {
            for (int k =0; k<3 && j+k<nodes;k++)
            {
                vec3f& r = ps->m_NODE[j+k].m_rt;
                fprintf(fp, "%g %g %g ", r.x, r.y, r.z);
            }
            fprintf(fp, "\n");
            j = j + 3;
        }
        fprintf(fp, "%s\n" ,"");
        
        // --- E L E M E N T S ---
        int nsize = 0;
		for (int j = 0; j<m.Elements(); ++j)
            nsize += m.Element(j).Nodes() + 1;
        
        fprintf(fp, "%s %d %d\n" ,"CELLS", m.Elements(), nsize);
        
        for (int j=0; j<m.Elements(); ++j)
        {
            FEElement& el = m.Element(j);
            fprintf(fp, "%d ", el.Nodes());
            for (int k=0; k<el.Nodes(); ++k) fprintf(fp, "%d ", el.m_node[k]);
            fprintf(fp, "\n");
        }
        
        fprintf(fp, "\n%s %d\n" ,"CELL_TYPES", m.Elements());
		for (int j = 0; j<m.Elements(); ++j)
        {
            FEElement& el = m.Element(j);
            int vtk_type;
            switch (el.Type()) {
                case FE_HEX8  : vtk_type = VTK_HEXAHEDRON; break;
                case FE_TET4  : vtk_type = VTK_TETRA; break;
                case FE_PENTA6: vtk_type = VTK_WEDGE; break;
                case FE_QUAD4 : vtk_type = VTK_QUAD; break;
                case FE_TRI3  : vtk_type = VTK_TRIANGLE; break;
                case FE_LINE2: vtk_type = VTK_LINE; break;
                case FE_HEX20 : vtk_type = VTK_QUADRATIC_HEXAHEDRON; break;
                case FE_QUAD8 : vtk_type = VTK_QUADRATIC_QUAD; break;
                case FE_LINE3: vtk_type = VTK_QUADRATIC_EDGE; break;
                case FE_TET10 : vtk_type = VTK_QUADRATIC_TETRA; break;
                case FE_TET15 : vtk_type = VTK_QUADRATIC_TETRA; break;
                case FE_PENTA15: vtk_type = VTK_QUADRATIC_WEDGE; break;
                case FE_HEX27 : vtk_type = VTK_QUADRATIC_HEXAHEDRON; break;
                case FE_TRI6  : vtk_type = VTK_QUADRATIC_TRIANGLE; break;
                case FE_QUAD9 : vtk_type = VTK_QUADRATIC_QUAD; break;
                default: vtk_type = -1; break;
            }
            
            fprintf(fp, "%d\n", vtk_type);
        }
        
        // --- N O D E   D A T A ---
        {
            FEDataManager& DM = *fem.GetDataManager();
            FEDataFieldPtr pd = DM.FirstDataField();
            
            int NDATA = ps->m_Data.size();
            if (NDATA > 0) fprintf(fp, "\n%s %d\n" ,"POINT_DATA",nodes);
            
            for (int n=0; n<NDATA; ++n, ++pd)
            {
                FEDataField& data = *(*pd);
                if ((data.DataClass() == CLASS_NODE) && (data.Flags() & EXPORT_DATA))
                {
                    FEMeshData& meshData = ps->m_Data[n];
                    char szname[256];
                    strcpy(szname, data.GetName().c_str());
                    Space2_(szname);
                    
                    // value array
                    vector<float> val;
                    if (FillNodeDataArray(val, meshData) == false) return false;
                    
                    // write the value array
                    if (val.empty() == false) {
                        int ntype = meshData.GetType();
                        if (ntype == DATA_FLOAT) {
                            fprintf(fp, "%s %s %s\n" ,"SCALARS",szname,"float");
                            fprintf(fp, "%s %s\n","LOOKUP_TABLE","default");
                            for (int i=0; i<val.size(); ++i) fprintf(fp,"%g\n",val[i]);
                        }
                        else if (ntype == DATA_VEC3F) {
                            fprintf(fp, "%s %s %s\n" ,"VECTORS",szname,"float");
                            for (int i=0; i<val.size(); i+=3) fprintf(fp,"%g %g %g\n",val[i],val[i+1],val[i+2]);
                        }
                        else if (ntype == DATA_MAT3FS) {
                            fprintf(fp, "%s %s %s\n" ,"TENSORS",szname,"float");
                            for (int i=0; i<val.size(); i+=6)
                                fprintf(fp,"%g %g %g\n%g %g %g\n%g %g %g\n\n",
                                        val[i  ],val[i+3],val[i+5],
                                        val[i+3],val[i+1],val[i+4],
                                        val[i+5],val[i+4],val[i+2]);
                        }
                        else if (ntype == DATA_MAT3FD) {
                            fprintf(fp, "%s %s %s\n" ,"TENSORS",szname,"float");
                            for (int i=0; i<val.size(); i+=3)
                                fprintf(fp,"%g %g %g\n%g %g %g\n%g %g %g\n\n",
                                        val[i  ],0.f,0.f,
                                        0.f,val[i+1],0.f,
                                        0.f,0.f,val[i+2]);
                        }
                    }
                }
            }
        }
        
        // --- E L E M E N T   P O I N T   D A T A ---
        {
            FEDataManager& DM = *fem.GetDataManager();
            FEDataFieldPtr pd = DM.FirstDataField();
            
            int NDATA = ps->m_Data.size();
            
            for (int n=0; n<NDATA; ++n, ++pd)
            {
                FEDataField& data = *(*pd);
                if ((data.DataClass() == CLASS_ELEM) && (data.Flags() & EXPORT_DATA))
                {
                    FEMeshData& meshData = ps->m_Data[n];
                    Data_Format dfmt = meshData.GetFormat();
                    if (dfmt == DATA_NODE) {
                        FEDataField& data = *(*pd);
                        char szname[256];
						strcpy(szname, data.GetName().c_str());
                        Space2_(szname);
                        
                        // value array
                        vector<float> val;
                        
                        int ND = m.Parts();
                        for (int i=0; i<ND; ++i)
                        {
                            FEPart& part = m.Part(i);
                            
                            if (FillElemDataArray(val, meshData, part) == false) return false;
                            
                            // write the value array
                            if (val.empty() == false) {
                                int ntype = meshData.GetType();
                                if (ntype == DATA_FLOAT) {
                                    if (i==0) {
                                        fprintf(fp, "%s %s %s\n" ,"SCALARS",szname,"float");
                                        fprintf(fp, "%s %s\n","LOOKUP_TABLE","default");
                                    }
                                    for (int i=0; i<val.size(); ++i) fprintf(fp,"%g\n",val[i]);
                                }
                                else if (ntype == DATA_VEC3F) {
                                    if (i==0) {
                                        fprintf(fp, "%s %s %s\n" ,"VECTORS",szname,"float");
                                    }
                                    for (int i=0; i<val.size(); i+=3) fprintf(fp,"%g %g %g\n",val[i],val[i+1],val[i+2]);
                                }
                                else if (ntype == DATA_MAT3FS) {
                                    if (i==0) {
                                        fprintf(fp, "%s %s %s\n" ,"TENSORS",szname,"float");
                                    }
                                    for (int i=0; i<val.size(); i+=6)
                                        fprintf(fp,"%g %g %g\n%g %g %g\n%g %g %g\n\n",
                                                val[i  ],val[i+3],val[i+5],
                                                val[i+3],val[i+1],val[i+4],
                                                val[i+5],val[i+4],val[i+2]);
                                }
                                else if (ntype == DATA_MAT3FD) {
                                    if (i==0) {
                                        fprintf(fp, "%s %s %s\n" ,"TENSORS",szname,"float");
                                    }
                                    for (int i=0; i<val.size(); i+=3)
                                        fprintf(fp,"%g %g %g\n%g %g %g\n%g %g %g\n\n",
                                                val[i  ],0.f,0.f,
                                                0.f,val[i+1],0.f,
                                                0.f,0.f,val[i+2]);
                                }
                            }
                        }
                    }
                }
            }
        }
        
        // --- E L E M E N T   C E L L   D A T A ---
        {
            FEDataManager& DM = *fem.GetDataManager();
            FEDataFieldPtr pd = DM.FirstDataField();
            
            int NDATA = ps->m_Data.size();
            if (NDATA > 0) fprintf(fp, "\n%s %d\n" ,"CELL_DATA",m.Elements());
            
            for (int n=0; n<NDATA; ++n, ++pd)
            {
                FEDataField& data = *(*pd);
                if ((data.DataClass() == CLASS_ELEM) && (data.Flags() & EXPORT_DATA))
                {
                    FEMeshData& meshData = ps->m_Data[n];
                    Data_Format dfmt = meshData.GetFormat();
                    if (dfmt == DATA_ITEM) {
                        FEDataField& data = *(*pd);
                        char szname[256];
						strcpy(szname, data.GetName().c_str());
                        Space2_(szname);
                        
                        // value array
                        vector<float> val;
                        
                        int ND = m.Parts();
                        for (int i=0; i<ND; ++i)
                        {
                            FEPart& part = m.Part(i);
                            
                            if (FillElemDataArray(val, meshData, part) == false) return false;
                            
                            // write the value array
                            if (val.empty() == false) {
                                int ntype = meshData.GetType();
                                if (ntype == DATA_FLOAT) {
                                    if (i==0) {
                                        fprintf(fp, "%s %s %s\n" ,"SCALARS",szname,"float");
                                        fprintf(fp, "%s %s\n","LOOKUP_TABLE","default");
                                    }
                                    for (int i=0; i<val.size(); ++i) fprintf(fp,"%g\n",val[i]);
                                }
                                else if (ntype == DATA_VEC3F) {
                                    if (i==0) {
                                        fprintf(fp, "%s %s %s\n" ,"VECTORS",szname,"float");
                                    }
                                    for (int i=0; i<val.size(); i+=3) fprintf(fp,"%g %g %g\n",val[i],val[i+1],val[i+2]);
                                }
                                else if (ntype == DATA_MAT3FS) {
                                    if (i==0) {
                                        fprintf(fp, "%s %s %s\n" ,"TENSORS",szname,"float");
                                    }
                                    for (int i=0; i<val.size(); i+=6)
                                        fprintf(fp,"%g %g %g\n%g %g %g\n%g %g %g\n\n",
                                                val[i  ],val[i+3],val[i+5],
                                                val[i+3],val[i+1],val[i+4],
                                                val[i+5],val[i+4],val[i+2]);
                                }
                                else if (ntype == DATA_MAT3FD) {
                                    if (i==0) {
                                        fprintf(fp, "%s %s %s\n" ,"TENSORS",szname,"float");
                                    }
                                    for (int i=0; i<val.size(); i+=3)
                                        fprintf(fp,"%g %g %g\n%g %g %g\n%g %g %g\n\n",
                                                val[i  ],0.f,0.f,
                                                0.f,val[i+1],0.f,
                                                0.f,0.f,val[i+2]);
                                }
                            }
                        }
                    }
                }
            }
        }
        
        fclose(fp);
    }
    
    return true;
}

void FEVTKExport::Space2_(char* szname)
{
    int n = (int)strlen(szname);
    for (int i=0; i<n; ++i)
        if (szname[i] == ' ') szname[i] = '_';
}
