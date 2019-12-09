FEBIOSTUDIODIR = ../../FEBioStudio

LIBS = XPLTLib ImageLib FSCore GeomLib MeshLib XML MathLib

postView: $(LIBS) GLWLib PostGL PostLib CUILib MeshTools GLLib
	cd ../PostView2; qmake PostView2.pro;
	$(MAKE) -C ../PostView2/;

$(LIBS):
	cd $(FEBIOSTUDIODIR)/$@;	$(MAKE) -f ../build/makeLibs.mk
	
GLLib:
	cd $(FEBIOSTUDIODIR)/GLLib; qmake GLLib.pro;
	$(MAKE) -C $(FEBIOSTUDIODIR)/GLLib/;

GLWLib:
	cd $(FEBIOSTUDIODIR)/GLWLib; qmake GLWLib.pro;
	$(MAKE) -C $(FEBIOSTUDIODIR)/GLWLib/;

PostGL:
	cd $(FEBIOSTUDIODIR)/PostGL; qmake PostGL.pro;
	$(MAKE) -C $(FEBIOSTUDIODIR)/PostGL/;

PostLib:
	cd $(FEBIOSTUDIODIR)/PostLib; qmake PostLib.pro;
	$(MAKE) -C $(FEBIOSTUDIODIR)/PostLib/;

CUILib:
	cd $(FEBIOSTUDIODIR)/CUILib; qmake CUILib.pro;
	$(MAKE) -C $(FEBIOSTUDIODIR)/CUILib/;

MeshTools:
	cd $(FEBIOSTUDIODIR)/MeshTools; qmake MeshTools.pro;
	$(MAKE) -C $(FEBIOSTUDIODIR)/MeshTools/;

clean:
	for lib in $(LIBS) ; do \
		cd $(FEBIOSTUDIODIR)/$$lib; $(MAKE) -f ../build/makeLibs.mk clean ;\
	done
	cd ../PostView2/; $(MAKE) distclean
	cd $(FEBIOSTUDIODIR)/GLWLib/; $(MAKE) distclean
	cd $(FEBIOSTUDIODIR)/MeshTools/; $(MAKE) distclean
	cd $(FEBIOSTUDIODIR)/PostGL/; $(MAKE) distclean
	cd $(FEBIOSTUDIODIR)/PostLib/; $(MAKE) distclean
	cd $(FEBIOSTUDIODIR)/CUILib/; $(MAKE) distclean

