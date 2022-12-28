# ----------- FOLDERS -----------

BINARIES_DIR = src/
LIBRARIES_DIR = libs/
LIBRARIES_BUILD = build/
TESTS_DIR = tests/
TESTS_BUILD = bin/

# ----------- BINARIES -----------

INDEXER = indexer
NLPROG = nlprog
EXPERIMENTAL = experimental

# ----------- COMPILER -----------

CC = gcc
CC_FLAGS = -lm

# ----------- VARIABLES -----------

LIBRARIES = $(patsubst $(LIBRARIES_DIR)%,%,$(wildcard $(LIBRARIES_DIR)*))
TESTS = $(addprefix $(TESTS_BUILD), $(basename $(notdir $(wildcard $(TESTS_DIR)*.c))))
ALL_OBJECTS = $(addprefix $(LIBRARIES_BUILD), $(addsuffix .o, $(LIBRARIES)))
ALL_INCLUDES = $(addprefix -I$(LIBRARIES_DIR), $(LIBRARIES))

all: \
	$(INDEXER) \
	$(NLPROG) \
	$(EXPERIMENTAL)

test: $(TESTS)

# ----------- Generate all .o -----------
$(LIBRARIES_BUILD)%.o: $(LIBRARIES_DIR)*/%.c $(LIBRARIES_DIR)*/%.h
	$(eval BASENAME = $(basename $(notdir $<)))
	$(eval OUT = $(addsuffix .o, $(addprefix $(LIBRARIES_BUILD), $(BASENAME))))
	@echo -e "building \033[1;32m$(BASENAME)\033[0m"
	@mkdir -p $(LIBRARIES_BUILD)
	@$(CC) $(CC_FLAGS) -c -o $(OUT) $(ALL_INCLUDES) $<

# ----------- Generate indexer -----------
$(INDEXER): $(BINARIES_DIR)indexer.c $(ALL_OBJECTS)
	@echo -e "linking with \033[1;35mindexer\033[0m"
	@$(CC) $(CC_FLAGS) -o $(INDEXER) $(ALL_INCLUDES) $(BINARIES_DIR)indexer.c $(ALL_OBJECTS)

# ----------- Generate nlprog -----------
$(NLPROG): $(BINARIES_DIR)nlprog.c $(ALL_OBJECTS)
	@echo -e "linking with \033[1;35mnlprog\033[0m"
	@$(CC) $(CC_FLAGS) -o $(NLPROG) $(ALL_INCLUDES) $(BINARIES_DIR)nlprog.c $(ALL_OBJECTS)

# ----------- Generate experimental -----------
$(EXPERIMENTAL): $(BINARIES_DIR)experimental.c $(ALL_OBJECTS)
	@echo -e "linking with \033[1;35mexperimental\033[0m"
	@$(CC) $(CC_FLAGS) -o $(EXPERIMENTAL) $(ALL_INCLUDES) $(BINARIES_DIR)experimental.c $(ALL_OBJECTS)

# ----------- Generate tests -----------
$(TESTS_BUILD)%: $(TESTS_DIR)%.c $(ALL_OBJECTS)
	$(eval BASENAME = $(basename $(notdir $@)))
	$(eval OUT = $(addprefix $(TESTS_BUILD), $(BASENAME)))
	@echo -e "generating test \033[1;33m$(BASENAME)\033[0m"
	@mkdir -p $(TESTS_BUILD)
	@$(CC) $(CC_FLAGS) -o $(OUT) $(TESTS_DIR)$(BASENAME).c $(ALL_OBJECTS) $(ALL_INCLUDES)

clean:
	rm -rf $(INDEXER) $(NLPROG) $(EXPERIMENTAL)
	rm -rf $(LIBRARIES_BUILD) $(TESTS_BUILD)