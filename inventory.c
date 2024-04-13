#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLEN 19
#define TABLESIZE 300007

typedef struct item {
    char name[MAXLEN+1];
    int quantity;
    int saleprice;
} item;

typedef struct node {
    item* iPtr;
    struct node* next;
} node;

typedef struct hashtable {
    node** lists;
    int size;
} hashtable;

int hashfunc(char* word, int size) {
    int len = strlen(word);
    int res = 0;
    for (int i=0; i<len; i++)
    res = (1151*res + (word[i]-'a'))%size;
    return res;
}

int main(int argc, char* argv[]) {
    //open files
    FILE* input = (FILE*)fopen(argv[1], "r");
    FILE* output = (FILE*)fopen(argv[2], "w");

    int numCmds;
    fscanf(input, "%d", &numCmds);

    int complexity = 0;
    int balance = 100000;

    hashtable table;
    table.size = TABLESIZE;
    table.lists = (node**)malloc(sizeof(node*) * TABLESIZE);

    for(int i = 0; i < numCmds; i++) {
        char cmd[MAXLEN + 1];
        fscanf(input, "%s", cmd);
        
        if(strcmp(cmd, "change_price") == 0){
            // start of input
            char item[MAXLEN + 1];
            int newPrice;

            fscanf(input, "%s %d", item, &newPrice);

            int index = hashfunc(item, table.size);

            node* curr = table.lists[index];

            int count = 1;
            while(strcmp(curr->iPtr->name, item) != 0) {
                curr = curr->next;
                count++;
            }

            curr->iPtr->saleprice = newPrice;
            complexity += count;

        } else {
            char item[MAXLEN + 1];
            struct item* product;

            if(strcmp(cmd, "buy") == 0) {
                // start of input
                int quantity;
                int totalPrice;

                fscanf(input, "%s %d %d", item, &quantity, &totalPrice);
                
                int index = hashfunc(item, table.size);

                if(table.lists[index] == NULL) {
                    //create a node
                    node* newNode = (node*)malloc(sizeof(node));
                    newNode->next = NULL;
                    table.lists[index] = newNode;

                    struct item* newItem = (struct item*)malloc(sizeof(struct item));
                    strcpy(newItem->name, item);
                    newItem->quantity = quantity;
                    newItem->saleprice = totalPrice/quantity;

                    newNode->iPtr = newItem;
                    complexity++;

                    product = newItem;

                } else {
                    node* curr = table.lists[index];
                    int count = 1;
                    int found = 1;
                    while(strcmp(curr->iPtr->name, item) != 0) {
                        if(curr->next != NULL) {
                            curr = curr->next;
                            count++;
                            continue;
                        }
                        found = 0;
                        break;
                    }

                    if(!found) {
                        //create a node
                        node* newNode = (node*)malloc(sizeof(node));
                        newNode->next = table.lists[index];
                        table.lists[index] = newNode;

                        struct item* newItem = (struct item*)malloc(sizeof(struct item));
                        strcpy(newItem->name, item);
                        newItem->quantity = quantity;
                        newItem->saleprice = totalPrice/quantity;

                        newNode->iPtr = newItem;
                        complexity += 1 + count;

                        product = newItem;
                    } else {
                        product = curr->iPtr;
                        curr->iPtr->quantity += quantity;
                        complexity += count;
                    }
                }

                balance -= totalPrice;

            } else if(strcmp(cmd, "sell") == 0) {
                // start of input
                int quantity;

                fscanf(input, "%s %d", item, &quantity);
            
                int index = hashfunc(item, table.size);

                node* curr = table.lists[index];
                
                int count = 1;
                while(strcmp(curr->iPtr->name, item) != 0) {
                    if(curr->next != NULL) {
                        curr = curr->next;
                        count++;
                        continue;
                    }
                    break;
                }

                product = curr->iPtr;

                if(product->quantity < quantity) {
                    balance += product->quantity * product->saleprice;
                    product->quantity = 0;
                } else {
                    balance += quantity * product->saleprice;
                    product->quantity -= quantity;
                }

                complexity += count;
            }
            
            fprintf(output, "%s %d %d\n", item, product->quantity, balance);
        }
    }

    fprintf(output, "%d\n%d\n", balance, complexity);

    //free memory
    for(int i = 0; i < table.size; i++) {
        if(table.lists[i] != NULL) {
            node* curr = table.lists[i];
            node* temp = curr;
            while(curr != NULL) {
                curr = curr->next;
                free(temp);
                temp = curr;
            }
            free(table.lists[i]);
        }
    }
    free(table.lists);
    fclose(input);
    fclose(output);

    return 0;
}