/*
 * Smart Emergency Dispatch Simulator using pthreads (Improved User-Friendly Version)
 * Author: [Your Name]
 * Description: Simulates an emergency response system using code-based emergency input.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_SERVICES 4
#define MAX_EMERGENCIES 40
#define MAX_UNITS 10
#define MAX_QUEUE 100

pthread_mutex_t queue_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_cond = PTHREAD_COND_INITIALIZER;

const char *SERVICES[] = {"Police", "Ambulance", "Fire Truck", "Animal Control", "Hazmat"};
int available_units[5] = {MAX_UNITS, MAX_UNITS, MAX_UNITS, MAX_UNITS, MAX_UNITS};

typedef struct {
    char id[5];
    char type[50];
    int priority;
    char services[MAX_SERVICES][20];
    int service_count;
} Emergency;

typedef struct Job {
    Emergency emergency;
    struct Job *next;
} Job;

Job *job_queue = NULL;
Emergency emergencies[MAX_EMERGENCIES];
int emergency_count = 0;

void add_emergency(const char *id, const char *type, int priority, const char *services[], int service_count) {
    Emergency *e = &emergencies[emergency_count++];
    strcpy(e->id, id);
    strcpy(e->type, type);
    e->priority = priority;
    for (int i = 0; i < service_count; i++) {
        strcpy(e->services[i], services[i]);
    }
    e->service_count = service_count;
}

void enqueue_job(Emergency emergency) {
    Job *new_job = malloc(sizeof(Job));
    new_job->emergency = emergency;
    new_job->next = NULL;

    pthread_mutex_lock(&queue_lock);
    if (!job_queue || emergency.priority > job_queue->emergency.priority) {
        new_job->next = job_queue;
        job_queue = new_job;
    } else {
        Job *curr = job_queue;
        while (curr->next && curr->next->emergency.priority >= emergency.priority) {
            curr = curr->next;
        }
        new_job->next = curr->next;
        curr->next = new_job;
    }
    pthread_cond_signal(&queue_cond);
    pthread_mutex_unlock(&queue_lock);
}

void *service_unit(void *arg) {
    char *service_name = (char *)arg;
    int index = (strcmp(service_name, "Police") == 0) ? 0 :
                (strcmp(service_name, "Ambulance") == 0) ? 1 :
                (strcmp(service_name, "Fire Truck") == 0) ? 2 :
                (strcmp(service_name, "Animal Control") == 0) ? 3 : 4;

    while (1) {
        pthread_mutex_lock(&queue_lock);
        while (!job_queue) {
            pthread_cond_wait(&queue_cond, &queue_lock);
        }

        Job *prev = NULL;
        Job *curr = job_queue;
        Job *target_job = NULL;

        while (curr) {
            for (int i = 0; i < curr->emergency.service_count; i++) {
                if (strcmp(curr->emergency.services[i], service_name) == 0) {
                    target_job = curr;
                    break;
                }
            }
            if (target_job) break;
            prev = curr;
            curr = curr->next;
        }

        if (target_job) {
            if (!prev) job_queue = target_job->next;
            else prev->next = target_job->next;

            pthread_mutex_unlock(&queue_lock);

            printf("[%s Unit] Dispatched for emergency: %s\n", service_name, target_job->emergency.type);
            sleep(3);

            pthread_mutex_lock(&queue_lock);
            available_units[index]++;
            pthread_mutex_unlock(&queue_lock);

            printf("[%s Unit] Completed emergency: %s\n", service_name, target_job->emergency.type);
            free(target_job);
        } else {
            pthread_mutex_unlock(&queue_lock);
        }
        sleep(1);
    }
    return NULL;
}

Emergency *get_emergency_by_code(const char *code) {
    for (int i = 0; i < emergency_count; i++) {
        if (strcasecmp(emergencies[i].id, code) == 0) {
            return &emergencies[i];
        }
    }
    return NULL;
}

void init_emergencies() {
    const char *s1[] = {"Fire Truck"};
    add_emergency("E01", "Fire (Residential)", 3, s1, 1);
    const char *s2[] = {"Police"};
    add_emergency("E02", "Burglary", 2, s2, 1);
    const char *s3[] = {"Ambulance"};
    add_emergency("E03", "Heart Attack", 5, s3, 1);
    const char *s4[] = {"Ambulance", "Police"};
    add_emergency("E04", "Road Accident", 4, s4, 2);
    const char *s5[] = {"Fire Truck", "Police"};
    add_emergency("E05", "Gas Leak", 4, s5, 2);
    const char *s6[] = {"Police", "Animal Control"};
    add_emergency("E06", "Aggressive Dog", 2, s6, 2);
    const char *s7[] = {"Animal Control"};
    add_emergency("E07", "Stray Animal", 1, s7, 1);
    const char *s8[] = {"Ambulance", "Police"};
    add_emergency("E08", "Drug Overdose", 5, s8, 2);
    const char *s9[] = {"Fire Truck"};
    add_emergency("E09", "Forest Fire", 5, s9, 1);
    const char *s10[] = {"Police", "Fire Truck", "Ambulance"};
    add_emergency("E10", "Building Collapse", 5, s10, 3);
    const char *s11[] = {"Hazmat", "Fire Truck"};
    add_emergency("E11", "Chemical Spill", 5, s11, 2);
    const char *s12[] = {"Police", "Ambulance"};
    add_emergency("E12", "Shooting", 5, s12, 2);
    const char *s13[] = {"Animal Control", "Hazmat"};
    add_emergency("E13", "Animal Poisoning Incident", 3, s13, 2);
    const char *s14[] = {"Police"};
    add_emergency("E14", "Vandalism", 1, s14, 1);
    const char *s15[] = {"Ambulance"};
    add_emergency("E15", "Seizure", 4, s15, 1);
    const char *s16[] = {"Police", "Fire Truck", "Ambulance"};
    add_emergency("E16", "Flood", 5, s16, 3);
    const char *s17[] = {"Fire Truck", "Ambulance", "Animal Control"};
    add_emergency("E17", "Tornado", 5, s17, 3);
    const char *s18[] = {"Police", "Fire Truck", "Hazmat"};
    add_emergency("E18", "Bomb Threat", 5, s18, 3);
    const char *s19[] = {"Animal Control", "Police"};
    add_emergency("E19", "Wildlife Emergency", 3, s19, 2);
    const char *s20[] = {"Police", "Ambulance"};
    add_emergency("E20", "Hostage Situation", 5, s20, 2);
    const char *s21[] = {"Ambulance", "Hazmat"};
    add_emergency("E21", "Chemical Burn", 5, s21, 2);
    const char *s22[] = {"Police", "Fire Truck"};
    add_emergency("E22", "Power Outage", 3, s22, 2);  // New emergency
    const char *s23[] = {"Fire Truck", "Ambulance", "Police"};
    add_emergency("E23", "Earthquake", 5, s23, 3);  // New emergency
    const char *s24[] = {"Hazmat", "Fire Truck"};
    add_emergency("E24", "Radioactive Leak", 5, s24, 2);  // New emergency
    const char *s25[] = {"Fire Truck", "Police", "Ambulance"};
    add_emergency("E25", "Tsunami", 5, s25, 3);  // New emergency
    const char *s26[] = {"Police", "Ambulance", "Fire Truck"};
    add_emergency("E26", "Evacuation", 5, s26, 3);  // New emergency
}

int main() {
    init_emergencies();

    pthread_t threads[5];
    pthread_create(&threads[0], NULL, service_unit, (void *)"Police");
    pthread_create(&threads[1], NULL, service_unit, (void *)"Ambulance");
    pthread_create(&threads[2], NULL, service_unit, (void *)"Fire Truck");
    pthread_create(&threads[3], NULL, service_unit, (void *)"Animal Control");
    pthread_create(&threads[4], NULL, service_unit, (void *)"Hazmat");

    int choice;
    char input[100];

    printf("========== SMART EMERGENCY DISPATCH SYSTEM ==========\n");

    while (1) {
        printf("\nMain Menu:\n");
        printf("1. Report an Emergency (by code)\n");
        printf("2. List All Emergency Codes & Types\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        fgets(input, sizeof(input), stdin);
        choice = atoi(input);

        if (choice == 1) {
            printf("\nEnter emergency code (e.g., E01): ");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = '\0';

            Emergency *e = get_emergency_by_code(input);
            if (e) {
                enqueue_job(*e);
                printf("[OK] Emergency '%s' (%s) reported and queued for dispatch.\n", e->id, e->type);
            } else {
                printf("[X] Unknown emergency code. Use option 2 to view the list.\n");
            }

        } else if (choice == 2) {
            printf("\n[List] Emergency Code List:\n");
            for (int i = 0; i < emergency_count; i++) {
                printf("  %s - %s\n", emergencies[i].id, emergencies[i].type);
            }

        } else if (choice == 3) {
            printf("Exiting Emergency Dispatch System. Stay safe!\n");
            exit(0);
        } else {
            printf("Invalid choice. Please select a valid option.\n");
        }
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
/*
 * Smart Emergency Dispatch Simulator using pthreads (Improved User-Friendly Version)
 * Author: [Your Name]
 * Description: Simulates an emergency response system using code-based emergency input.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_SERVICES 4
#define MAX_EMERGENCIES 40
#define MAX_UNITS 10
#define MAX_QUEUE 100

pthread_mutex_t queue_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_cond = PTHREAD_COND_INITIALIZER;

const char *SERVICES[] = {"Police", "Ambulance", "Fire Truck", "Animal Control", "Hazmat"};
int available_units[5] = {MAX_UNITS, MAX_UNITS, MAX_UNITS, MAX_UNITS, MAX_UNITS};

typedef struct {
    char id[5];
    char type[50];
    int priority;
    char services[MAX_SERVICES][20];
    int service_count;
} Emergency;

typedef struct Job {
    Emergency emergency;
    struct Job *next;
} Job;

Job *job_queue = NULL;
Emergency emergencies[MAX_EMERGENCIES];
int emergency_count = 0;

void add_emergency(const char *id, const char *type, int priority, const char *services[], int service_count) {
    Emergency *e = &emergencies[emergency_count++];
    strcpy(e->id, id);
    strcpy(e->type, type);
    e->priority = priority;
    for (int i = 0; i < service_count; i++) {
        strcpy(e->services[i], services[i]);
    }
    e->service_count = service_count;
}

void enqueue_job(Emergency emergency) {
    Job *new_job = malloc(sizeof(Job));
    new_job->emergency = emergency;
    new_job->next = NULL;

    pthread_mutex_lock(&queue_lock);
    if (!job_queue || emergency.priority > job_queue->emergency.priority) {
        new_job->next = job_queue;
        job_queue = new_job;
    } else {
        Job *curr = job_queue;
        while (curr->next && curr->next->emergency.priority >= emergency.priority) {
            curr = curr->next;
        }
        new_job->next = curr->next;
        curr->next = new_job;
    }
    pthread_cond_signal(&queue_cond);
    pthread_mutex_unlock(&queue_lock);
}

void *service_unit(void *arg) {
    char *service_name = (char *)arg;
    int index = (strcmp(service_name, "Police") == 0) ? 0 :
                (strcmp(service_name, "Ambulance") == 0) ? 1 :
                (strcmp(service_name, "Fire Truck") == 0) ? 2 :
                (strcmp(service_name, "Animal Control") == 0) ? 3 : 4;

    while (1) {
        pthread_mutex_lock(&queue_lock);
        while (!job_queue) {
            pthread_cond_wait(&queue_cond, &queue_lock);
        }

        Job *prev = NULL;
        Job *curr = job_queue;
        Job *target_job = NULL;

        while (curr) {
            for (int i = 0; i < curr->emergency.service_count; i++) {
                if (strcmp(curr->emergency.services[i], service_name) == 0) {
                    target_job = curr;
                    break;
                }
            }
            if (target_job) break;
            prev = curr;
            curr = curr->next;
        }

        if (target_job) {
            if (!prev) job_queue = target_job->next;
            else prev->next = target_job->next;

            pthread_mutex_unlock(&queue_lock);

            printf("[%s Unit] Dispatched for emergency: %s\n", service_name, target_job->emergency.type);
            sleep(3);

            pthread_mutex_lock(&queue_lock);
            available_units[index]++;
            pthread_mutex_unlock(&queue_lock);

            printf("[%s Unit] Completed emergency: %s\n", service_name, target_job->emergency.type);
            free(target_job);
        } else {
            pthread_mutex_unlock(&queue_lock);
        }
        sleep(1);
    }
    return NULL;
}

Emergency *get_emergency_by_code(const char *code) {
    for (int i = 0; i < emergency_count; i++) {
        if (strcasecmp(emergencies[i].id, code) == 0) {
            return &emergencies[i];
        }
    }
    return NULL;
}

void init_emergencies() {
    const char *s1[] = {"Fire Truck"};
    add_emergency("E01", "Fire (Residential)", 3, s1, 1);
    const char *s2[] = {"Police"};
    add_emergency("E02", "Burglary", 2, s2, 1);
    const char *s3[] = {"Ambulance"};
    add_emergency("E03", "Heart Attack", 5, s3, 1);
    const char *s4[] = {"Ambulance", "Police"};
    add_emergency("E04", "Road Accident", 4, s4, 2);
    const char *s5[] = {"Fire Truck", "Police"};
    add_emergency("E05", "Gas Leak", 4, s5, 2);
    const char *s6[] = {"Police", "Animal Control"};
    add_emergency("E06", "Aggressive Dog", 2, s6, 2);
    const char *s7[] = {"Animal Control"};
    add_emergency("E07", "Stray Animal", 1, s7, 1);
    const char *s8[] = {"Ambulance", "Police"};
    add_emergency("E08", "Drug Overdose", 5, s8, 2);
    const char *s9[] = {"Fire Truck"};
    add_emergency("E09", "Forest Fire", 5, s9, 1);
    const char *s10[] = {"Police", "Fire Truck", "Ambulance"};
    add_emergency("E10", "Building Collapse", 5, s10, 3);
    const char *s11[] = {"Hazmat", "Fire Truck"};
    add_emergency("E11", "Chemical Spill", 5, s11, 2);
    const char *s12[] = {"Police", "Ambulance"};
    add_emergency("E12", "Shooting", 5, s12, 2);
    const char *s13[] = {"Animal Control", "Hazmat"};
    add_emergency("E13", "Animal Poisoning Incident", 3, s13, 2);
    const char *s14[] = {"Police"};
    add_emergency("E14", "Vandalism", 1, s14, 1);
    const char *s15[] = {"Ambulance"};
    add_emergency("E15", "Seizure", 4, s15, 1);
    const char *s16[] = {"Police", "Fire Truck", "Ambulance"};
    add_emergency("E16", "Flood", 5, s16, 3);
    const char *s17[] = {"Fire Truck", "Ambulance", "Animal Control"};
    add_emergency("E17", "Tornado", 5, s17, 3);
    const char *s18[] = {"Police", "Fire Truck", "Hazmat"};
    add_emergency("E18", "Bomb Threat", 5, s18, 3);
    const char *s19[] = {"Animal Control", "Police"};
    add_emergency("E19", "Wildlife Emergency", 3, s19, 2);
    const char *s20[] = {"Police", "Ambulance"};
    add_emergency("E20", "Hostage Situation", 5, s20, 2);
    const char *s21[] = {"Ambulance", "Hazmat"};
    add_emergency("E21", "Chemical Burn", 5, s21, 2);
    const char *s22[] = {"Police", "Fire Truck"};
    add_emergency("E22", "Power Outage", 3, s22, 2);  // New emergency
    const char *s23[] = {"Fire Truck", "Ambulance", "Police"};
    add_emergency("E23", "Earthquake", 5, s23, 3);  // New emergency
    const char *s24[] = {"Hazmat", "Fire Truck"};
    add_emergency("E24", "Radioactive Leak", 5, s24, 2);  // New emergency
    const char *s25[] = {"Fire Truck", "Police", "Ambulance"};
    add_emergency("E25", "Tsunami", 5, s25, 3);  // New emergency
    const char *s26[] = {"Police", "Ambulance", "Fire Truck"};
    add_emergency("E26", "Evacuation", 5, s26, 3);  // New emergency
}

int main() {
    init_emergencies();

    pthread_t threads[5];
    pthread_create(&threads[0], NULL, service_unit, (void *)"Police");
    pthread_create(&threads[1], NULL, service_unit, (void *)"Ambulance");
    pthread_create(&threads[2], NULL, service_unit, (void *)"Fire Truck");
    pthread_create(&threads[3], NULL, service_unit, (void *)"Animal Control");
    pthread_create(&threads[4], NULL, service_unit, (void *)"Hazmat");

    int choice;
    char input[100];

    printf("========== SMART EMERGENCY DISPATCH SYSTEM ==========\n");

    while (1) {
        printf("\nMain Menu:\n");
        printf("1. Report an Emergency (by code)\n");
        printf("2. List All Emergency Codes & Types\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        fgets(input, sizeof(input), stdin);
        choice = atoi(input);

        if (choice == 1) {
            printf("\nEnter emergency code (e.g., E01): ");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = '\0';

            Emergency *e = get_emergency_by_code(input);
            if (e) {
                enqueue_job(*e);
                printf("[OK] Emergency '%s' (%s) reported and queued for dispatch.\n", e->id, e->type);
            } else {
                printf("[X] Unknown emergency code. Use option 2 to view the list.\n");
            }

        } else if (choice == 2) {
            printf("\n[List] Emergency Code List:\n");
            for (int i = 0; i < emergency_count; i++) {
                printf("  %s - %s\n", emergencies[i].id, emergencies[i].type);
            }

        } else if (choice == 3) {
            printf("Exiting Emergency Dispatch System. Stay safe!\n");
            exit(0);
        } else {
            printf("Invalid choice. Please select a valid option.\n");
        }
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}