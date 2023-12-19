#include "server.h"

void set_value(struct Context *c, const char *key, const char *value) {
    //synchronisation 
    pthread_mutex_lock(&c->mutex);

    // recherche de la cle existe dans le tab entries[] elle la met a jour
    for (int i = 0; i < MAX_ENTRIES; ++i) {
        if (strlen(c->entries[i].key) > 0 && strcmp(key, c->entries[i].key) == 0) {
            
            strncpy(c->entries[i].value, value, sizeof(c->entries[i].value) - 1);
            c->entries[i].value[sizeof(c->entries[i].value) - 1] = '\0';

            // et l'insére dans le fichier Cle_Valeur.data
            FILE *file = fopen(FICHIER, "a");
            if (file != NULL) {
                fprintf(file, "%s %s\n", key, value);
                fclose(file);
            }

            pthread_mutex_unlock(&c->mutex);
            return;
        }
    }
    //si la clé n'est pas trouvée ajouter une nouvelle clé valeur
    for (int i = 0; i < MAX_ENTRIES; ++i) {
        if (strlen(c->entries[i].key) == 0) {
            strncpy(c->entries[i].key, key, sizeof(c->entries[i].key) - 1);
            c->entries[i].key[sizeof(c->entries[i].key) - 1] = '\0';

            strncpy(c->entries[i].value, value, sizeof(c->entries[i].value) - 1);
            c->entries[i].value[sizeof(c->entries[i].value) - 1] = '\0';

            // ensuite insérer dans le fichier 
            FILE *file = fopen(FICHIER, "a");
            if (file != NULL) {
                fprintf(file, "%s %s\n", key, value);
                fclose(file);
            }

            pthread_mutex_unlock(&c->mutex);
            return;
        }
    }

    pthread_mutex_unlock(&c->mutex);
}
void get_values(struct Context *c, int client_socket, const char *requested_key) {
    pthread_mutex_lock(&c->mutex);

    char response[MAX_BUFFER_SIZE] = "";
    //recherche si la clé existe presente elle renvoie la valeur 
    for (int i = 0; i < MAX_ENTRIES; ++i) {
        if (strlen(c->entries[i].key) > 0 && strcmp(requested_key, c->entries[i].key) == 0) {
            
            strcat(response, c->entries[i].value);
            send(client_socket, response, strlen(response), 0);

            pthread_mutex_unlock(&c->mutex);
            return;
        }
    }

    // sinon elle affiche le message cle non existante
    strcat(response, "Cle non existante");
    send(client_socket, response, strlen(response), 0);

    pthread_mutex_unlock(&c->mutex);
}

