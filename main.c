#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 10
#define ERROR_FILE_OPEN -3
#define FILENAME "output.bin"

typedef struct Node {
    int key;
    struct Node * next;
    int len;
    int jump;
} Node;

typedef struct Table {
    FILE * file;
    int max;
} Table;

int plus_memory (char ** line, int  * count, char * some_letter) {
    *count += strlen(some_letter);
    *line = realloc(*line, *count * sizeof(char));

    return 0;
}

int hex_key(int key) {
    return key % SIZE;
}

int search_elem(Node * tmp, int key) {
    while (tmp != NULL) {
        if (tmp->key == key) {
            return 1;
        }
        tmp = tmp->next;
    }

    return 0;
}

void add(Node * table[SIZE], Table * T) {
    int key = 0;

    printf("[key] => ");
    scanf("%d[^\n]", &key);
    printf("\n");
    scanf("%*c");

    int table_key = hex_key(key);

    if (search_elem(table[table_key], key) == 0) {
        int sc = 1;
        char buf[11];
        int count = 1;
        char * line = calloc(count, sizeof(char));

        printf("[info] => ");

        do {
            sc = scanf("%10[^\n]", buf);

            if (sc == -1) break;
            else if (sc == 1) {
                plus_memory(&line, &count, buf);
                strcat(line, buf);
                memset(buf, 0, sizeof(buf)*sizeof(char));

            } else {
                scanf("%*c");
                sc = -1;
            }

        } while (sc != -1);

        printf("\n");
        Node * tmp = calloc(1, sizeof *tmp);
    
        T->file = fopen(FILENAME, "a+b");
        fseek(T->file, 0, SEEK_END);
        tmp->jump = ftell(T->file);
        tmp->len = strlen(line);
        fwrite(line, sizeof(char), strlen(line), T->file);
        T->max = ftell(T->file);
        fclose(T->file);
        
        free(line);
        tmp->key = key;
        tmp->next = table[table_key];
        table[table_key] = tmp;
        printf("[+] Complite! The element was created successfully!\n\n");
    } else {
        printf("[-] Error! An element with this key has already been created!\n\n");
    }
}

void init(Node * table[SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        table[i] = NULL;
    }
}

int Saved (Node * del, Table * T) {
    int num;
    
    T->file = fopen(FILENAME, "r+b");
    fseek(T->file, sizeof(int) * (hex_key(del->key) + 1), SEEK_SET);
    fread(&num, sizeof(int), 1, T->file);
    
    while (num != 0) {
        fseek(T->file, (num + 3 * sizeof(int)), SEEK_SET);
        int bias = num;
        fread(&num, sizeof(int), 1, T->file);
        
        if (num == del->jump) {
            fread(&num, sizeof(int), 1, T->file);
            return 1;
        } else{
            fseek(T->file, bias + sizeof(int) * 3, SEEK_SET);
            fread(&num, sizeof(int), 1, T->file);
        }
    }

    fclose(T->file);

    return 0;
}

int chain (Table * T,  int last_key) {
    int num;
    int jump;
    int end;
    
    T->file = fopen(FILENAME, "r+b");
    
    fseek(T->file, 0, SEEK_END);
    end = ftell(T->file);
    
    fseek(T->file, sizeof(int) * (hex_key(last_key) + 1), SEEK_SET);
    fread(&num, sizeof(int), 1, T->file);
    
    while (num != 0) {
        jump = num + 3 * sizeof(int);
        fseek(T->file, jump, SEEK_SET);
        fread(&num, sizeof(int), 1, T->file);
    }
    
    fseek(T->file, jump, SEEK_SET);
    fwrite(&end, sizeof(int), 1, T->file);

    fclose(T->file);
    return 0;
}

