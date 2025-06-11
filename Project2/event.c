#include "defs.h"
#include <stdlib.h>
#include <stdio.h>

/* Event functions */

/**
 * Initializes an `Event` structure.
 *
 * Sets up an `Event` with the provided system, resource, status, priority, and amount.
 *
 * @param[out] event     Pointer to the `Event` to initialize.
 * @param[in]  system    Pointer to the `System` that generated the event.
 * @param[in]  resource  Pointer to the `Resource` associated with the event.
 * @param[in]  status    Status code representing the event type.
 * @param[in]  priority  Priority level of the event.
 * @param[in]  amount    Amount related to the event (e.g., resource amount).
 */
void event_init(Event *event, System *system, Resource *resource, int status, int priority, int amount) {
    event->system = system;
    event->resource = resource;
    event->status = status;
    event->priority = priority;
    event->amount = amount;
}

/* EventQueue functions */

/**
 * Initializes the `EventQueue`.
 *
 * Sets up the queue for use, initializing any necessary data (e.g., semaphores when threading).
 *
 * @param[out] queue  Pointer to the `EventQueue` to initialize.
 */
void event_queue_init(EventQueue *queue) {
    if(queue == NULL){
        return;
    }
    // sets the head of the queue to null
    queue->head = NULL;
    // initializes the semaphore
    sem_init(&queue->eventQueue_mutex, 0, 1);
}

/**
 * Cleans up the `EventQueue`.
 *
 * Frees any memory and resources associated with the `EventQueue`.
 * 
 * @param[in,out] queue  Pointer to the `EventQueue` to clean.
 */
void event_queue_clean(EventQueue *queue) {
    // cleans the queue 
    if (queue != NULL){
        EventNode *current = queue->head;
        while (current != NULL) {
            EventNode *next = current->next; 
            free(current);                  
            current = next;                 
        }
        queue->head = NULL; 
        queue->size = 0;
        sem_destroy(&queue->eventQueue_mutex);    
    }
}

/**
 * Pushes an `Event` onto the `EventQueue`.
 *
 * Adds the event to the queue in a thread-safe manner, maintaining priority order (highest first).
 *
 * @param[in,out] queue  Pointer to the `EventQueue`.
 * @param[in]     event  Pointer to the `Event` to push onto the queue.
 */
void event_queue_push(EventQueue *queue, const Event *event) {
    // Unlocks the program
    sem_wait(&queue->eventQueue_mutex);
    
    // allocates memory for node
    EventNode *node = (EventNode *)malloc(sizeof(EventNode));
    // Checks if node is null
    if (node == NULL) {
        perror("Failed to allocate memory for new EventNode");
        sem_post(&queue->eventQueue_mutex); 
        return;
    }

    node->event = *event;
    node->next = NULL;

    
    if (queue->head == NULL || queue->head->event.priority < event->priority) {
        node->next = queue->head;
        queue->head = node;
    } 
    else {
        EventNode *curr = queue->head;
        while (curr->next != NULL && curr->next->event.priority >= event->priority) {
            curr = curr->next;
        }
        node->next = curr->next;
        curr->next = node;
    }

    // Increments size of the queue
    queue->size++;
    // Locks the program 
    sem_post(&queue->eventQueue_mutex);
}


/**
 * Pops an `Event` from the `EventQueue`.
 *
 * Removes the highest priority event from the queue in a thread-safe manner.
 *
 * @param[in,out] queue  Pointer to the `EventQueue`.
 * @param[out]    event  Pointer to the `Event` structure to store the popped event.
 * @return               Non-zero if an event was successfully popped; zero otherwise.
 */
int event_queue_pop(EventQueue *queue, Event *event) {
    // Unlocks the program
    sem_wait(&queue->eventQueue_mutex);
    if (queue->head == NULL) {
    	sem_post(&queue->eventQueue_mutex); 
        return 0; 
    }

    *event = queue->head->event;

    EventNode *remove = queue->head;
    queue->head = queue->head->next;

    // frees remove
    free(remove);

    // size of the queue decrements
    queue->size--;
    
    // Locks the program
    sem_post(&queue->eventQueue_mutex);
    return 1;
}
