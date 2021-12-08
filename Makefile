IDIR =include
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=src
LDIR =lib

LIBS=-lm

_DEPS = core.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = core.o corefunc.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

all: core clean

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -std=c11 -c -o $@ $< $(CFLAGS)

core: $(OBJ)
	$(CC) -std=c11 -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o 
