CC=gcc
CFLAGS=-I$(IDIR) `pkg-config gtk+-3.0 --cflags` -Wall -Wextra -rdynamic

IDIR = headers
ODIR = obj
LDIR = lib
SDIR = src

LIBS=`pkg-config gtk+-3.0 --libs`

_DEPS = utils.h mills.h gui.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o mills.o gui.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

mills: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
