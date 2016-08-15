PSTDIR = $(dir $(CURDIR))
lnx64: PLAT = lnx64
osx: PLAT = osx
osxclean: PLAT = osx
lnx64g: PLAT = lnx64g
lnx32: PLAT = lnx32
lnx64clean: PLAT = lnx64
lnx64gclean: PLAT = lnx64g
lnx32clean: PLAT = lnx32

export PLAT
export PSTDIR

lnx64 lnx32 lnx64g osx:
	( cd $(PLAT)/PostViewLib;	$(MAKE) -f ../../Makelibs.mk )
	( cd $(PLAT)/PostView2;		$(MAKE) -f ../../postview.mk )

lnx64clean lnx64gclean lnx32clean osxclean:

	( cd $(PLAT)/PostViewLib;	$(MAKE) -f ../../Makelibs.mk clean )
	( cd $(PLAT)/PostView2;		$(MAKE) -f ../../postview.mk clean )

.PHONY: lnx64 lnx32 lnx64g osx

