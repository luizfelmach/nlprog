#include <libgen.h>
#include <map.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char path[128];

typedef struct {
    char filename[128];
    char class[32];
} train_item;

int train_item_get(FILE *file, train_item *item);

typedef struct {
    char name[1024];
    int count;
} Word;

typedef Word *Word_pt;

void word_add(Map map, char *word);
int word_cmp(const void *data1, const void *data2);
void word_show(void *data);

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("error: missing parameters.");
        exit(1);
    }

    strcpy(path, argv[1]);
    dirname(path);

    printf("current folder context: %s\n", path);

    FILE *train = fopen(argv[1], "r");

    if (!train) {
        printf("error: can not open file.");
        exit(1);
    }

    Map word_index = map_new();
    train_item item;
    char word[1024];

    while (train_item_get(train, &item)) {
        FILE *data = fopen(item.filename, "r");

        while (fscanf(data, "%s", word) > 0) {
            word_add(word_index, word);
        }

        fclose(data);
    }

    map_foreach(word_index, word_show);

    fclose(train);
    map_destroy(word_index, free);

    return 0;
}

int train_item_get(FILE *file, train_item *item) {
    char path_to_file[128];
    int result = fscanf(file, "%s %s%*c", path_to_file, item->class) > 0;
    if (!result) {
        return 0;
    }
    strcpy(item->filename, path);
    strcat(item->filename, "/");
    strcat(item->filename, path_to_file);
    return result;
}

void word_add(Map map, char *word) {
    Word_pt w = (Word_pt)map_get(map, word, word_cmp);

    if (!w) {
        Word_pt new_word = (Word_pt)calloc(1, sizeof(Word));
        new_word->count = 1;
        strcpy(new_word->name, word);
        map_insert(map, word, new_word);
    } else {
        w->count += 1;
    }
}

int word_cmp(const void *data1, const void *data2) {
    Word_pt w = (Word_pt)data1;
    return strcmp(w->name, (char *)data2);
}

void word_show(void *data) {
    Word_pt w = (Word_pt)data;
    printf("%s: %d\n", w->name, w->count);
}