void delete_value(struct Context *c, const char *key) {
    pthread_mutex_lock(&c->mutex);
    //parcourir 
    for (int i = 0; i < MAX_ENTRIES; ++i) {
        if (strlen(c->entries[i].key) > 0 && strcmp(key, c->entries[i].key) == 0) {
            //effacer la cle et la valeur en placant le caractére nul
            c->entries[i].key[0] = '\0';
            c->entries[i].value[0] = '\0';

            pthread_mutex_unlock(&c->mutex);
            return;
        }
    }

    pthread_mutex_unlock(&c->mutex);
}
void incr_value(struct Context *c, const char *key) {
    // Synchronisation
    pthread_mutex_lock(&c->mutex);
    //j"ai stocker la valeur actuelle de la clé donné
    int current_value = 0;
    char value[MAX_VALUE_SIZE];

    //il parcourt le tableau, si la clé est trouvée il incrémente 
    //sil la trouve pas il icremente a partir de 0

    int key_found = 0;
    for (int i = 0; i < MAX_ENTRIES; ++i) {
        if (strlen(c->entries[i].key) > 0 && strcmp(key, c->entries[i].key) == 0) {
            key_found = 1;

            current_value = atoi(c->entries[i].value);
            current_value++;

            sprintf(value, "%d", current_value);

            strncpy(c->entries[i].value, value, sizeof(c->entries[i].value) - 1);
            c->entries[i].value[sizeof(c->entries[i].value) - 1] = '\0';
            //ensuite ajoute dans le fichier 

            FILE *file = fopen(FICHIER, "a");
            if (file != NULL) {
                fprintf(file, "%s %s\n", key, value);
                fclose(file);
            }

            break;
        }
    }
    
    if (!key_found) {
        current_value = 1;
        sprintf(value, "%d", current_value);


        for (int i = 0; i < MAX_ENTRIES; ++i) {
            if (strlen(c->entries[i].key) == 0) {
                strncpy(c->entries[i].key, key, sizeof(c->entries[i].key) - 1);
                c->entries[i].key[sizeof(c->entries[i].key) - 1] = '\0';

                strncpy(c->entries[i].value, value, sizeof(c->entries[i].value) - 1);
                c->entries[i].value[sizeof(c->entries[i].value) - 1] = '\0';

                FILE *file = fopen(FICHIER, "a");
                if (file != NULL) {
                    fprintf(file, "%s %s\n", key, value);
                    fclose(file);
                }

                break;
            }
        }
    }

    pthread_mutex_unlock(&c->mutex);
}
//la meme chose pour la decrementation sil trouve il decremente sinon affiche -1
void decr_value(struct Context *c, const char *key) {
    // Synchronisation
    pthread_mutex_lock(&c->mutex);

    int current_value = 0;
    char value[MAX_VALUE_SIZE];

    int key_found = 0;
    for (int i = 0; i < MAX_ENTRIES; ++i) {
        if (strlen(c->entries[i].key) > 0 && strcmp(key, c->entries[i].key) == 0) {
            key_found = 1;

            current_value = atoi(c->entries[i].value);
            current_value--;

            sprintf(value, "%d", current_value);

            strncpy(c->entries[i].value, value, sizeof(c->entries[i].value) - 1);
            c->entries[i].value[sizeof(c->entries[i].value) - 1] = '\0';

            FILE *file = fopen(FICHIER, "a");
            if (file != NULL) {
                fprintf(file, "%s %s\n", key, value);
                fclose(file);
            }

            break;
        }
    }
    
    if (!key_found) {
        current_value = -1;
        sprintf(value, "%d", current_value);

        for (int i = 0; i < MAX_ENTRIES; ++i) {
            if (strlen(c->entries[i].key) == 0) {
                strncpy(c->entries[i].key, key, sizeof(c->entries[i].key) - 1);
                c->entries[i].key[sizeof(c->entries[i].key) - 1] = '\0';

                strncpy(c->entries[i].value, value, sizeof(c->entries[i].value) - 1);
                c->entries[i].value[sizeof(c->entries[i].value) - 1] = '\0';

                FILE *file = fopen(FICHIER, "a");
                if (file != NULL) {
                    fprintf(file, "%s %s\n", key, value);
                    fclose(file);
                }

                break;
            }
        }
    }

    pthread_mutex_unlock(&c->mutex);
}

