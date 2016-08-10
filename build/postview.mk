include ../../$(PLAT).mk

SRC = $(wildcard $(PSTDIR)PostView2/*.cpp)
OBJ = $(patsubst $(PSTDIR)PostView2/%.cpp, %.o, $(SRC))
DEP = $(patsubst $(PSTDIR)PostView2/%.cpp, %.d, $(SRC))

TARGET =  $(PSTDIR)build/bin/postview.$(PLAT)

PSTLIBS = -L$(PSTDIR)build/lib -lpostviewlib_$(PLAT)

$(TARGET): $(OBJ)
	$(CC) -o $(TARGET) $(DEF) $(FLG) $(INC) $(OBJ) $(PSTLIBS) $(LIBS)

%.o: $(PSTDIR)PostView2/%.cpp
	$(CC) $(INC) $(DEF) $(FLG) -MMD -c -o $@ $<

clean:
	$(RM) *.o *.d $(TARGET)

-include $(DEP)