void clean(Node * table[SIZE], Table * T) {   // Key    Len    Jump    Next_Elem_Jump
    Node * del;
    int num = 0;
    int first = 1;
    int last_key;
    
    for (int i = 0; i < SIZE; i++) {
        
        if (table[i] == NULL) {
            T->file = fopen(FILENAME, "r+b");
            num = 0;
            fseek(T->file, (1 + i) * sizeof(int), SEEK_SET);
            fwrite(&num, sizeof(int), 1, T->file);
            fclose(T->file);
        }
        
        while (table[i] != NULL) {
            del = table[i];
            if (Saved(del, T) != 1) {
                
                if (first == 0) {
                    chain(T, last_key);
                }

                T->file = fopen(FILENAME, "r+b");
                fseek(T->file, 0, SEEK_END);
                num = ftell(T->file);
                
                 if (first == 1) {
                    fseek(T->file, sizeof(int) * (hex_key(del->key) + 1), SEEK_SET);
                    fwrite(&num, sizeof(int), 1, T->file);
                 }
                
                fseek(T->file, 0, SEEK_END);
                fwrite(&(del->key), sizeof(int), 1, T->file);
                fwrite(&(del->len), sizeof(int), 1, T->file);
                fwrite(&(del->jump), sizeof(int), 1, T->file);
                
                num = 0;
                fwrite(&num, sizeof(int), 1, T->file);
                last_key = del->key;
                T->max = ftell(T->file);
                fclose(T->file);
            }
            
            table[i] = table[i]->next;
            free(del);
            first = 0;
        }
            first = 1;
    }
    
}

void show(Node * table[SIZE], Table * T) {
    Node * tmp;
    char * line = NULL;

    for (int i = 0; i < SIZE; i++) {
        tmp = table[i];
        if (tmp != NULL) {
            while (tmp != NULL) {
                line = calloc(tmp->len + 1, sizeof(char));
                T->file = fopen(FILENAME, "r+b");
                fseek(T->file, tmp->jump, SEEK_SET);
                fread(line, sizeof(char), tmp->len, T->file);
                fclose(T->file);
                printf("-> %p(\"%s\", %d) ", tmp, line, tmp->key);
                free(line);
                tmp = tmp->next;
            }
            printf("-> NULL");
        } else {
            printf("\t\t-> NULL");
        }
        printf("\n");
    }
    printf("\n");
}

int count_list(Node * tmp) {
    int count = 0;

    while (tmp != NULL) {
        count++;
        tmp = tmp->next;
    }

    return count;
}

int delete_elem(Node * table[SIZE]) {
    int key = 0;
    Node * tmp;
    Node * last;

    printf("[key] => ");
    scanf("%d[^\n]", &key);
    printf("\n");
    scanf("%*c");
    tmp = table[hex_key(key)];
    last = table[hex_key(key)];

    if (tmp != NULL) {
        while (tmp != NULL) {
            if (tmp->key == key) {
                if (tmp == table[hex_key(key)]) {
                    table[hex_key(key)] = tmp->next;
                    free(last);
                } else {
                    last->next = tmp->next;
                    free(tmp);
                }
                printf("[+] Complite! The element with key %d was successfully deleted!\n\n", key);

                return 0;
            } else {
                last = tmp;
                tmp = tmp->next;
            }
        }
    }

    printf("[-] Error! There is no such key!\n\n");
    return 0;
}

void synonyms(Node * table[SIZE], Table * T) {
    int key = 0;
    char * line;

    printf("[key] => ");
    scanf("%d[^\n]", &key);
    printf("\n");
    scanf("%*c");

    Node * tmp = table[hex_key(key)];
    const int size = count_list(tmp);

    if (size != 0) {
        Node * new_table[size];
        for (int i = 0; i < size; i++) {
            new_table[i] = NULL;
        }

        for (int i = 0; i < size; i++) {
            new_table[i] = tmp;
            tmp = tmp->next;
        }

        printf("[+] Complite! The synonym table for key %d was created successfully!\n\n", key);

        for (int i = 0; i < size; i++) {
            tmp = new_table[i];
            if (tmp != NULL) {
                line = calloc(tmp->len, sizeof(char));
                T->file = fopen(FILENAME, "r+b");
                fseek(T->file, tmp->jump, SEEK_SET);
                fread(line, sizeof(char), tmp->len, T->file);
                fclose(T->file);
                printf("\t\t-> %p(\"%s\", %d) ", tmp, line, tmp->key);
                free(line);
                printf("-> NULL");
            } else {
                printf("\t\t-> NULL");
            }
            printf("\n");
        }
        printf("\n");
    } else {
        printf("[-] Error! This key does not have synonyms in this table!\n\n");
    }

}

