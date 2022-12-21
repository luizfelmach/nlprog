#include <linkedlist.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Person {
    char name[20];
    int id;
};

typedef struct Person *Person_pt;

int person_cmp(const void *p1, const void *p2);
Person_pt person_new(char *name, int id);
void person_show(Person_pt p);

int main() {
    Linkedlist ll = linkedlist_new();

    char *names[5] = {"John", "Jullie", "Louis", "Peter", "Max"};

    Person_pt p;
    int i;
    for (i = 0; i < 5; i++) {
        p = person_new(names[i], i);
        linkedlist_add(ll, p);
    }

    Person_pt p1 = (Person_pt)linkedlist_search(ll, "Doe", person_cmp);
    Person_pt p2 = (Person_pt)linkedlist_search(ll, "Louis", person_cmp);

    person_show(p1);
    person_show(p2);

    linkedlist_destroy(ll, free);
    return 0;
}

int person_cmp(const void *p1, const void *p2) {
    return strcmp(((Person_pt)p1)->name, (char *)p2);
}

Person_pt person_new(char *name, int id) {
    Person_pt p = (Person_pt)calloc(1, sizeof(struct Person));
    p->id = id;
    strcpy(p->name, name);
    return p;
}

void person_show(Person_pt p) {
    if (p == NULL) {
        printf("Person does not exists\n");
        return;
    }
    printf("%d: %s\n", p->id, p->name);
}