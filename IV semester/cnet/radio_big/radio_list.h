#ifndef MIM_RADIO_LIST_H_
#define MIM_RADIO_LIST_H_

#include <time.h>
#include "common.h"
#include "err.h"

#define TIMEOUT 20

typedef struct radio_station radio_station;

struct radio_station {
    char* host;
    uint16_t data_port;
    char* name;

    int station_socket;
    struct ip_mreq ip_mreq;
    time_t last_lookup_time;

    radio_station* next_station;
};

radio_station* create_new_radio_station(char* host, uint16_t data_port, char* name) {

    radio_station* res = calloc(1, sizeof(radio_station));
    ENSURE(res != NULL);

    res->host = calloc(strlen(host) + 1, sizeof(char));
    ENSURE(res->host != NULL);
    strcpy(res->host, host);

    res->data_port = data_port;

    res->name = calloc(strlen(name) + 1, sizeof(char));
    ENSURE(res->name != NULL);
    strcpy(res->name, name);

    res->station_socket = open_socket_udp();
    set_port_reuse(res->station_socket);
    bind_socket(res->station_socket, res->data_port);

    res->ip_mreq.imr_multiaddr.s_addr = inet_addr(host);
    res->ip_mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    CHECK_ERRNO(inet_pton(AF_INET, host, &res->ip_mreq.imr_multiaddr));
    CHECK_ERRNO(setsockopt(res->station_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP,
        (void*)&res->ip_mreq, sizeof(res->ip_mreq)));

    res->last_lookup_time = time(NULL);
    res->next_station = NULL;

    return res;
}

void remove_radio_station(radio_station** first, radio_station* station) {

    if (*first == NULL) {
        return;
    }

    radio_station* previous;

    if (station == *first) {
        previous = *first;
        *first = (*first)->next_station;

        free(previous->host);
        previous->host = NULL;
        free(previous->name);
        previous->name = NULL;

        CHECK_ERRNO(setsockopt(previous->station_socket, IPPROTO_IP, IP_DROP_MEMBERSHIP,
            (void*)&previous->ip_mreq, sizeof(previous->ip_mreq)));
        CHECK_ERRNO(close(previous->station_socket));

        free(previous);
        previous = NULL;

        return;
    }

    radio_station* temp = (*first)->next_station;
    previous = *first;

    while (temp != NULL && temp->next_station != NULL) {
        if (temp == station) {
            previous->next_station = temp->next_station;

            free(temp->host);
            temp->host = NULL;
            free(temp->name);
            temp->name = NULL;

            CHECK_ERRNO(setsockopt(temp->station_socket, IPPROTO_IP, IP_DROP_MEMBERSHIP,
                (void*)&temp->ip_mreq, sizeof(temp->ip_mreq)));
            CHECK_ERRNO(close(temp->station_socket));

            free(temp);
            temp = NULL;

            return;
        }
        else {
            previous = temp;
            temp = temp->next_station;
        }
    }

    if (temp == station) {
        free(temp->host);
        temp->host = NULL;
        free(temp->name);
        temp->name = NULL;

        CHECK_ERRNO(setsockopt(temp->station_socket, IPPROTO_IP, IP_DROP_MEMBERSHIP,
            (void*)&temp->ip_mreq, sizeof(temp->ip_mreq)));
        CHECK_ERRNO(close(temp->station_socket));

        free(temp);
        temp = NULL;

        if (previous != NULL) {
            previous->next_station = NULL;
        }
        return;
    }
}

radio_station* find_radio_station_by_name(radio_station** first, char* name) {

    if (*first == NULL) {
        return NULL;
    }

    radio_station* temp = *first;
    while (temp != NULL) {
        if (strcmp(temp->name, name) == 0) {
            return temp;
        }
        temp = temp->next_station;
    }

    return NULL;
}

