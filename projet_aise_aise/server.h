#ifndef SERVER_H
#define SERVER_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_KEY_SIZE 256
#define MAX_VALUE_SIZE 1024
#define MAX_ENTRIES 100
#define MAX_BUFFER_SIZE 1024
#define MAX_CLIENTS 10
#define FICHIER "data.txt"

struct Entry {
    char key[MAX_KEY_SIZE];
    char value[MAX_VALUE_SIZE];
};

struct Context {
    pthread_mutex_t mutex;
    struct Entry entries[MAX_ENTRIES];
};

void set_value(struct Context *c, const char *key, const char *value);
void get_values(struct Context *c, int client_socket, const char *requested_key);
void delete_value(struct Context *c, const char *key);
void incr_value(struct Context *c, const char *key);
void decr_value(struct Context *c, const char *key);
int key_exists(struct Context *c, const char *key);
void echo_message(int client_socket, const char *message);
void append_value(struct Context *c, const char *key, const char *append_text);
void rpush_values(struct Context *c, const char *key, char *values[]);
void quit_connection(int client_socket);
void rename_key(struct Context *c, const char *key, const char *newkey);

void *handle_client(void *arg);

#endif // SERVER_H
