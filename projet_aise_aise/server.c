#include "server.h"
//set cle valeur
void set_value(struct Context *c, const char *key, const char *value) {
    //verouille du mutex 
    pthread_mutex_lock(&c->mutex);

    //si la cle est ==a la cle actuelle
    if (strcmp(key, c->CV.key) == 0) {
        pthread_mutex_unlock(&c->mutex);
        return;
    }
    //eviter les depassement de tampon
    strncpy(c->CV.key, key, sizeof(c->CV.key) - 1);
    //assurer la terminaison de la chaine
    c->CV.key[sizeof(c->CV.key) - 1] = '\0';

    strncpy(c->CV.value, value, sizeof(c->CV.value) - 1);
    c->CV.value[sizeof(c->CV.value) - 1] = '\0';

    //inserer dans le fichier 
    FILE *file = fopen(FICHIER, "a");
    if (file != NULL) {
        fprintf(file, "%s %s\n", key, value);
        fclose(file);
    }

    pthread_mutex_unlock(&c->mutex);
}

void incr_value(struct Context *c, const char *key) {
    //synchronisation
    pthread_mutex_lock(&c->mutex);

    int current_value = 0;
    char value[MAX_value_SIZE];
    
    //verifier si la cle existe 
    if (strcmp(key, c->CV.key) == 0) {
        //si la clé existe il recupére et inremente la valeur 
        current_value = atoi(c->CV.value);
        current_value++;
        //convertir la nouvelle valeur en chaine de caract
        sprintf(value, "%d", current_value);

        //mettre a jour la valeur dans la structure 
        strncpy(c->CV.value, value, sizeof(c->CV.value) - 1);
        c->CV.value[sizeof(c->CV.value) - 1] = '\0';

        //ajouter la cle et valeur dans le fichier 
        FILE *file = fopen(FICHIER, "a");
        if (file != NULL) {
            fprintf(file, "%s %s\n", key, value);
            fclose(file);
        }
    } else {
        //dans cette partie j'ai incremente 0+1 si la valeur n'éxiste pas 
        current_value = 1;
        sprintf(value, "%d", current_value);

        //mise a jour dans la structure
        strncpy(c->CV.key, key, sizeof(c->CV.key) - 1);
        c->CV.key[sizeof(c->CV.key) - 1] = '\0';

        strncpy(c->CV.value, value, sizeof(c->CV.value) - 1);
        c->CV.value[sizeof(c->CV.value) - 1] = '\0';

        FILE *file = fopen(FICHIER, "a");
        if (file != NULL) {
            fprintf(file, "%s %s\n", key, value);
            fclose(file);
        }
    }

    pthread_mutex_unlock(&c->mutex);
}

//cette fonction pour decrementer DECR A (--)
void decr_value(struct Context *c, const char *key) {
    pthread_mutex_lock(&c->mutex);

    int current_value = 0;
    char value[MAX_value_SIZE];

    if (strcmp(key, c->CV.key) == 0) {
        current_value = atoi(c->CV.value);
        current_value--;
        sprintf(value, "%d", current_value);

        strncpy(c->CV.value, value, sizeof(c->CV.value) - 1);
        c->CV.value[sizeof(c->CV.value) - 1] = '\0';

        FILE *file = fopen(FICHIER, "a");
        if (file != NULL) {
            fprintf(file, "%s %s\n", key, value);
            fclose(file);
        }
    } else {
        current_value = -1;
        sprintf(value, "%d", current_value);

        strncpy(c->CV.key, key, sizeof(c->CV.key) - 1);
        c->CV.key[sizeof(c->CV.key) - 1] = '\0';

        strncpy(c->CV.value, value, sizeof(c->CV.value) - 1);
        c->CV.value[sizeof(c->CV.value) - 1] = '\0';

        FILE *file = fopen(FICHIER, "a");
        if (file != NULL) {
            fprintf(file, "%s %s\n", key, value);
            fclose(file);
        }
    }

    pthread_mutex_unlock(&c->mutex);
}
//cette fonction ajoute plusieur valeur (RPUSH cle val1 val2 val3 ...)
void rpush_values(struct Context *c, const char *key, char *values[]) {
    pthread_mutex_lock(&c->mutex);
    
    //ajout dans le fichier
    FILE *file = fopen(FICHIER, "a");
    if (file != NULL) {
        fprintf(file, "%s", key);
        for (int i = 0; values[i] != NULL; ++i) {
            fprintf(file, " %s", values[i]);
        }
        fprintf(file, "\n");
        fclose(file);
    }

    pthread_mutex_unlock(&c->mutex);
}
//suprimer (DEL cle )
void delete_value(struct Context *c, const char *key) {
    pthread_mutex_lock(&c->mutex);

    if (strcmp(key, c->CV.key) == 0) {
        c->CV.key[0] = '\0';
        c->CV.value[0] = '\0';
    }

    pthread_mutex_unlock(&c->mutex);
}

