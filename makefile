CC=gcc
CFLAGS=-I$(IDIR) `pkg-config gtk+-3.0 --cflags` -Wall -Wextra -rdynamic

IDIR = headers
ODIR = obj
LDIR = lib
SDIR = src

LIBS=`pkg-config gtk+-3.0 --libs`

_DEPS = utils.h mills.h gui.h json.h concurrency.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o mills.o gui.o utils.o json.o concurrency.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	@mkdir -p $(@D)
	$(CC) -c -o $@ $< $(CFLAGS)

mills: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o gtk/*~ core $(INCDIR)/*~