//fait un parcourt si la clé existe ou non 
int key_exists(struct Context *c, const char *key) {
    pthread_mutex_lock(&c->mutex);

    for (int i = 0; i < MAX_ENTRIES; ++i) {
        if (strlen(c->entries[i].key) > 0 && strcmp(key, c->entries[i].key) == 0) {
            pthread_mutex_unlock(&c->mutex);
            return 1;
        }
    }

    pthread_mutex_unlock(&c->mutex);
    return 0; 
}
//affiche un message passé comme argument 
void echo_message(int client_socket, const char *message) {
    send(client_socket, message, strlen(message), 0);
}
//faire une concatenation
void append_value(struct Context *c, const char *key, const char *append_text) {
    pthread_mutex_lock(&c->mutex);
    //parcourir le tableau pour chercher une clé ensuite ajouter la valeur
    for (int i = 0; i < MAX_ENTRIES; ++i) {
        if (strlen(c->entries[i].key) > 0 && strcmp(key, c->entries[i].key) == 0) {
            // cette ligne est ajouter pour ajouter un espace 
            strncat(c->entries[i].value, " ", sizeof(c->entries[i].value) - strlen(c->entries[i].value) - 1);
            //ajouter le append_text 
            strncat(c->entries[i].value, append_text, sizeof(c->entries[i].value) - strlen(c->entries[i].value) - 1);
            c->entries[i].value[sizeof(c->entries[i].value) - 1] = '\0';

           
            FILE *file = fopen(FICHIER, "a");
            if (file != NULL) {
                fprintf(file, "%s %s\n", key, c->entries[i].value);
                fclose(file);
            }

            pthread_mutex_unlock(&c->mutex);
            return;
        }
    }

    
    pthread_mutex_unlock(&c->mutex);
}
//cette fonction ajoute des valeur a une liste 
void rpush_values(struct Context *c, const char *key, char *values[]) {
    pthread_mutex_lock(&c->mutex);

    for (int i = 0; i < MAX_ENTRIES; ++i) {
        if (strlen(c->entries[i].key) == 0) {
            strncpy(c->entries[i].key, key, sizeof(c->entries[i].key) - 1);
            c->entries[i].key[sizeof(c->entries[i].key) - 1] = '\0';

            //initialiser la chaine pour concatner 
            char concatenated_values[MAX_VALUE_SIZE] = "";
            for (int j = 0; values[j] != NULL; ++j) {
                strncat(concatenated_values, values[j], sizeof(concatenated_values) - strlen(concatenated_values) - 1);
                strncat(concatenated_values, " ", sizeof(concatenated_values) - strlen(concatenated_values) - 1);
            }
            concatenated_values[sizeof(concatenated_values) - 1] = '\0';

            strncpy(c->entries[i].value, concatenated_values, sizeof(c->entries[i].value) - 1);
            c->entries[i].value[sizeof(c->entries[i].value) - 1] = '\0';

           
            FILE *file = fopen(FICHIER, "a");
            if (file != NULL) {
                fprintf(file, "%s %s\n", key, concatenated_values);
                fclose(file);
            }

            break;
        }
    }

    pthread_mutex_unlock(&c->mutex);
}
//quitter la connection d'une maniére propre 
void quit_connection(int client_socket) {
  //se message saffiche dans le serveur
    printf("Client déconnecté\n");

    
    close(client_socket);
}
void rename_key(struct Context *c, const char *key, const char *newkey) {
    pthread_mutex_lock(&c->mutex);

    //remplacer key par newkey
    for (int i = 0; i < MAX_ENTRIES; ++i) {
        if (strlen(c->entries[i].key) > 0 && strcmp(key, c->entries[i].key) == 0) {
           
            strncpy(c->entries[i].key, newkey, sizeof(c->entries[i].key) - 1);
            c->entries[i].key[sizeof(c->entries[i].key) - 1] = '\0';

           

            pthread_mutex_unlock(&c->mutex);
            return;
        }
    }

    pthread_mutex_unlock(&c->mutex);
}
//recupére les valeur de RPUSH 
void lrange_values(struct Context *c, int client_socket, const char *key) {
    pthread_mutex_lock(&c->mutex);

    char response[MAX_BUFFER_SIZE] = "";
    char *token;
    int start = 0, stop = MAX_ENTRIES - 1;

    // si la clé existe 
    int key_found = 0;
    for (int i = 0; i < MAX_ENTRIES; ++i) {
        if (strlen(c->entries[i].key) > 0 && strcmp(key, c->entries[i].key) == 0) {
            key_found = 1;

            char *values = c->entries[i].value;

            token = strtok(values, " ");
            int index = 0;

            while (token != NULL) {
                if (index >= start && index <= stop) {
    
                    strcat(response, token);
                    strcat(response, " ");
                }
                token = strtok(NULL, " ");
                index++;
            }

            send(client_socket, response, strlen(response), 0);
            pthread_mutex_unlock(&c->mutex);
            return;
        }
    }

    if (!key_found) {
        strcat(response, "clé non trouvé");
        send(client_socket, response, strlen(response), 0);
    }

    pthread_mutex_unlock(&c->mutex);
}