int key_exists(struct Context *c, const char *key) {
    pthread_mutex_lock(&c->mutex);

    int exists = (strcmp(key, c->CV.key) == 0);

    pthread_mutex_unlock(&c->mutex);

    return exists;
}

void echo_message(int client_socket, const char *message) {
    send(client_socket, message, strlen(message), 0);
}

void rename_key(struct Context *c, const char *old_key, const char *new_key) {
    pthread_mutex_lock(&c->mutex);

    if (strcmp(old_key, c->CV.key) == 0) {
        strncpy(c->CV.key, new_key, sizeof(c->CV.key) - 1);
        c->CV.key[sizeof(c->CV.key) - 1] = '\0';
    }

    pthread_mutex_unlock(&c->mutex);
}

void *handle_client(void *arg) {
    int client_socket = *((int *)arg);
    //initialiser les membre de la structure à 0 
    struct Context server_context = {0};
    //ce tableau est utiliser pour stocker les données recues du client
    char buffer[MAX_BUFFER_SIZE];
    int bytes_received;

    while (1) {
        //reception des données du client via le socket
        bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

        if (bytes_received <= 0) {
            close(client_socket);
            break;
        }

        buffer[bytes_received] = '\0';
        //comparer si la donnée recue du client == a ces commandes donc le serveur
        //repond par :
        if (strcmp(buffer, "PING") == 0) {
            send(client_socket, "PONG", 4, 0);
        } else if (strncmp(buffer, "SET ", 4) == 0) {
            char key[MAX_KEY_SIZE], value[MAX_value_SIZE];
            sscanf(buffer, "SET %s %s", key, value);
            set_value(&server_context, key, value);
            send(client_socket, "OK", 2, 0);

        } else if (strncmp(buffer, "GET ", 4) == 0) {
            char key[MAX_KEY_SIZE];
            sscanf(buffer, "GET %s", key);

            if (strcmp(key, server_context.CV.key) == 0) {
                send(client_socket, server_context.CV.value, strlen(server_context.CV.value), 0);
            } else {
                send(client_socket, "cle non existante", 20, 0);
            }
        } else if (strncmp(buffer, "DEL ", 4) == 0) {
            char key[MAX_KEY_SIZE];
            sscanf(buffer, "DEL %s", key);
            delete_value(&server_context, key);
            send(client_socket, "OK", 2, 0);
        } else if (strncmp(buffer, "INCR ", 5) == 0) {
            char key[MAX_KEY_SIZE];
            sscanf(buffer, "INCR %s", key);
            incr_value(&server_context, key);
            send(client_socket, "OK", 2, 0);
        } else if (strncmp(buffer, "DECR ", 5) == 0) {
            char key[MAX_KEY_SIZE];
            sscanf(buffer, "DECR %s", key);
            decr_value(&server_context, key);
            send(client_socket, "OK", 2, 0);
        } else if (strncmp(buffer, "RPUSH ", 6) == 0) {
            char key[MAX_KEY_SIZE];
            sscanf(buffer, "RPUSH %s", key);

            char *token = strtok(buffer + 6, " ");
            int count = 0;
            char *values[MAX_value_SIZE];

            while (token != NULL) {
                values[count] = token;
                token = strtok(NULL, " ");
                count++;
            }

            rpush_values(&server_context, key, values);
            send(client_socket, "OK", 2, 0);

        }else if (strncmp(buffer, "EXISTS ", 7) == 0) {
            char key[MAX_KEY_SIZE];
            sscanf(buffer, "EXISTS %s", key);

            if (key_exists(&server_context, key)) {
                send(client_socket, "OK", 2, 0);
            } else {
                send(client_socket, "la cle n'existe pas", 25, 0);
            }
        } else if (strncmp(buffer, "ECHO ", 5) == 0) {
            char message[MAX_BUFFER_SIZE];
            sscanf(buffer, "ECHO %[^\n]", message);

            echo_message(client_socket, message);
        } else if (strncmp(buffer, "RENAME ", 7) == 0) {
            char old_key[MAX_KEY_SIZE], new_key[MAX_KEY_SIZE];
            sscanf(buffer, "RENAME %s %s", old_key, new_key);

            rename_key(&server_context, old_key, new_key);
            send(client_socket, "OK", 2, 0);
        }
        else {
            //si aucune de ces commandes donc affichage de :
            send(client_socket, "la commande n'existe pas", 30, 0);
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    //ecriture dans le terminale pour exécuter ./server <port>
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
    //ce commentaire saffiche pour assurer que le serveur est allumé
    printf("le serveur est en ecoute sur le port %d...\n", port);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket == -1) {
            perror("acceptation refusé");
            exit(EXIT_FAILURE);
        }

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
