#include <stdio.h>
#include <stdlib.h>

#define TABLE_DIMENSION 4

int** mount_table(char *file)
{
    int** table = (int**)malloc(TABLE_DIMENSION * sizeof(int*));
    for (int i = 0; i < 4; i++) {
        table[i] = (int*)malloc(TABLE_DIMENSION * sizeof(int));
    }
    
    FILE *table_input = fopen(file, "r");
    if (table_input == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        exit(1);
    }

    for (int i = 0; i < TABLE_DIMENSION; i++)
        for(int j = 0; j < TABLE_DIMENSION; j++)
            fscanf(table_input, "%d,", &table[i][j], &table[i][j], &table[i][j], &table[i][j]);

    return table;
}

int main()
{
    int** table = mount_table("../input/in.txt");
    for (int i = 0; i < TABLE_DIMENSION; i++) {
        for (int j = 0; j < TABLE_DIMENSION; j++) {
            printf("%d ", table[i][j]);
        }
        printf("\n");
    }
    return 0;
}