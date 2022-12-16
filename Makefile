LIBRARYS = \
	test \
	vector \
	article

# ----------- FOLDERS -----------

BINARYS_DIR = src
LIBRARYS_DIR = libs

# ----------- OUTPUTS -----------

INDEXER = indexer
NLPROG = nlprog
EXPERIMENTAL = experimental

# ----------- COMPILER -----------

all: \
	$(INDEXER) \
	$(NLPROG) \
	$(EXPERIMENTAL)
	@echo "finished"

$(INDEXER): $(BINARYS_DIR)/indexer.c

$(NLPROG): $(BINARYS_DIR)/nlprog.c

$(EXPERIMENTAL): $(BINARYS_DIR)/experimental.c

clean:
	rm -rf \
		$(INDEXER) \
		$(NLPROG) \
		$(EXPERIMENTAL)