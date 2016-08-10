include ../../$(PLAT).mk

LIBDIR = $(notdir $(CURDIR))
PSTLIB = $(shell echo $(LIBDIR) | tr A-Z a-z)

SRC = $(wildcard $(PSTDIR)$(LIBDIR)/*.cpp)
OBJ = $(patsubst $(PSTDIR)$(LIBDIR)/%.cpp, %.o, $(SRC))
DEP = $(patsubst $(PSTDIR)$(LIBDIR)/%.cpp, %.d, $(SRC))

LIB = $(PSTDIR)build/lib/lib$(PSTLIB)_$(PLAT).a

$(LIB): $(OBJ)
	ar -cvr $(LIB) $(OBJ)

%.o: $(PSTDIR)$(LIBDIR)/%.cpp
	$(CC) -MMD -c $(INC) $(DEF) $(FLG) -o $@ $<

clean:
	$(RM) *.o *.d $(LIB)

-include $(DEP)
