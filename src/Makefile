# $Id$ #

# ---- Compiling ---- #

# CFLAGS += -I/usr/local/include

# Optimization
CFLAGS += -O2


# ---- Linking ---- #

# If you need anything special for the linker, put it here

#LDFLAGS = 



# ---- Debugging ---- #

# Include the following to allow debugging messages to stderr
CFLAGS += -DDEBUG

# Include the following for profiling information
#
#CFLAGS += -pg
#LDFLAGS += -pg


# Include the following to link against ElectricFence
# LDFLAGS += -lefence

# Include the following to link against DUMA (http://duma.sourceforge.net)
# LDFLAGS += -lduma

# Include the following for debugging output in the binary
CFLAGS += -g

### End of user configuration ###
#################################

CFLAGS  += -Wall -finline-functions -D_GNU_SOURCE
#CFLAGS  += -Wall -pedantic -Wextra

# LDFLAGS += -lpthread

objects = util.o
wg_objects = word_graph.o parse_graph.o search_graph.o

headers = util.h parse_graph.h search_graph.h

all:	word_graph sort_paths

word_graph:	$(objects) $(wg_objects)
	@echo "Linking word_graph ..."
	$(CC) -o word_graph $(wg_objects) $(objects) $(LDFLAGS)

sort_paths: $(objects) sort_paths.o
	@echo "Linking sort_paths ..."
	$(CC) -o sort_paths sort_paths.o $(objects) $(LDFLAGS)

$(objects): $(headers)

$(wg_objects): $(headers)

.PHONY:		clean
clean:
	-rm -f $(objects) $(wg_objects) sort_paths.o word_graph sort_paths
