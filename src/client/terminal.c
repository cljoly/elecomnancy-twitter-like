#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdarg.h>
#include <limits.h>

#include "client.h"
#include "tools.h"
#include "terminal.h"

static char* commands[] = {
        "Quitter",
        "Créer un compte",
        "Se connecter",
        "Envoyer un gazouilli         ",
        "Relayer un gazouilli         ",
        "Suivre un utilisateur        ",
        "Ne plus suivre un utilisateur",
        "Utilisateurs suivis          ",
        "Mes Abonnés                  ",
        "Suivre une thématique        ",
        "Ne plus suivre une thématique",
        "Thématique suivies           ",
        "Déconnexion                  "
};

static const unsigned int commands_count = sizeof(commands) / sizeof(char*);

void clear_all_terminal() {
    printf("\e[1;1H\e[2J");
}

void clear_terminal_exceptHeader() {
    printf("\033[9;1H");
}

void print_menu(int connected) {
    clear_terminal_exceptHeader();

    // get terminal info
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);


    unsigned int first_command_index = 0;
    unsigned int last_command_index = 2;
    if (connected != -1) {
        first_command_index = 3;
        last_command_index = commands_count - 1;
    }

    // print a line
    for (unsigned int j = 0; j < w.ws_col; j++) {
        printf("-");
    }
    printf("\n");
    unsigned int printed_line_chars = 0;
    for (unsigned int i = first_command_index; i <= last_command_index; i++) {
        printed_line_chars += 5 + strlen(commands[i]);
        printf("%2d - %s\t", i , commands[i]);
        if(((i+1-first_command_index) % 5 == 0) && (i+1 <= last_command_index)) {
            printf("\n");
        }
    }
    printf("\n");
    // print a line
    for (unsigned int j = 0; j < w.ws_col; j++) {
        printf("-");
    }
    printf("\n\n");
}

void print_message(message_type_t type, const char* format, ...) {
    switch(type) {
        case SUCCESS:
            // print in green
            printf("\033[1;32m");
            break;
        case ERROR:
            // print in yellow
            printf("\033[01;33m");
            break;
        case FATAL_ERROR:
            // print in red
            printf("\033[1;31m");
            break;
        case DEBUG:
            // print in grey
            printf("\033[1;0m");
            break;
        case INFO:
        default:
            // print in blue
            printf("\033[0;34m");
            break;
    }
    va_list args;
    va_start(args, format);
    printf(format, args);
    va_end(args);
    printf("\033[0m");
}

void print_message_above(message_type_t type, const char* format, ...) {
    printf("\033[8;1H");
    va_list args;
    va_start(args, format);
    print_message(type, format, args);
    va_end(args);
}

void print_message_below(message_type_t type, const char* format, ...) {
    printf("\033[16;1H");
    va_list args;
    va_start(args, format);
    print_message(type, format, args);
    va_end(args);

}

/**
 * Demande à l'utilisateur l'action à effectuer et la retourne
 * Affiche le menu, en appellant \seealso print_menu()
 * @return le numéro de la commande demandée
 */
unsigned int prompt_user(int connected) {
    print_menu(connected);
    // prompt
    //printf("\e[13;39H\e[2J");
    printf("\033[14;1H");
    printf("\n");
    printf("\033[14;1H");
    printf("> Quelle action voulez-vous effectuer ? ");

    // Lecture des données
    char buf[3] = {'\0'};
    read_stdin(buf, 3);

    // conversion et test
    char* endptr;
    unsigned int input = (unsigned int) strtoul(buf, &endptr, 10);
    if (endptr == buf) {
        print_message_above(ERROR, "Veuillez entrer un numéro de commande\n");
        return UINT_MAX;
    } else if (input > commands_count) {
        print_message_above(ERROR, "Veuillez entrer une commande valide\n");
        return UINT_MAX;
    } else {
        return input;
    }
}

/**
 * Demande la valeur d'un paramètre d'une requête à l'utilisateur
 * @param prompt Le message affiché pour expliquer la demande
 * @param result L'entrée tappée par l'utilisateur
 * @return 1 en cas d'erreur, 0 sinon
 */
int prompt_user_for_parameter(const char* prompt, char* result) {
    printf("%s : ", prompt);
    size_t length = read_stdin(result, MAXDATASIZE);
    //Si la longueur vaut 0, c'est une erreur
    return length == 0;
}


void printTitle() {
    printf("\033[0;36m");
    printf(" __  __      _____          _ _   _\n");
    printf("|  \\/  |_   |_   _|_      _(_) |_| |_ ___ _ __\n");
    printf("| |\\/| | | | || | \\ \\ /\\ / / | __| __/ _ \\ '__|\n");
    printf("| |  | | |_| || |  \\ V  V /| | |_| ||  __/ |\n");
    printf("|_|  |_|\\__, ||_|   \\_/\\_/ |_|\\__|\\__\\___|_|\n");
    printf("        |___/\n");
    printf("\033[0m");
}