#include "DataFieldSelector.h"
#include <QTreeWidget>
#include <QHeaderView>
#include <PostViewLib/FEModel.h>
#include <PostViewLib/constants.h>

CDataFieldSelector::CDataFieldSelector(QWidget* parent) : QComboBox(parent)
{
	QTreeWidget* pw = new QTreeWidget;
	pw->header()->hide();
	setModel(pw->model());
	setView(pw);
	setMinimumWidth(200);
	view()->setMinimumWidth(200);
	view()->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
	setCurrentIndex(-1);

/*    QString styleSheet = "QComboBox \
        {  \
            background-color:           white; \
        }";

    setStyleSheet(styleSheet); 
*/
}

void CDataFieldSelector::BuildMenu(FEModel* fem, int nclass, bool btvec)
{
	// get the tree view and clear it
	QTreeWidget* pw = qobject_cast<QTreeWidget*>(view());
	pw->clear();

	// get the datamanager
	FEDataManager* pdm = fem->GetDataManager();

	QTreeWidgetItem* pi;

	// add fields
	FEDataFieldPtr pd;
	int nval = 0;
	int n = 0;
	char szline[256] = {0};
	for (int j=0; j<3; ++j)
	{
		int ntype = 0;
		int N = 0;
		switch(j)
		{
		case 0: N = pdm->NodeFields   (); pd = pdm->FirstNode   (); ntype = 1; break;
		case 1: N = pdm->ElementFields(); pd = pdm->FirstElement(); ntype = 2; break;
		case 2: N = pdm->FaceFields   (); pd = pdm->FirstFace   (); ntype = 4; break;
		}

		for (int i=0; i<N; ++i, ++pd)
		{
			int ncode = i;
			assert(ncode <= 0xFF);
			int nfield = 0;
			FEDataField& d = *(*pd);
			int nt = d.Type();
			switch (nclass)
			{
			case DATA_FLOAT:
				switch (nt)
				{
				case DATA_FLOAT: 
					nfield = BUILD_FIELD(ntype, ncode, 0);
					pi = new QTreeWidgetItem(pw); pi->setText(0, d.GetName());
					pi->setData(0, Qt::UserRole, QVariant(nfield));
					break;
				case DATA_VEC3F:
					pi = new QTreeWidgetItem(pw); pi->setText(0, d.GetName());
					pi->setFlags(pi->flags() & ~Qt::ItemIsSelectable); 

					nfield = BUILD_FIELD(ntype, ncode, 0); sprintf(szline, "X - %s"  , d.GetName()); addComponent(pi, szline, nfield);
					nfield = BUILD_FIELD(ntype, ncode, 1); sprintf(szline, "Y - %s"  , d.GetName()); addComponent(pi, szline, nfield);
					nfield = BUILD_FIELD(ntype, ncode, 2); sprintf(szline, "Z - %s"  , d.GetName()); addComponent(pi, szline, nfield);
					nfield = BUILD_FIELD(ntype, ncode, 3); sprintf(szline, "XY - %s" , d.GetName()); addComponent(pi, szline, nfield);
					nfield = BUILD_FIELD(ntype, ncode, 4); sprintf(szline, "YZ - %s" , d.GetName()); addComponent(pi, szline, nfield);
					nfield = BUILD_FIELD(ntype, ncode, 5); sprintf(szline, "XZ - %s" , d.GetName()); addComponent(pi, szline, nfield);
					nfield = BUILD_FIELD(ntype, ncode, 6); sprintf(szline, "Total %s", d.GetName()); addComponent(pi, szline, nfield);
					n++;
					break;
				case DATA_MAT3F:
				case DATA_MAT3D:
					pi = new QTreeWidgetItem(pw); pi->setText(0, d.GetName());
					pi->setFlags(pi->flags() & ~Qt::ItemIsSelectable); 

					nfield = BUILD_FIELD(ntype, ncode,  0); sprintf(szline, "XX - %s" , d.GetName()); addComponent(pi, szline, nfield);
					nfield = BUILD_FIELD(ntype, ncode,  1); sprintf(szline, "XY - %s" , d.GetName()); addComponent(pi, szline, nfield);
					nfield = BUILD_FIELD(ntype, ncode,  2); sprintf(szline, "XZ - %s" , d.GetName()); addComponent(pi, szline, nfield);
					nfield = BUILD_FIELD(ntype, ncode,  3); sprintf(szline, "YX - %s" , d.GetName()); addComponent(pi, szline, nfield);
					nfield = BUILD_FIELD(ntype, ncode,  4); sprintf(szline, "YY - %s" , d.GetName()); addComponent(pi, szline, nfield);
					nfield = BUILD_FIELD(ntype, ncode,  5); sprintf(szline, "YZ - %s" , d.GetName()); addComponent(pi, szline, nfield);
					nfield = BUILD_FIELD(ntype, ncode,  6); sprintf(szline, "ZX - %s" , d.GetName()); addComponent(pi, szline, nfield);
					nfield = BUILD_FIELD(ntype, ncode,  7); sprintf(szline, "ZY - %s" , d.GetName()); addComponent(pi, szline, nfield);
					nfield = BUILD_FIELD(ntype, ncode,  8); sprintf(szline, "ZZ - %s" , d.GetName()); addComponent(pi, szline, nfield);

					n++;
					break;
				case DATA_MAT3FS:
					pi = new QTreeWidgetItem(pw); pi->setText(0, d.GetName());
					pi->setFlags(pi->flags() & ~Qt::ItemIsSelectable); 

					nfield = BUILD_FIELD(ntype, ncode,  0); sprintf(szline, "X - %s"  , d.GetName()); addComponent(pi, szline, nfield);
					nfield = BUILD_FIELD(ntype, ncode,  1); sprintf(szline, "Y - %s"  , d.GetName()); addComponent(pi, szline, nfield);
					nfield = BUILD_FIELD(ntype, ncode,  2); sprintf(szline, "Z - %s"  , d.GetName()); addComponent(pi, szline, nfield);
					nfield = BUILD_FIELD(ntype, ncode,  3); sprintf(szline, "XY - %s" , d.GetName()); addComponent(pi, szline, nfield);
					nfield = BUILD_FIELD(ntype, ncode,  4); sprintf(szline, "YZ - %s" , d.GetName()); addComponent(pi, szline, nfield);
					nfield = BUILD_FIELD(ntype, ncode,  5); sprintf(szline, "XZ - %s" , d.GetName()); addComponent(pi, szline, nfield);
					nfield = BUILD_FIELD(ntype, ncode,  6); sprintf(szline, "Effective %s"  , d.GetName()); addComponent(pi, szline, nfield);
					nfield = BUILD_FIELD(ntype, ncode,  7); sprintf(szline, "1 Principal %s", d.GetName()); addComponent(pi, szline, nfield);
					nfield = BUILD_FIELD(ntype, ncode,  8); sprintf(szline, "2 Principal %s", d.GetName()); addComponent(pi, szline, nfield);
					nfield = BUILD_FIELD(ntype, ncode,  9); sprintf(szline, "3 Principal %s", d.GetName()); addComponent(pi, szline, nfield);
					nfield = BUILD_FIELD(ntype, ncode, 10); sprintf(szline, "1 Dev Princ %s", d.GetName()); addComponent(pi, szline, nfield);
					nfield = BUILD_FIELD(ntype, ncode, 11); sprintf(szline, "2 Dev Princ %s", d.GetName()); addComponent(pi, szline, nfield);
					nfield = BUILD_FIELD(ntype, ncode, 12); sprintf(szline, "3 Dev Princ %s", d.GetName()); addComponent(pi, szline, nfield);
					nfield = BUILD_FIELD(ntype, ncode, 13); sprintf(szline, "Max Shear %s"  , d.GetName()); addComponent(pi, szline, nfield);

					n++;
					break;
				case DATA_MAT3FD:
					pi = new QTreeWidgetItem(pw); pi->setText(0, d.GetName());
					pi->setFlags(pi->flags() & ~Qt::ItemIsSelectable); 

					nfield = BUILD_FIELD(ntype, ncode, 0); sprintf(szline, "1 - %s"  , d.GetName()); addComponent(pi, szline, nfield);
					nfield = BUILD_FIELD(ntype, ncode, 1); sprintf(szline, "2 - %s"  , d.GetName()); addComponent(pi, szline, nfield);
					nfield = BUILD_FIELD(ntype, ncode, 2); sprintf(szline, "3 - %s"  , d.GetName()); addComponent(pi, szline, nfield);

					n++;
					break;
                case DATA_TENS4FS:
					pi = new QTreeWidgetItem(pw); pi->setText(0, d.GetName());
					pi->setFlags(pi->flags() & ~Qt::ItemIsSelectable); 
                        
                    nfield = BUILD_FIELD(ntype, ncode,  0); sprintf(szline, "XXXX - %s" , d.GetName()); addComponent(pi, szline, nfield);
                    nfield = BUILD_FIELD(ntype, ncode,  1); sprintf(szline, "XXYY - %s" , d.GetName()); addComponent(pi, szline, nfield);
                    nfield = BUILD_FIELD(ntype, ncode,  2); sprintf(szline, "YYYY - %s" , d.GetName()); addComponent(pi, szline, nfield);
                    nfield = BUILD_FIELD(ntype, ncode,  3); sprintf(szline, "XXZZ - %s" , d.GetName()); addComponent(pi, szline, nfield);
                    nfield = BUILD_FIELD(ntype, ncode,  4); sprintf(szline, "YYZZ - %s" , d.GetName()); addComponent(pi, szline, nfield);
                    nfield = BUILD_FIELD(ntype, ncode,  5); sprintf(szline, "ZZZZ - %s" , d.GetName()); addComponent(pi, szline, nfield);
                    nfield = BUILD_FIELD(ntype, ncode,  6); sprintf(szline, "XXXY - %s" , d.GetName()); addComponent(pi, szline, nfield);
                    nfield = BUILD_FIELD(ntype, ncode,  7); sprintf(szline, "YYXY - %s" , d.GetName()); addComponent(pi, szline, nfield);
                    nfield = BUILD_FIELD(ntype, ncode,  8); sprintf(szline, "ZZXY - %s" , d.GetName()); addComponent(pi, szline, nfield);
                    nfield = BUILD_FIELD(ntype, ncode,  9); sprintf(szline, "XYXY - %s" , d.GetName()); addComponent(pi, szline, nfield);
                    nfield = BUILD_FIELD(ntype, ncode, 10); sprintf(szline, "XXYZ - %s" , d.GetName()); addComponent(pi, szline, nfield);
                    nfield = BUILD_FIELD(ntype, ncode, 11); sprintf(szline, "YYYZ - %s" , d.GetName()); addComponent(pi, szline, nfield);
                    nfield = BUILD_FIELD(ntype, ncode, 12); sprintf(szline, "ZZYZ - %s" , d.GetName()); addComponent(pi, szline, nfield);
                    nfield = BUILD_FIELD(ntype, ncode, 13); sprintf(szline, "XYYZ - %s" , d.GetName()); addComponent(pi, szline, nfield);
                    nfield = BUILD_FIELD(ntype, ncode, 14); sprintf(szline, "YZYZ - %s" , d.GetName()); addComponent(pi, szline, nfield);
                    nfield = BUILD_FIELD(ntype, ncode, 15); sprintf(szline, "XXXZ - %s" , d.GetName()); addComponent(pi, szline, nfield);
                    nfield = BUILD_FIELD(ntype, ncode, 16); sprintf(szline, "YYXZ - %s" , d.GetName()); addComponent(pi, szline, nfield);
                    nfield = BUILD_FIELD(ntype, ncode, 17); sprintf(szline, "ZZXZ - %s" , d.GetName()); addComponent(pi, szline, nfield);
                    nfield = BUILD_FIELD(ntype, ncode, 18); sprintf(szline, "XYXZ - %s" , d.GetName()); addComponent(pi, szline, nfield);
                    nfield = BUILD_FIELD(ntype, ncode, 19); sprintf(szline, "YZXZ - %s" , d.GetName()); addComponent(pi, szline, nfield);
                    nfield = BUILD_FIELD(ntype, ncode, 20); sprintf(szline, "XZXZ - %s" , d.GetName()); addComponent(pi, szline, nfield);
                    n++;
                    break;
				}
				break;
			case DATA_VEC3F:
				switch (nt)
				{
				case DATA_VEC3F:
					nfield = BUILD_FIELD(ntype, ncode,  0);
					pi = new QTreeWidgetItem(pw); pi->setText(0, d.GetName());
					pi->setData(0, Qt::UserRole, QVariant(nfield));
					break;
				case DATA_MAT3FS:
					if (btvec)
					{
						pi = new QTreeWidgetItem(pw); pi->setText(0, d.GetName());
						pi->setFlags(pi->flags() & ~Qt::ItemIsSelectable); 

						nfield = BUILD_FIELD(ntype, ncode,  0); sprintf(szline, "1 Principal %s"  , d.GetName()); addComponent(pi, szline, nfield);
						nfield = BUILD_FIELD(ntype, ncode,  1); sprintf(szline, "2 Principal %s"  , d.GetName()); addComponent(pi, szline, nfield);
						nfield = BUILD_FIELD(ntype, ncode,  2); sprintf(szline, "3 Principal %s"  , d.GetName()); addComponent(pi, szline, nfield);
						n++;
					}
					break;
				}
				break;
			case DATA_MAT3FS:
				if (nt == DATA_MAT3FS)
				{
					pi = new QTreeWidgetItem(pw); pi->setText(0, d.GetName());
					pi->setData(0, Qt::UserRole, QVariant(nfield));
				}
				break;
			default:
				assert(false);
			}
			if ((nfield != 0) && (nval == 0)) nval = nfield;
			d.SetFieldID(nfield);
		}
	}

	setCurrentIndex(-1);
}

void CDataFieldSelector::addComponent(QTreeWidgetItem* parent, const char* szname, int ndata)
{
	QTreeWidgetItem* pi = new QTreeWidgetItem(parent);
	pi->setText(0, szname);
	pi->setData(0, Qt::UserRole, QVariant(ndata));
}
