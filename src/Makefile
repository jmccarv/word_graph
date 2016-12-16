# ---- Compiling ---- #

# Optimization
CFLAGS += -O2


# ---- Linking ---- #

# If you need anything special for the linker, put it here

#LDFLAGS = 


# ---- Debugging ---- #

# Include the following for profiling information
#
#CFLAGS += -pg
#LDFLAGS += -pg

# Include the following for debugging output in the binary
# CFLAGS += -g

### End of user configuration ###
#################################

CFLAGS += -Wall -finline-functions -D_GNU_SOURCE

objects = util.o
headers = inc/util.h

wg_objects = word_graph.o parse_graph.o search_graph.o
wg_headers = inc/parse_graph.h inc/search_graph.h

sp_objects = sort_paths.o parse_graph.o path_util.o
sp_headers = inc/parse_graph.h inc/path_util.h

vp_objects = view_paths.o parse_graph.o path_util.o
vp_headers = inc/parse_graph.h inc/path_util.h

all:	word_graph sort_paths view_paths

word_graph:	$(objects) $(wg_objects)
	@echo "Linking word_graph ..."
	$(CC) -o word_graph $(wg_objects) $(objects) $(LDFLAGS)

sort_paths: $(objects) $(sp_objects)
	@echo "Linking sort_paths ..."
	$(CC) -o sort_paths $(sp_objects) $(objects) $(LDFLAGS)

view_paths: $(objects) $(vp_objects)
	@echo "Linking view_paths ..."
	$(CC) -o view_paths $(vp_objects) $(objects) $(LDFLAGS) -lcurses

$(objects): $(headers)

$(wg_objects): $(wg_headers)
$(sp_objects): $(sp_headers)
$(vp_objects): $(vp_headers)

.PHONY:		clean
clean:
	-rm -f $(objects) $(wg_objects) $(sp_objects) $(vp_objects) word_graph sort_paths view_paths
