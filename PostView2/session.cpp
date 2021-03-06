/*This file is part of the PostView source code and is licensed under the MIT license
listed below.

See Copyright-PostView.txt for details.

Copyright (c) 2020 University of Utah, The Trustees of Columbia University in 
the City of New York, and others.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#include "stdafx.h"
#include "DocManager.h"
#include "Document.h"
#include "XML/XMLWriter.h"
#include "XML/XMLReader.h"
#include "PostLib/FEBioImport.h"
#include "PostLib/FELSDYNAPlot.h"
#include "XPLTLib/xpltFileReader.h"
#include "PostLib/FELSDYNAimport.h"
#include "PostLib/GMeshImport.h"
#include "PostLib/FENikeImport.h"
#include "PostLib/FESTLimport.h"
#include "PostLib/FEASCIIImport.h"
#include "PostLib/FEVTKImport.h"
#include "PostLib/FEU3DImport.h"
#include <PostGL/GLModel.h>
#include <PostGL/GLPlaneCutPlot.h>
#include <PostLib/ImageModel.h>
#include <ImageLib/3DImage.h>

//-----------------------------------------------------------------------------
// choose a file importer based on extension
Post::FEFileReader* GetFileReader(const char* szfile)
{
	Post::FEFileReader* pimp = 0;

	// get the extension
	const char* szext = strrchr(szfile, '.');

	if      (szext && (strcmp(szext, ".feb" ) == 0)) pimp = new Post::FEBioImport(nullptr);
	else if (szext && (strcmp(szext, ".plt" ) == 0)) pimp = new Post::FELSDYNAPlotImport(nullptr);
	else if (szext && (strcmp(szext, ".xplt") == 0)) pimp = new xpltFileReader(nullptr);
	else if (szext && (strcmp(szext, ".k"   ) == 0)) pimp = new Post::FELSDYNAimport(nullptr);
	else if (szext && (strcmp(szext, ".msh" ) == 0)) pimp = new Post::GMeshImport(nullptr);
	else if (szext && (strcmp(szext, ".gmsh") == 0)) pimp = new Post::GMeshImport(nullptr);
	else if (szext && (strcmp(szext, ".n"   ) == 0)) pimp = new Post::FENikeImport(nullptr);
	else if (szext && (strcmp(szext, ".stl" ) == 0)) pimp = new Post::FESTLimport(nullptr);
	else if (szext && (strcmp(szext, ".txt" ) == 0)) pimp = new Post::FEASCIIImport(nullptr);
	else if (szext && (strcmp(szext, ".vtk" ) == 0)) pimp = new Post::FEVTKimport(nullptr);
	else if (szext && (strcmp(szext, ".u3d" ) == 0)) pimp = new Post::FEU3DImport(nullptr);
	else if (szext == 0                            ) pimp = new Post::FELSDYNAPlotImport(nullptr);
	else assert(false);

	return pimp;
}

//-----------------------------------------------------------------------------
// Save the current session
bool CDocManager::SaveSession(const std::string& sfile)
{
	XMLWriter xml;
	if (xml.open(sfile.c_str()) == false) return false;
	xml.add_branch("postview_spec");
	{
		for (int i=0; i<Documents(); ++i)
		{
			CDocument* doc = GetDocument(i);

			XMLElement e;
			e.name("Model");

			// only store the filename
			const char* sz = doc->GetFile().c_str();
			e.add_attribute("file", sz);

			// store model data
			Post::CGLModel* pmdl = doc->GetGLModel();
			Post::FEPostModel* pfem = pmdl->GetFEModel();
			xml.add_branch(e);
			{
				int ntime = doc->currentTime();
				xml.add_leaf("state", ntime);

				xml.add_leaf("show_ghost", pmdl->m_bghost);
				xml.add_leaf("line_color", pmdl->m_line_col);
				xml.add_leaf("node_color", pmdl->m_node_col);
				xml.add_leaf("selection_color", pmdl->m_sel_col);
				xml.add_leaf("smooth_angle", pmdl->GetSmoothingAngle());
				xml.add_leaf("elem_div", pmdl->GetSubDivisions());
				xml.add_leaf("shell2hex", pmdl->ShowShell2Solid());
				xml.add_leaf("shellref", pmdl->ShellReferenceSurface());
				xml.add_leaf("render_mode", pmdl->GetRenderMode());

				// Displacement Map properties
				Post::CGLDisplacementMap* pd = pmdl->GetDisplacementMap();
				if (pd)
				{
					xml.add_branch("Displacement");
					{
						xml.add_leaf("enable", pd->IsActive());
						xml.add_leaf("field", pfem->GetDisplacementField());
						xml.add_leaf("scale", pd->GetScale());
					}
					xml.close_branch();
				}

				// Color Map properties
				Post::CGLColorMap* pc = pmdl->GetColorMap();
				if (pc)
				{
					Post::CColorTexture* pmap = pc->GetColorMap();
					xml.add_branch("ColorMap");
					{
						xml.add_leaf("enable", pc->IsActive());
						xml.add_leaf("field", pc->GetEvalField());
						xml.add_leaf("smooth", pmap->GetSmooth());
						xml.add_leaf("map", pmap->GetColorMap());
						xml.add_leaf("nodal_values", pc->DisplayNodalValues());
						xml.add_leaf("range_type", pc->GetRangeType());
						xml.add_leaf("range_divs", pmap->GetDivisions());
						xml.add_leaf("show_legend", pc->ShowLegend());

						float d[2];
						pc->GetRange(d);
						xml.add_leaf("user_max", d[1]);
						xml.add_leaf("user_min", d[0]);
					}
					xml.close_branch();
				}

				// store plots
				Post::GPlotList& plotList = pmdl->GetPlotList();
				if (plotList.IsEmpty() == false)
				{
					for (int n =0; n<plotList.Size(); ++n)
					{
						Post::CGLPlot* p = plotList[n];
						if (dynamic_cast<Post::CGLPlaneCutPlot*>(p))
						{
							Post::CGLPlaneCutPlot* pcut = dynamic_cast<Post::CGLPlaneCutPlot*>(p);
							XMLElement el("plot");
							el.add_attribute("type", "planecut");
							xml.add_branch(el);
							{
								xml.add_leaf("enable", pcut->IsActive());
								xml.add_leaf("show_plane", pcut->m_bshowplane);
								xml.add_leaf("cut_hidden", pcut->m_bcut_hidden);

								double a[4];
								pcut->GetNormalizedEquations(a);
								xml.add_leaf("plane", a, 4);
							}
							xml.close_branch();
						}

					}
				}

				// image models
				for (int j = 0; j < doc->ImageModels(); ++j)
				{
					Post::CImageModel* img = doc->GetImageModel(j);

					XMLElement e("ImageStack");
					e.add_attribute("id", j + 1);
					e.add_attribute("name", img->GetName());
					xml.add_branch(e);
					{
						C3DImage* im = img->GetImageSource()->Get3DImage();
						int dim[3] = { im->Width(), im->Height(), im->Depth() };
						BOX bb = img->GetBoundingBox();
						double b[6] = { bb.x0, bb.y0, bb.z0, bb.x1, bb.y1, bb.z1};
                        string fname = img->GetImageSource()->GetFileName();
						xml.add_leaf("file", fname);
						xml.add_leaf("size", dim, 3);
						xml.add_leaf("box", b, 6);
					}
					xml.close_branch();
				}

				// View properties
				CGView* pv = doc->GetView();
				if (pv)
				{
					CGLCamera& cam = pv->GetCamera();
					quatd q = cam.GetOrientation();
					float w = q.GetAngle();
					vec3d v = q.GetVector()*w;
					vec3d r = cam.GetPosition();
					float d = cam.GetTargetDistance();
//					int nproj = GetViewSettings().m_nproj;
//					int nconv = GetViewSettings().m_nconv;

					xml.add_branch("View");
					{
						xml.add_leaf("x-angle", v.x);
						xml.add_leaf("y-angle", v.y);
						xml.add_leaf("z-angle", v.z);
						xml.add_leaf("x-pos", r.x);
						xml.add_leaf("y-pos", r.y);
						xml.add_leaf("z-pos", r.z);
						xml.add_leaf("target", d);
//						xml.add_leaf("projection", nproj);
//						xml.add_leaf("convention", nconv);

						int N = pv->CameraKeys();
						for (int i=0; i<N; ++i)
						{
							GLCameraTransform& key = pv->GetKey(i);
							q = key.rot;
							w = q.GetAngle();
							v = q.GetVector()*w;
							XMLElement Key;
							Key.name("Key");
							string keyName = key.GetName();
							Key.add_attribute("name", keyName.c_str());
							xml.add_branch(Key);
							{
								xml.add_leaf("x-angle", v.x);
								xml.add_leaf("y-angle", v.y);
								xml.add_leaf("z-angle", v.z);
								xml.add_leaf("x-pos", key.pos.x);
								xml.add_leaf("y-pos", key.pos.y);
								xml.add_leaf("z-pos", key.pos.z);
								xml.add_leaf("x-trg", key.trg.x);
								xml.add_leaf("y-trg", key.trg.y);
								xml.add_leaf("z-trg", key.trg.z);
							}
							xml.close_branch();
						}
					}
					xml.close_branch();
				}

				// material properties
				int nmat = pfem->Materials();
				for (int i=0; i<nmat; ++i)
				{
					Post::FEMaterial& m = *pfem->GetMaterial(i);
					e.name("Material");
					e.add_attribute("id", i+1);
					e.add_attribute("name", m.GetName());
					xml.add_branch(e);
					{
						xml.add_leaf("diffuse", m.diffuse);
						xml.add_leaf("ambient", m.ambient);
						xml.add_leaf("specular", m.specular);
						xml.add_leaf("emission", m.emission);
						xml.add_leaf("mesh_col", m.meshcol);
						xml.add_leaf("shininess", m.shininess);
						xml.add_leaf("transparency", m.transparency);
						xml.add_leaf("enable", m.benable);
						xml.add_leaf("visible", m.bvisible);
						xml.add_leaf("show_mesh", m.bmesh);
						xml.add_leaf("shadow", m.bcast_shadows);
						xml.add_leaf("clip", m.bclip);
						xml.add_leaf("render_mode", m.m_nrender);
						xml.add_leaf("transparency_mode", m.m_ntransmode);
					}
					xml.close_branch();
				}
			}
			xml.close_branch();

			// store the view settings
/*			xml.add_branch("Settings");
			{
				VIEWSETTINGS& v = GetViewSettings();
				xml.add_leaf("bgcol1", v.bgcol1);
				xml.add_leaf("bgcol2", v.bgcol2);
				xml.add_leaf("fgcol", v.fgcol);
				xml.add_leaf("bgstyle", v.bgstyle);
				xml.add_leaf("shadows", v.m_bShadows);
				xml.add_leaf("shadow_intensity", v.m_shadow_intensity);
				xml.add_leaf("ambient", v.m_ambient);
				xml.add_leaf("diffuse", v.m_diffuse);
				xml.add_leaf("triad", v.m_bTriad);
				xml.add_leaf("tags", v.m_bTags);
				xml.add_leaf("taginfo", v.m_ntagInfo);
				xml.add_leaf("title", v.m_bTitle);
				xml.add_leaf("select_connected", v.m_bconn);
				xml.add_leaf("ignore_interior", v.m_bext);
				xml.add_leaf("show_box", v.m_bBox);
				xml.add_leaf("projection", v.m_nproj);
				xml.add_leaf("convention", v.m_nconv);
				xml.add_leaf("lighting", v.m_bLighting);
				xml.add_leaf("cull_face", v.m_bignoreBackfacingItems);
				xml.add_leaf("line_smooth", v.m_blinesmooth);
				xml.add_leaf("line_thickness", v.m_flinethick);
				xml.add_leaf("spring_thickness", v.m_fspringthick);
				xml.add_leaf("point_size", v.m_fpointsize);
			}
			xml.close_branch();
*/		}
	}
	xml.close_branch();

	xml.close();
	return true;
}

