#include "stdafx.h"
#include "FEFEBioExport.h"
#include "XMLWriter.h"

bool FEFEBioExport::Save(FEModel& fem, const char* szfile)
{
	int nmat = fem.Materials();

	FEMeshBase* pm = fem.GetFEMesh(0);
	FEState* pst = fem.GetActiveState();

	XMLWriter xml;
	if (xml.open(szfile) == false) return false;

	XMLElement el;

	el.name("febio_spec");
	el.add_attribute("version", "1.0");
	xml.add_branch(el);
	{
		xml.add_branch("Material");
		{
			char sz[256] = {0};
			el.clear();
			el.name("material");
			int n1 = el.add_attribute("id", "");
			int n2 = el.add_attribute("name", "");
			el.add_attribute("type", "linear elastic");
			for (int i=0; i<nmat; ++i)
			{
				sprintf(sz, "material%02d", i+1);
				el.set_attribute(n1, i+1);
				el.set_attribute(n2, sz);
				xml.add_leaf(el, false);				
			}
		}
		xml.close_branch();

		xml.add_branch("Geometry");
		{
			xml.add_branch("Nodes");
			{
				el.clear();
				el.name("node");
				int n1 = el.add_attribute("id", "");
				for (int i=0; i<pm->Nodes(); ++i)
				{
					vec3f& r = pst->m_NODE[i].m_rt;
					el.set_attribute(n1, i+1);
					el.value(r);
					xml.add_leaf(el, false);
				}
			}
			xml.close_branch(); // Nodes

			xml.add_branch("Elements");
			{
				int n[FEGenericElement::MAX_NODES];
				el.clear();
				int n1 = el.add_attribute("id", "");
				int n2 = el.add_attribute("mat", "");
				for (int i=0; i<pm->Elements(); ++i)
				{
					FEElement& elm = pm->Element(i);
					switch (elm.Type())
					{
					case FE_HEX8   : el.name("hex8"  ); break;
					case FE_HEX20  : el.name("hex20" ); break;
					case FE_HEX27  : el.name("hex27" ); break;
					case FE_PENTA6 : el.name("penta6"); break;
                    case FE_PENTA15: el.name("penta15"); break;
                    case FE_TET4   : el.name("tet4"  ); break;
					case FE_QUAD4  : el.name("quad4" ); break;
                    case FE_QUAD8  : el.name("quad8" ); break;
                    case FE_QUAD9  : el.name("quad9" ); break;
					case FE_TRI3   : el.name("tri3"  ); break;
					case FE_TET10  : el.name("tet10" ); break;
					case FE_TET15  : el.name("tet15" ); break;
					case FE_TET20  : el.name("tet20" ); break;
                    case FE_TRI6   : el.name("tri6"  ); break;
					case FE_PYRA5  : el.name("pyra5" ); break;
					default:
						assert(false);
					}

					for (int j=0; j<elm.Nodes(); ++j) n[j] = elm.m_node[j]+1;

					el.set_attribute(n1, i+1);
					el.set_attribute(n2, elm.m_MatID+1);
					el.value(n, elm.Nodes());
					xml.add_leaf(el, false);
				}
			}
			xml.close_branch();
		}
		xml.close_branch(); // Geometry
	}
	xml.close_branch(); // febio_spec
	xml.close();

	return true;
}
