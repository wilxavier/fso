#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_NAME_LEN 50
#define MAX_FILES 100
#define MAX_DIRS 50

//O trecho abaixo até a linha 20 é a base para implementar 
//operações como navegação entre diretórios, criação de arquivos/diretórios, 
//e listagem de conteúdos.
typedef enum { FILE_TYPE, DIR_TYPE} ItemType;

typedef struct FileSystemItem
{
    char name[MAX_NAME_LEN];
    ItemType type;
    struct FileSystemItem *parent;
    struct FileSystemItem *children[MAX_FILES];
    int child_count;
} FSItem;

FSItem *root = NULL;
FSItem *current_dir = NULL;

// Função auxiliar para exibir mensagens formatadas
void show_message(const char *message, bool success) {
    if(success) {
        //\033 é o caractere de escape. [1;32m define o estilo do texto:.  1 significa "negrito". 
        //32 é o código da cor verde. \033[0m no final reseta a formatação para o padrão do terminal.
        printf("\033[1;32m✓ %s\033[0m\n", message); // Verde para sucesso
    } else {
        printf("\033[1;31m✗ %s\033[0m\n", message); // Vermelho para erros
    }
}

FSItem* create_item(const char *name, ItemType type) {
    FSItem *new_item = (FSItem*)malloc(sizeof(FSItem));
    strncpy(new_item->name, name, MAX_NAME_LEN);
    new_item->type = type;
    new_item->parent = current_dir;
    new_item->child_count = 0;
    return new_item;
}

void init_filesystem() {
    root = create_item("root", DIR_TYPE);
    current_dir = root;
    show_message("Sistema de arquivos inicializado!", true);
}

void mkdir(const char *name) {
    if(current_dir->child_count >= MAX_FILES) {
        show_message("Limite de arquivos/diretórios atingido!", false);
        return;
    }

    for(int i = 0; i < current_dir->child_count; i++) {
        if(strcmp(current_dir->children[i]->name, name) == 0) {
            show_message("Já existe um item com esse nome!", false);
            return;
        }
    }

    FSItem *new_dir = create_item(name, DIR_TYPE);
    current_dir->children[current_dir->child_count++] = new_dir;
    show_message("Diretório criado com sucesso!", true);
}

void cd(const char *name) {
    if(strcmp(name, "..") == 0) {
        if(current_dir != root) {
            current_dir = current_dir->parent;
            show_message("Retornou ao diretório anterior", true);
        }
        return;
    }

    for(int i = 0; i < current_dir->child_count; i++) {
        if(current_dir->children[i]->type == DIR_TYPE && 
           strcmp(current_dir->children[i]->name, name) == 0) {
            current_dir = current_dir->children[i];
            show_message("Diretório alterado com sucesso!", true);
            return;
        }
    }
    show_message("Diretório não encontrado!", false);
}

void ls() {
    printf("\n\033[1;34mConteúdo de: %s\033[0m\n", current_dir->name);
    for(int i = 0; i < current_dir->child_count; i++) {
        if(current_dir->children[i]->type == DIR_TYPE) {
            printf("📁 %s/\n", current_dir->children[i]->name);
        } else {
            printf("📄 %s\n", current_dir->children[i]->name);
        }
    }
    printf("\n");
}

void mv(const char *old_name, const char *new_name) {
    for(int i = 0; i < current_dir->child_count; i++) {
        if(strcmp(current_dir->children[i]->name, old_name) == 0) {
            strncpy(current_dir->children[i]->name, new_name, MAX_NAME_LEN);
            show_message("Item renomeado com sucesso!", true);
            return;
        }
    }
    show_message("Item não encontrado para renomear!", false);
}

void print_prompt() {
    printf("\n\033[1;36m%s > \033[0m", current_dir->name);
}

int main() {
    init_filesystem();
    char command[20];
    char arg1[MAX_NAME_LEN];
    char arg2[MAX_NAME_LEN];

    while(true) {
        print_prompt();
        scanf("%s", command);

        if(strcmp(command, "mkdir") == 0) {
            scanf("%s", arg1);
            mkdir(arg1);
        }
        else if(strcmp(command, "cd") == 0) {
            scanf("%s", arg1);
            cd(arg1);
        }
        else if(strcmp(command, "ls") == 0) {
            ls();
        }
        else if(strcmp(command, "mv") == 0) {
            scanf("%s %s", arg1, arg2);
            mv(arg1, arg2);
        }
        else if(strcmp(command, "exit") == 0) {
            show_message("Saindo do sistema de arquivos...", true);
            break;
        }
        else {
            show_message("Comando desconhecido!", false);
        }
    }

    return 0;
}

/*
mkdir <nome>      - Cria novo diretório
cd <nome>         - Entra em diretório
cd ..             - Volta ao diretório anterior
ls                - Lista conteúdo do diretório atual
mv <antigo> <novo> - Renomeia arquivo/diretório
exit              - Sai do sistema
*/