//-----------------------------------------------------------------------------
void get_file_path(const char* szfilename, char* szpath)
{
	strcpy(szpath, szfilename);
	char* ch = strrchr(szpath, '\\');
	if (ch==0) ch = strrchr(szpath, '/');
	if (ch==0) szpath[0] = 0;
	else *(ch+1)=0;
}

//-----------------------------------------------------------------------------
void make_file_path(char* szout, const char* szfile, const char* szpath)
{
	strcpy(szout, szfile);

	// if not, use the same path as the session file
	char* ch = strrchr(szout, '\\');
	if (ch == 0) ch = strrchr(szout, '/');
	if (ch == 0) sprintf(szout, "%s%s", szpath, szfile);
}

//-----------------------------------------------------------------------------
// Restore a saved session
bool CDocManager::OpenSession(const std::string& sfile)
{
	FILE* fp = fopen(sfile.c_str(), "rt");
	if (fp == 0) return false;
	XMLReader xml;
	xml.Attach(fp);

	// try to open the file
	XMLTag tag;
	if (xml.FindTag("postview_spec", tag) == false) { fclose(fp); return false; }

	// get the path of the file
	char szpath[1024] = {0};
	get_file_path(sfile.c_str(), szpath);

	// parse the file
	xml.NextTag(tag);
	do
	{
		if (tag == "Model")
		{
			const char* szfile = tag.AttributeValue("file");
			Post::FEFileReader* pimp = GetFileReader(szfile);
			if (pimp == 0) { fclose(fp); return false; }

			// see if there is a path defined
			char szfilename[1024];
			make_file_path(szfilename, szfile, szpath);

			// create a new document
			CDocument* doc = new CDocument(m_wnd);

			// try to load the model
			if (doc->LoadFEModel(pimp, szfilename) == false) 
			{ 
				delete doc;
				fclose(fp); 
				return false; 
			}
			else AddDocument(doc);

			int ntime = doc->currentTime();

			Post::CGLModel* pmdl = doc->GetGLModel();
			Post::FEPostModel* pfem = pmdl->GetFEModel();

			float f;
			double g;
			int n;
			bool b;

			xml.NextTag(tag);
			do
			{
				if      (tag == "show_ghost"  ) tag.value(pmdl->m_bghost);
				else if (tag == "line_color"  ) tag.value(pmdl->m_line_col);
				else if (tag == "node_color"  ) tag.value(pmdl->m_node_col);
				else if (tag == "selection_color") tag.value(pmdl->m_sel_col);
				else if (tag == "smooth_angle") { tag.value(g); pmdl->SetSmoothingAngle(g); }
				else if (tag == "elem_div"    ) { tag.value(n); pmdl->SetSubDivisions(n); }
				else if (tag == "shell2hex"   ) { tag.value(b); pmdl->ShowShell2Solid(b); }
				else if (tag == "render_mode" ) { tag.value(n); pmdl->SetRenderMode(n); }
				else if (tag == "state") tag.value(ntime);
				else if (tag == "Displacement")
				{
					Post::CGLDisplacementMap* pd = pmdl->GetDisplacementMap();
					assert(pd);
					if (pd)
					{
						xml.NextTag(tag);
						do
						{
							if (tag == "field") { tag.value(n); pfem->SetDisplacementField(n); }
							else if (tag == "scale") { tag.value(f); pd->SetScale(f); }
							else if (tag == "enable") { tag.value(b); pd->Activate(b); }
							xml.NextTag(tag);
						}
						while (!tag.isend());
					}
				}
				else if (tag == "ColorMap")
				{
					Post::CGLColorMap* pc = pmdl->GetColorMap();
					Post::CColorTexture* pmap = pc->GetColorMap();
					assert(pc);
					if (pc)
					{
						xml.NextTag(tag);
						do
						{
							if (tag == "field") { tag.value(n); pc->SetEvalField(n); }
							else if (tag == "map") { tag.value(n); pmap->SetColorMap(n); }
							else if (tag == "smooth") { tag.value(b); pmap->SetSmooth(b); }
							else if (tag == "nodal_value") { tag.value(b); pc->DisplayNodalValues(b); }
							else if (tag == "range_type") { tag.value(n); pc->SetRangeType(n); }
							else if (tag == "range_divs") { tag.value(n); pmap->SetDivisions(n); }
							else if (tag == "show_legend") { tag.value(b); pc->ShowLegend(b); }
							else if (tag == "user_max") { tag.value(f); pc->SetRangeMax(f); }
							else if (tag == "user_min") { tag.value(f); pc->SetRangeMin(f); }
							else if (tag == "enable") { tag.value(b); pc->Activate(b); }
							else xml.SkipTag(tag);
							xml.NextTag(tag);
						}
						while (!tag.isend());
					}
				}
				else if (tag == "plot")
				{
					const char* sztype = tag.AttributeValue("type");
					if (strcmp(sztype, "planecut") == 0)
					{
						Post::CGLPlaneCutPlot* pg = new Post::CGLPlaneCutPlot(pmdl);
						xml.NextTag(tag);
						do
						{
							if (tag == "enable") { tag.value(b); pg->Activate(b); }
							else if (tag == "show_plane") { tag.value(b); pg->m_bshowplane = b; }
							else if (tag == "cut_hidden") { tag.value(b); pg->m_bcut_hidden = b; }
							else if (tag == "plane")
							{
								double a[4];
								tag.value(a, 4);
//								pg->SetPlaneEqn(a);
							}
							xml.NextTag(tag);
						}
						while (!tag.isend());
						pmdl->AddPlot(pg);
					}
					else xml.SkipTag(tag);
				}
				else if (tag == "ImageStack")
				{
					const char* szname = tag.AttributeValue("name", true);
					if (szname == 0) szname = "image";
					Post::CImageModel* img = new Post::CImageModel(doc->GetGLModel());
					img->SetName(szname);

					char szfile[1024] = { 0 };
					int dim[3];
					float b[6];

					xml.NextTag(tag);
					do
					{
						if (tag == "file") tag.value(szfile);
						else if (tag == "size") tag.value(dim, 3);
						else if (tag == "box") tag.value(b, 6);
						xml.NextTag(tag);
					}
					while (!tag.isend());

					make_file_path(szfilename, szfile, szpath);

					BOX box(b[0], b[1], b[2], b[3], b[4], b[5]);

					if (img->LoadImageData(szfilename, dim[0], dim[1], dim[2], box) == false)
					{
						delete img;
						fclose(fp);
						return false;
					}

					doc->AddImageModel(img);
				}
				else if (tag == "View")
				{
					CGView* pv = doc->GetView();
					assert(pv);
					if (pv)
					{
						vec3f v, r;
						float f=0;
						int nproj=0;
                        int nconv=0;
						xml.NextTag(tag);
						do
						{
							if      (tag == "x-angle") tag.value(v.x);
							else if (tag == "y-angle") tag.value(v.y);
							else if (tag == "z-angle") tag.value(v.z);
							else if (tag == "x-pos") tag.value(r.x);
							else if (tag == "y-pos") tag.value(r.y);
							else if (tag == "z-pos") tag.value(r.z);
							else if (tag == "target") tag.value(f);
							else if (tag == "projection") tag.value(nproj);
                            else if (tag == "convention") tag.value(nconv);
							else if (tag == "Key")
							{
								GLCameraTransform key;
								vec3f vk;
								const char* szname = tag.AttributeValue("name");
								key.SetName(szname);
								xml.NextTag(tag);
								do
								{
									if      (tag == "x-angle") tag.value(vk.x);
									else if (tag == "y-angle") tag.value(vk.y);
									else if (tag == "z-angle") tag.value(vk.z);
									else if (tag == "x-pos") tag.value(key.pos.x);
									else if (tag == "y-pos") tag.value(key.pos.y);
									else if (tag == "z-pos") tag.value(key.pos.z);
									else if (tag == "x-trg") tag.value(key.trg.x);
									else if (tag == "y-trg") tag.value(key.trg.y);
									else if (tag == "z-trg") tag.value(key.trg.z);
									else xml.SkipTag(tag);
									xml.NextTag(tag);
								}
								while (!tag.isend());
								float w = vk.Length();
								if (w != 0.f) key.rot = quatd(w, vk); else key.rot = quatd(0.f, vec3f(1.f, 0.f, 0.f));
								pv->AddCameraKey(key);
							}
							else xml.SkipTag(tag);
							xml.NextTag(tag);
						}
						while (!tag.isend());

						quatd q = quatd(v.Length(), v);
						CGLCamera& cam = pv->GetCamera();
						cam.SetTargetDistance(f);
						cam.SetTarget(r);
						cam.SetOrientation(q);
						cam.Update(true);
						
//						GetViewSettings().m_nproj = nproj;
//                        GetViewSettings().m_nconv = nconv;
					}
				}
				else if (tag == "Material")
				{
					int nmat = pfem->Materials();
					int nid = atoi(tag.AttributeValue("id"))-1;
					if ((nid >= 0) && (nid < nmat))
					{
						Post::FEMaterial& m = *pfem->GetMaterial(nid);
						m.SetName(tag.AttributeValue("name"));

						xml.NextTag(tag);
						do
						{
							if (tag == "diffuse") tag.value(m.diffuse);
							else if (tag == "ambient") tag.value(m.ambient);
							else if (tag == "specular") tag.value(m.specular);
							else if (tag == "emission") tag.value(m.emission);
							else if (tag == "mesh_col") tag.value(m.meshcol);
							else if (tag == "shininess") tag.value(m.shininess);
							else if (tag == "transparency") tag.value(m.transparency);
							else if (tag == "enable") tag.value(m.benable);
							else if (tag == "visible") tag.value(m.bvisible);
							else if (tag == "show_mesh") tag.value(m.bmesh);
							else if (tag == "shadow") tag.value(m.bcast_shadows);
							else if (tag == "clip") tag.value(m.bclip);
							else if (tag == "render_mode") tag.value(m.m_nrender);
							else if (tag == "transparency_mode") tag.value(m.m_ntransmode);
							else xml.SkipTag(tag);
							xml.NextTag(tag);
						}
						while (!tag.isend());
					}
					else xml.SkipTag(tag);
				}
				else xml.SkipTag(tag);

				xml.NextTag(tag);
			}
			while (!tag.isend());

			if (ntime != doc->currentTime()) doc->SetCurrentTime(ntime);
		}
/*		else if (tag == "Settings")
		{
			// read the view settings
			VIEWSETTINGS& v = GetViewSettings();
			xml.NextTag(tag);
			do
			{
				if (tag == "bgcol1") tag.value(v.bgcol1);
				else if (tag == "bgcol2") tag.value(v.bgcol2);
				else if (tag == "fgcol") tag.value(v.fgcol);
				else if (tag == "bgstyle") tag.value(v.bgstyle);
				else if (tag == "shadows") tag.value(v.m_bShadows);
				else if (tag == "shadow_intensity") tag.value(v.m_shadow_intensity);
				else if (tag == "ambient") tag.value(v.m_ambient);
				else if (tag == "diffuse") tag.value(v.m_diffuse);
				else if (tag == "triad") tag.value(v.m_bTriad);
				else if (tag == "tags") tag.value(v.m_bTags);
				else if (tag == "taginfo") tag.value(v.m_ntagInfo);
				else if (tag == "title") tag.value(v.m_bTitle);
				else if (tag == "select_connected") tag.value(v.m_bconn);
				else if (tag == "ignore_interior") tag.value(v.m_bext);
				else if (tag == "show_box") tag.value(v.m_bBox);
				else if (tag == "projection") tag.value(v.m_nproj);
                else if (tag == "convention") tag.value(v.m_nconv);
				else if (tag == "lighting") tag.value(v.m_bLighting);
				else if (tag == "cull_face") tag.value(v.m_bignoreBackfacingItems);
				else if (tag == "line_smooth") tag.value(v.m_blinesmooth);
				else if (tag == "line_thickness") tag.value(v.m_flinethick);
				else if (tag == "spring_thickness") tag.value(v.m_fspringthick);
				else if (tag == "point_size") tag.value(v.m_fpointsize);
				else xml.SkipTag(tag);
				xml.NextTag(tag);
			}
			while (!tag.isend());
		}
*/		else xml.SkipTag(tag);
		xml.NextTag(tag);
	}
	while (!tag.isend());

	fclose(fp);
	return true;
}