// Update lookup time of station with given arguments.
// We pass atomic_bool show_ui to the function to mark that if there was a change
// in radio list (we added a new radio) to mark that we want to show ui for TCP connections once again.
// If station with given arguments does not exist, create new one.
// Return pointer to station with given arguments (existing or a new one).
radio_station* update_lookup_time(radio_station** first, char* host, uint16_t data_port, char* name, atomic_bool *show) {

    if (*first == NULL) {
        radio_station* new_station = create_new_radio_station(host, data_port, name);
        *show = true;
        *first = new_station;
        return new_station;
    }

    radio_station* temp = *first;
    while (temp != NULL) {
        if (strcmp(temp->name, name) == 0 &&
            strcmp(temp->host, host) == 0 &&
            temp->data_port == data_port) {
            temp->last_lookup_time = time(NULL);
            return temp;
        }

        else if ((strcmp(temp->name, name) == 0) ||
            (temp->next_station != NULL && strcmp(temp->name, name) < 0 && strcmp(temp->next_station->name, name) > 0)) {
            radio_station* new_station = create_new_radio_station(host, data_port, name);
            *show = true;
            new_station->next_station = temp->next_station;
            temp->next_station = new_station;
            return new_station;
        }
        else if (temp->next_station == NULL && strcmp(temp->name, name) < 0) {
            radio_station* new_station = create_new_radio_station(host, data_port, name);
            *show = true;
            temp->next_station = new_station;
            return new_station;
        }
        else if (temp == *first && strcmp(temp->name, name) > 0) {
            radio_station* new_station = create_new_radio_station(host, data_port, name);
            new_station->next_station = temp;
            *show = true;
            *first = new_station;
            return new_station;
        }

        temp = temp->next_station;
    }

    return NULL;
}

// Removes all inactive stations except the one that is currently playing.
// Returns true if the station is still active, false otherwise.
bool remove_inactive_stations(radio_station** first, radio_station* station, pthread_mutex_t* radio_mut, atomic_bool* show) {
    // PO KAZDORAZOWYM USUNIECU STACJI WYSWIETL UI, CZYLI CONSTRUCT USER INTERFACE
    radio_station* temp = *first;
    bool res = true;

    while (temp != NULL) {

        if (time(NULL) - temp->last_lookup_time >= TIMEOUT) {
            if (temp != station) {
                radio_station* to_delete = temp;
                temp = temp->next_station;
                remove_radio_station(first, to_delete);
                *show = true;
                CHECK_ERRNO(pthread_mutex_unlock(radio_mut));
                while((*show) != false) {}
                CHECK_ERRNO(pthread_mutex_lock(radio_mut));

            }
            else {
                res = false;
                temp = temp->next_station;
            }
        }
        else {
            temp = temp->next_station;
        }
    }

    return res;
}

radio_station* get_previous_radio_station(radio_station** first, radio_station* station) {

    if (*first == NULL) {
        return NULL;
    }

    if (station == *first) {
        return station;
    }

    radio_station* temp = *first;

    while (temp != NULL) {
        if (temp->next_station == station) {
            return temp;
        }
        temp = temp->next_station;
    }

    return NULL;
}

char* construct_user_interface(radio_station* first, radio_station* current_station) {

    size_t buf_size = INITIAL_UI_SIZE;
    char* buf = calloc(buf_size, sizeof(char));
    ENSURE(buf != NULL);
    radio_station* temp = first;

    sprintf(buf, "%s\r\n\r\n", UI_LINE);
    sprintf(buf + strlen(buf), "%s\r\n\r\n", UI_NAME);
    sprintf(buf + strlen(buf), "%s\r\n\r\n", UI_LINE);
    while (temp != NULL) {
        if (strlen(buf) + strlen(temp->name) + 5 > buf_size) {
            buf_size *= 2;
            buf = realloc(buf, buf_size);
            ENSURE(buf != NULL);
        }
        if (temp == current_station) {
            sprintf(buf + strlen(buf), " > %s\r\n\r\n", temp->name);
        }
        else {
            sprintf(buf + strlen(buf), "%s\r\n\r\n", temp->name);
        }
        temp = temp->next_station;
    }
    if (strlen(buf) + strlen(UI_LINE) + 20 > buf_size) {
        buf_size *= 2;
        buf = realloc(buf, buf_size);
        ENSURE(buf != NULL);
    }
    sprintf(buf + strlen(buf), "%s\r\n\xFF\xFB\x01\xFF\xFD\x01\xFF\xFD\x22\r\n", UI_LINE);

    return buf;
}

#endif // MIM_RADIO_LIST_H_
