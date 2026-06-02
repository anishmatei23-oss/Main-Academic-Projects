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
    if (emergency_count >= MAX_EMERGENCIES) {
        fprintf(stderr, "Maximum number of emergencies reached\n");
        return;
    }

    Emergency *e = &emergencies[emergency_count++];
    strncpy(e->id, id, sizeof(e->id) - 1);
    e->id[sizeof(e->id) - 1] = '\0';
    strncpy(e->type, type, sizeof(e->type) - 1);
    e->type[sizeof(e->type) - 1] = '\0';
    e->priority = priority;
    e->service_count = service_count > MAX_SERVICES ? MAX_SERVICES : service_count;

    for (int i = 0; i < e->service_count; i++) {
        strncpy(e->services[i], services[i], sizeof(e->services[i]) - 1);
        e->services[i][sizeof(e->services[i]) - 1] = '\0';
    }
}

void enqueue_job(Emergency emergency) {
    Job *new_job = malloc(sizeof(Job));
    if (!new_job) {
        perror("Failed to allocate memory for new job");
        return;
    }

    new_job->emergency = emergency;
    new_job->next = NULL;

    pthread_mutex_lock(&queue_lock);

    // Check if queue is empty or new job has higher priority than head
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
    int index = -1;

    // Find service index
    for (int i = 0; i < 5; i++) {
        if (strcmp(SERVICES[i], service_name) == 0) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        fprintf(stderr, "Unknown service: %s\n", service_name);
        return NULL;
    }

    while (1) {
        pthread_mutex_lock(&queue_lock);

        // Wait until there's a job that needs this service
        while (1) {
            Job *prev = NULL;
            Job *curr = job_queue;
            Job *target_job = NULL;

            // Find first job that requires this service
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
                // Remove job from queue
                if (!prev) {
                    job_queue = target_job->next;
                } else {
                    prev->next = target_job->next;
                }

                available_units[index]--;
                pthread_mutex_unlock(&queue_lock);

                printf("[%s Unit] Dispatched for emergency: %s (Priority: %d). ",
                       service_name, target_job->emergency.type, target_job->emergency.priority);

                // Print all dispatched services and remaining units
                printf("Dispatched Services: ");
                for (int i = 0; i < target_job->emergency.service_count; i++) {
                    printf("%s", target_job->emergency.services[i]);
                    int service_index = -1;
                    for (int j = 0; j < 5; j++) {
                        if (strcmp(SERVICES[j], target_job->emergency.services[i]) == 0) {
                            service_index = j;
                            break;
                        }
                    }
                    if (service_index != -1) {
                        printf(" (%d/%d left)", available_units[service_index] - (strcmp(SERVICES[index], target_job->emergency.services[i]) == 0 ? 1 : 0), MAX_UNITS);
                    }
                    if (i < target_job->emergency.service_count - 1) {
                        printf(", ");
                    }
                }
                printf("\n");

                sleep(3); // Simulate work

                pthread_mutex_lock(&queue_lock);
                available_units[index]++;
                pthread_mutex_unlock(&queue_lock);

                printf("[%s Unit] Completed emergency: %s\n", service_name, target_job->emergency.type);
                free(target_job);
                break;
            } else {
                // No jobs for this service - wait for signal
                pthread_cond_wait(&queue_cond, &queue_lock);
            }
        }
        pthread_mutex_unlock(&queue_lock);
        sleep(1); // Brief pause between jobs
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
    add_emergency("E22", "Power Outage", 3, s22, 2);
    const char *s23[] = {"Fire Truck", "Ambulance", "Police"};
    add_emergency("E23", "Earthquake", 5, s23, 3);
    const char *s24[] = {"Hazmat", "Fire Truck"};
    add_emergency("E24", "Radioactive Leak", 5, s24, 2);
    const char *s25[] = {"Fire Truck", "Police", "Ambulance"};
    add_emergency("E25", "Tsunami", 5, s25, 3);
    const char *s26[] = {"Police", "Ambulance", "Fire Truck"};
    add_emergency("E26", "Evacuation", 5, s26, 3);
}

int main() {
    init_emergencies();

    pthread_t threads[5];
    for (int i = 0; i < 5; i++) {
        if (pthread_create(&threads[i], NULL, service_unit, (void *)SERVICES[i]) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }

    int choice;
    char input[100];

    printf("========== SMART EMERGENCY DISPATCH SYSTEM ==========\n");

    while (1) {
        printf("\nMain Menu:\n");
        printf("1. Report an Emergency (by code)\n");
        printf("2. List All Emergency Codes & Types\n");
        printf("3. View Available Units\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");

        if (fgets(input, sizeof(input), stdin) == NULL) {
            perror("Error reading input");
            continue;
        }

        choice = atoi(input);

        switch (choice) {
            case 1: {
                printf("\nEnter emergency code (e.g., E01): ");
                if (fgets(input, sizeof(input), stdin) == NULL) {
                    perror("Error reading input");
                    continue;
                }
                input[strcspn(input, "\n")] = '\0';

                Emergency *e = get_emergency_by_code(input);
                if (e) {
                    enqueue_job(*e);
                    printf("[OK] Emergency '%s' (%s) reported and queued for dispatch.\n", e->id, e->type);
                } else {
                    printf("[X] Unknown emergency code. Use option 2 to view the list.\n");
                }
                break;
            }
            case 2: {
                printf("\n[List] Emergency Code List:\n");
                for (int i = 0; i < emergency_count; i++) {
                    printf("  %s - %s (Priority: %d)\n", emergencies[i].id, emergencies[i].type, emergencies[i].priority);
                }
                break;
            }
            case 3: {
                printf("\n[Status] Available Units:\n");
                pthread_mutex_lock(&queue_lock);
                for (int i = 0; i < 5; i++) {
                    printf("  %s: %d/%d available\n", SERVICES[i], available_units[i], MAX_UNITS);
                }
                pthread_mutex_unlock(&queue_lock);
                break;
            }
            case 4: {
                printf("Exiting Emergency Dispatch System. Stay safe!\n");
                exit(0);
            }
            default: {
                printf("Invalid choice. Please select a valid option.\n");
                break;
            }
        }
    }

    // Cleanup (though we never reach here due to infinite loop)
    for (int i = 0; i < 5; i++) {
        pthread_cancel(threads[i]);
        pthread_join(threads[i], NULL);
    }

    return 0;
}