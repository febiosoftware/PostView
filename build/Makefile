postView: XPLT GLWLib PostGL PostViewLib
	cd ../PostView2; /home/sci/mherron/Resources/Qt/5.12.0/gcc_64/bin/qmake PostView2.pro;
	$(MAKE) -C ../PostView2/;

XPLT:
	cd ../XPLTLib;	$(MAKE) -C ../XPLTLib/;
	
GLWLib:
	cd ../GLWLib; /home/sci/mherron/Resources/Qt/5.12.0/gcc_64/bin/qmake glwlib.pro;
	$(MAKE) -C ../GLWLib/;

PostGL:
	cd ../PostGL; /home/sci/mherron/Resources/Qt/5.12.0/gcc_64/bin/qmake PostGL.pro;
	$(MAKE) -C ../PostGL/;

PostViewLib:
	cd ../PostViewLib; /home/sci/mherron/Resources/Qt/5.12.0/gcc_64/bin/qmake PostView2Lib.pro;
	$(MAKE) -C ../PostViewLib/;

clean:
	cd ../XPLTLib; $(MAKE) -f ../XPLTLib/Makefile clean ;

	cd ../GLWLib/; $(MAKE) distclean
	cd ../PostGL/; $(MAKE) distclean
	cd ../PostViewLib/; $(MAKE) distclean
	cd ../PostView2/; $(MAKE) distclean
