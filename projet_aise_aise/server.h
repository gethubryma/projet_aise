#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_CLIENTS 10
#define MAX_BUFFER_SIZE 1024
#define MAX_KEY_SIZE 128
#define MAX_value_SIZE 128
#define MAX_VALUES 100
#define FICHIER "Cle_Valeur.data"

struct cleValeur {
    char key[MAX_KEY_SIZE];
    char value[MAX_value_SIZE];
    
};

struct Context {
    struct cleValeur CV;
    pthread_mutex_t mutex;
};
void set_value(struct Context *c, const char *key, const char *value);
void incr_value(struct Context *c, const char *key);
void decr_value(struct Context *c, const char *key);
void rpush_values(struct Context *c, const char *key, char *values[]);
void delete_value(struct Context *c, const char *key);
void *handle_client(void *arg);

#endif // SERVER_H
