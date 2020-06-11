QMAKE = /home/mherron/Resources/Qt/5.14.1/gcc_64/bin/qmake
FEBIOSTUDIODIR = ../../FEBioStudio

LIBS = XPLTLib ImageLib GeomLib MeshLib XML MathLib

postView: $(LIBS) GLWLib PostGL PostLib CUILib MeshTools GLLib FSCore
	cd ../PostView2; $(QMAKE) PostView2.pro;
	$(MAKE) -C ../PostView2/;

$(LIBS):
	cd $(FEBIOSTUDIODIR)/$@;	$(MAKE) -f ../build/makeLibs.mk
	
GLLib:
	cd $(FEBIOSTUDIODIR)/GLLib; $(QMAKE) GLLib.pro;
	$(MAKE) -C $(FEBIOSTUDIODIR)/GLLib/;

GLWLib:
	cd $(FEBIOSTUDIODIR)/GLWLib; $(QMAKE) GLWLib.pro;
	$(MAKE) -C $(FEBIOSTUDIODIR)/GLWLib/;

PostGL:
	cd $(FEBIOSTUDIODIR)/PostGL; $(QMAKE) PostGL.pro;
	$(MAKE) -C $(FEBIOSTUDIODIR)/PostGL/;

PostLib:
	cd $(FEBIOSTUDIODIR)/PostLib; $(QMAKE) PostLib.pro;
	$(MAKE) -C $(FEBIOSTUDIODIR)/PostLib/;

CUILib:
	cd $(FEBIOSTUDIODIR)/CUILib; $(QMAKE) CUILib.pro;
	$(MAKE) -C $(FEBIOSTUDIODIR)/CUILib/;

MeshTools:
	cd $(FEBIOSTUDIODIR)/MeshTools; $(QMAKE) MeshTools.pro;
	$(MAKE) -C $(FEBIOSTUDIODIR)/MeshTools/;

FSCore:
	cd $(FEBIOSTUDIODIR)/FSCore; $(QMAKE) FSCore.pro;
	$(MAKE) -C $(FEBIOSTUDIODIR)/FSCore/;

clean:
	for lib in $(LIBS) ; do \
		cd $(FEBIOSTUDIODIR)/$$lib; $(MAKE) -f ../build/makeLibs.mk clean ;\
	done
	
	cd $(FEBIOSTUDIODIR)/GLWLib/; $(MAKE) distclean
	cd $(FEBIOSTUDIODIR)/MeshTools/; $(MAKE) distclean
	cd $(FEBIOSTUDIODIR)/PostGL/; $(MAKE) distclean
	cd $(FEBIOSTUDIODIR)/PostLib/; $(MAKE) distclean
	cd $(FEBIOSTUDIODIR)/CUILib/; $(MAKE) distclean
	cd $(FEBIOSTUDIODIR)/FSCore/; $(MAKE) distclean
	cd ../PostView2/; $(MAKE) distclean