Table * Table_Init () {
    Table * T = (struct Table *)calloc(SIZE, sizeof(struct Table));
    T->file = NULL;
    T->max = 0;
    
    return T;
}

void Create_Table (Table * T) {
    int num = SIZE;
    T->file = fopen(FILENAME, "r+b");
    fwrite(&num, sizeof(int), 1, T->file);
    num = 0;
           
    for (int i = 0; i < SIZE; i++) {
        fwrite(&num, sizeof(int), 1, T->file);
    }
           
    fclose(T->file);
    printf("[+] The new file has been successfully created!\n\n");
}

void Load_Table (Node * table[SIZE], Table * T) {
    T->file = fopen(FILENAME, "r+b");
    Node * tmp;
    Node * last;
    int num;
    int table_key;
    
    for (int i = 0; i < SIZE; i++) {
        fseek(T->file, sizeof(int) * (i + 1), SEEK_SET);
        fread(&num, sizeof(int), 1, T->file);
        printf("%d ", num);
        
        if (num != 0) {
        
            last = calloc(1, sizeof *last);
        
            fseek(T->file, num, SEEK_SET);
            fread(&num, sizeof(int), 1, T->file);
            last->key = num;
            table_key = hex_key(num);
            fread(&num, sizeof(int), 1, T->file);
            last->len = num;
            fread(&num, sizeof(int), 1, T->file);
            last->jump = num;
            fread(&num, sizeof(int), 1, T->file);
            table[table_key] = last;
        
            while (num != 0) {
                tmp = calloc(1, sizeof *tmp);
            
                fseek(T->file, num, SEEK_SET);
                fread(&num, sizeof(int), 1, T->file);
                tmp->key = num;
                table_key = hex_key(num);
                fread(&num, sizeof(int), 1, T->file);
                tmp->len = num;
                fread(&num, sizeof(int), 1, T->file);
                tmp->jump = num;
                fread(&num, sizeof(int), 1, T->file);
            
                last->next = tmp;
                last = tmp;
                tmp = tmp->next;
            }
        
            last->next = NULL;
            
        }
        
    }
    
    fclose(T->file);
    printf("\n[+] The table was loaded successfully!\n\n");
}

int main() {
    int num = 0;
    Node * table[SIZE];
    int com = 0;
    int sc = 1;
    
    init(table);
    Table * T = Table_Init();
    
    T->file = fopen(FILENAME, "a+b");
    
    if (T->file == NULL) {
        printf("[-] Error! Error opening file!\n");
        exit(ERROR_FILE_OPEN);
    }
    
    fclose(T->file);
    
    T->file = fopen(FILENAME, "r+b");
    fread(&num, sizeof(int), 1, T->file);
    fclose(T->file);
    
    if (num == 0) {
        Create_Table(T);
    } else {
        Load_Table(table, T);
    }

    show(table, T);

    do {

        printf("{1} -> Add\t{3} -> Synonyms\n{2} -> Delete\t{4} -> Show\t{5} -> Exit\n\n|=> ");
        sc = scanf("%d", &com);
        printf("\n");

        if (sc == -1) break;
        else if (sc == 1) {
            switch (com) {
                case 1:
                    add(table, T);
                    break;
                case 2:
                    delete_elem(table);
                    break;
                case 3:
                    synonyms(table, T);
                    break;
                case 4:
                    show(table, T);
                    break;
                case 5:
                    sc = -1;
                    break;
                default:
                    printf("[-] Error! Enter a number {1-5}!\n\n");
            }
        } else {
            scanf("%*c");
        }

    } while (sc != -1);

    clean(table, T);
    free(T);
    return 0;
}