void *handle_client(void *arg) {
    int client_socket = *((int *)arg);
    
    struct Context server_context = {0};
    
    char buffer[MAX_BUFFER_SIZE];
    int bytes_received;

    while (1) {
        
        bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

        if (bytes_received <= 0) {

            printf("Client déconnecté\n");
            close(client_socket);
            break;
        }

        buffer[bytes_received] = '\0';
       //les chiffres inqique la taille 
        if (strcmp(buffer, "PING") == 0) {
            send(client_socket, "PONG", 4, 0);
        } else if (strncmp(buffer, "SET ", 4) == 0) {
            char key[MAX_KEY_SIZE], value[MAX_VALUE_SIZE];
            sscanf(buffer, "SET %s %s", key, value);
            set_value(&server_context, key, value);
            send(client_socket, "OK", 2, 0);
        } else if (strncmp(buffer, "GET ", 4) == 0) {
            char key[MAX_KEY_SIZE];
            sscanf(buffer, "GET %s", key);
            get_values(&server_context, client_socket, key);
        }else if (strncmp(buffer, "DEL ", 4) == 0) {
            char key[MAX_KEY_SIZE];
            sscanf(buffer, "DEL %s", key);
            delete_value(&server_context, key);
            send(client_socket, "OK", 2, 0);
        }else if (strncmp(buffer, "INCR ", 5) == 0) {
            char key[MAX_KEY_SIZE];
            sscanf(buffer, "INCR %s", key);
            incr_value(&server_context, key);
            send(client_socket, "OK", 2, 0);
        }else if (strncmp(buffer, "DECR ", 5) == 0) {
            char key[MAX_KEY_SIZE];
            sscanf(buffer, "DECR %s", key);
            decr_value(&server_context, key);
            send(client_socket, "OK", 2, 0);
        }else if (strncmp(buffer, "EXISTS ", 7) == 0) {
            char key[MAX_KEY_SIZE];
            sscanf(buffer, "EXISTS %s", key);

            if (key_exists(&server_context, key)) {
                send(client_socket, "ok", 2, 0);
            } else {
                send(client_socket, "no", 2, 0);
            }
        }
        else if (strncmp(buffer, "ECHO ", 5) == 0) {
            char message[MAX_BUFFER_SIZE];
            sscanf(buffer, "ECHO %[^\n]", message);
            echo_message(client_socket, message);
        }else if (strncmp(buffer, "APPEND ", 7) == 0) {
            char key[MAX_KEY_SIZE], append_text[MAX_BUFFER_SIZE];
            sscanf(buffer, "APPEND %s %[^\n]", key, append_text);
            append_value(&server_context, key, append_text);
            send(client_socket, "OK", 2, 0);
        }else if (strncmp(buffer, "RPUSH ", 6) == 0) {
            char key[MAX_KEY_SIZE];
            sscanf(buffer, "RPUSH %s", key);

            char *token = strtok(buffer + 6, " ");
            int count = 0;
            char *values[MAX_VALUE_SIZE];

            while (token != NULL) {
                values[count] = token;
                token = strtok(NULL, " ");
                count++;
            }

            rpush_values(&server_context, key, values);
            send(client_socket, "OK", 2, 0);
        }else if (strncmp(buffer, "QUIT", 4) == 0) {
            quit_connection(client_socket);
            break;  
        }else if (strncmp(buffer, "RENAME ", 7) == 0) {
            char key[MAX_KEY_SIZE], newkey[MAX_KEY_SIZE];
            sscanf(buffer, "RENAME %s %s", key, newkey);

            rename_key(&server_context, key, newkey);
            send(client_socket, "OK", 2, 0);
        }else if (strncmp(buffer, "LRANGE ", 7) == 0) {
            char key[MAX_KEY_SIZE];
            sscanf(buffer, "LRANGE %s", key);
            lrange_values(&server_context, client_socket, key);
        }

         else {
            
            send(client_socket, "la commande n'existe pas", 30, 0);
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    //le message s"affiche quand on essaye d'éxecuter d"une maniére fausse 
    if (argc != 2) {
        fprintf(stderr, "veillez utiliser: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);

    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    pthread_t thread_id;
    struct Context server_context = {0};

    pthread_mutex_init(&server_context.mutex, NULL);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket == -1) {
        perror("création failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("erreur");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("erreur ecoute");
        exit(EXIT_FAILURE);
    }
    //ce message s"affiche quand le serveur s'allume 
    printf("le serveur est en ecoute sur le port %d...\n", port);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket == -1) {
            perror("acceptation refusé");
            exit(EXIT_FAILURE);
        }
       //quand un client se connecte au port
        printf("client connecté\n");

        if (pthread_create(&thread_id, NULL, handle_client, (void *)&client_socket) != 0) {
            perror("erreur creation thread");
            close(client_socket);
        }

        pthread_detach(thread_id);
    }

    pthread_mutex_destroy(&server_context.mutex);
    close(server_socket);

    return 0;
}
