#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_NAME_LEN 50
#define MAX_FILES 100
#define MAX_DIRS 50
#define MAX_HISTORY 100

typedef enum { FILE_TYPE, DIR_TYPE } ItemType;

typedef struct FileSystemItem {
    char name[MAX_NAME_LEN];
    ItemType type;
    struct FileSystemItem *parent;
    struct FileSystemItem *children[MAX_FILES];
    int child_count;
    bool is_locked; // Permissão de acesso
} FSItem;

FSItem *root = NULL;
FSItem *current_dir = NULL;
char command_history[MAX_HISTORY][MAX_NAME_LEN];
int history_count = 0;

// Função auxiliar para exibir mensagens formatadas
void show_message(const char *message, bool success) {
    if(success) {
        printf("\033[1;32m✓ %s\033[0m\n", message); // Verde para sucesso
    } else {
        printf("\033[1;31m✗ %s\033[0m\n", message); // Vermelho para erros
    }
}

// Função para adicionar comandos ao histórico
void add_to_history(const char *command) {
    if(history_count < MAX_HISTORY) {
        strncpy(command_history[history_count++], command, MAX_NAME_LEN);
    }
}

FSItem* create_item(const char *name, ItemType type) {
    FSItem *new_item = (FSItem*)malloc(sizeof(FSItem));
    strncpy(new_item->name, name, MAX_NAME_LEN);
    new_item->type = type;
    new_item->parent = current_dir;
    new_item->child_count = 0;
    new_item->is_locked = false; // Por padrão, não bloqueado
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

void touch(const char *name) {
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

    FSItem *new_file = create_item(name, FILE_TYPE);
    current_dir->children[current_dir->child_count++] = new_file;
    show_message("Arquivo criado com sucesso!", true);
}

void rm(const char *name) {
    for(int i = 0; i < current_dir->child_count; i++) {
        if(strcmp(current_dir->children[i]->name, name) == 0) {
            if(current_dir->children[i]->is_locked) {
                show_message("Item bloqueado! Não pode ser removido.", false);
                return;
            }

            free(current_dir->children[i]);
            for(int j = i; j < current_dir->child_count - 1; j++) {
                current_dir->children[j] = current_dir->children[j + 1];
            }
            current_dir->child_count--;
            show_message("Item removido com sucesso!", true);
            return;
        }
    }
    show_message("Item não encontrado!", false);
}

void cp(const char *source, const char *dest) {
    FSItem *source_item = NULL;
    for(int i = 0; i < current_dir->child_count; i++) {
        if(strcmp(current_dir->children[i]->name, source) == 0) {
            source_item = current_dir->children[i];
            break;
        }
    }

    if(!source_item) {
        show_message("Item de origem não encontrado!", false);
        return;
    }

    if(current_dir->child_count >= MAX_FILES) {
        show_message("Limite de arquivos/diretórios atingido!", false);
        return;
    }

    FSItem *new_item = create_item(dest, source_item->type);
    current_dir->children[current_dir->child_count++] = new_item;
    show_message("Item copiado com sucesso!", true);
}

void mv_item(const char *source, const char *dest) {
    FSItem *source_item = NULL;
    int source_index = -1;

    for(int i = 0; i < current_dir->child_count; i++) {
        if(strcmp(current_dir->children[i]->name, source) == 0) {
            source_item = current_dir->children[i];
            source_index = i;
            break;
        }
    }

    if(!source_item) {
        show_message("Item de origem não encontrado!", false);
        return;
    }

    for(int i = 0; i < current_dir->child_count; i++) {
        if(strcmp(current_dir->children[i]->name, dest) == 0) {
            show_message("Já existe um item com o nome de destino!", false);
            return;
        }
    }

    strncpy(source_item->name, dest, MAX_NAME_LEN);
    show_message("Item movido/renomeado com sucesso!", true);
}

void chmod(const char *name, bool lock) {
    for(int i = 0; i < current_dir->child_count; i++) {
        if(strcmp(current_dir->children[i]->name, name) == 0) {
            current_dir->children[i]->is_locked = lock;
            show_message(lock ? "Item bloqueado!" : "Item desbloqueado!", true);
            return;
        }
    }
    show_message("Item não encontrado!", false);
}

void history() {
    printf("\n\033[1;35mHistórico de comandos:\033[0m\n");
    for(int i = 0; i < history_count; i++) {
        printf("%d: %s\n", i + 1, command_history[i]);
    }
    printf("\n");
}

void ls() {
    printf("\n\033[1;34mConteúdo de: %s\033[0m\n", current_dir->name);
    for(int i = 0; i < current_dir->child_count; i++) {
        if(current_dir->children[i]->type == DIR_TYPE) {
            printf("📁 %s/ %s\n", current_dir->children[i]->name, 
                   current_dir->children[i]->is_locked ? "(bloqueado)" : "");
        } else {
            printf("📄 %s %s\n", current_dir->children[i]->name, 
                   current_dir->children[i]->is_locked ? "(bloqueado)" : "");
        }
    }
    printf("\n");
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
        add_to_history(command);

        if(strcmp(command, "mkdir") == 0) {
            scanf("%s", arg1);
            mkdir(arg1);
        }
        else if(strcmp(command, "touch") == 0) {
            scanf("%s", arg1);
            touch(arg1);
        }
        //else if(strcmp(command, "cd") == 0) {
           // scanf("%s", arg1);
           // cd (arg1);
        //}
        else if(strcmp(command, "ls") == 0) {
            ls();
        }
        else if(strcmp(command, "rm") == 0) {
            scanf("%s", arg1);
            rm(arg1);
        }
        else if(strcmp(command, "cp") == 0) {
            scanf("%s %s", arg1, arg2);
            cp(arg1, arg2);
        }
        else if(strcmp(command, "mv") == 0) {
            scanf("%s %s", arg1, arg2);
            mv_item(arg1, arg2);
        }
        else if(strcmp(command, "chmod") == 0) {
            scanf("%s %s", arg1, arg2);
            chmod(arg1, strcmp(arg2, "lock") == 0);
        }
        else if(strcmp(command, "history") == 0) {
            history();
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

/* root > mkdir documentos
✓ Diretório criado com sucesso!

root > touch arquivo.txt
✓ Arquivo criado com sucesso!

root > ls
Conteúdo de: root
📁 documentos/
📄 arquivo.txt

root > chmod arquivo.txt block
✓ Item bloqueado!

root > rm arquivo.txt
✗ Item bloqueado! Não pode ser removido.

root > history
Histórico de comandos:
1: mkdir
2: touch
3: ls
4: chmod
5: rm
6: history
*/