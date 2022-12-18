LIBRARIES = \
	vector \

# ----------- FOLDERS -----------

BINARIES_DIR = src
LIBRARIES_DIR = libs
LIBRARIES_BUILD = build

# ----------- BINARIES -----------

INDEXER = indexer
NLPROG = nlprog
EXPERIMENTAL = experimental

# ----------- COMPILER -----------

all: \
	$(INDEXER) \
	$(NLPROG) \
	$(EXPERIMENTAL)
	@echo "finished"

$(INDEXER): $(BINARIES_DIR)/indexer.c

$(NLPROG): $(BINARIES_DIR)/nlprog.c

$(EXPERIMENTAL): $(BINARIES_DIR)/experimental.c

clean:
	rm -rf \
		$(INDEXER) \
		$(NLPROG) \
		$(EXPERIMENTAL)