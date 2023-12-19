#ifndef SERVER_H
#define SERVER_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

//definition des données et des fonctions
#define MAX_KEY_SIZE 256
#define MAX_VALUE_SIZE 1024
#define MAX_ENTRIES 100
#define MAX_BUFFER_SIZE 1024
#define MAX_CLIENTS 10
//le fichier ou j'ai stockée les cles valeurs
#define FICHIER "Cle_Valeur.data"

//cette structure représente la paire clé valeur
struct Entry {
    char key[MAX_KEY_SIZE];
    char value[MAX_VALUE_SIZE];
};

//stocker l'état du serveur 
//utiliser mutex afin d'assurer la synchronisation entre les threads
struct Context {
    pthread_mutex_t mutex;
    struct Entry entries[MAX_ENTRIES];
};
//cette commande ajoute une cle valeur dans le contexte du serveur
void set_value(struct Context *c, const char *key, const char *value);
//cette commande envoi la valeur de la cle
void get_values(struct Context *c, int client_socket, const char *requested_key);
//cette commande supprime la cle 
void delete_value(struct Context *c, const char *key);
//incremente 
void incr_value(struct Context *c, const char *key);
//decrementer
void decr_value(struct Context *c, const char *key);
//si une cle existe ou pas 
int key_exists(struct Context *c, const char *key);
//affiche le message entré
void echo_message(int client_socket, const char *message);
//ajoute une valeur a une une cle ,fait une concatenation 
void append_value(struct Context *c, const char *key, const char *append_text);
//permet dajouter plusieurs valeurs a une clé 
void rpush_values(struct Context *c, const char *key, char *values[]);
//quitter et terminer la connection du client proprement
void quit_connection(int client_socket);
//renomer une clé existante 
void rename_key(struct Context *c, const char *key, const char *newkey);
//affixhe la liste ajouté avec RPUSH
void lrange_values(struct Context *c, int client_socket, const char *key);
//il vas traiter les commandes envoyées par le client (prise en charge de plusieurs client)
void *handle_client(void *arg);

#endif // SERVER_H
