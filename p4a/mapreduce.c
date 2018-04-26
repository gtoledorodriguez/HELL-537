#include "mapreduce.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Different function pointer types used by MR
//typedef char *(*Getter)(char *key, int partition_number);
//typedef void (*Mapper)(char *file_name);
//typedef void (*Reducer)(char *key, Getter get_func, int partition_number);
//typedef unsigned long (*Partitioner)(char *key, int num_partitions);


// Structs
typedef struct argument_struct {
	char **argv;
	int argc;
}argument;

typedef struct value_node {
	char *value;
	struct value_node *next;

}Vnode;

typedef struct value_list {
	Vnode *head;
	int counter;
}Vlist;

typedef struct hashtable_for_keys {
	char *key;
	Vlist *values;
	pthread_mutex_t keylock;
	struct hashtable_for_keys *head;
	struct hashtable_for_keys *next;
}hasht;	

typedef struct Key_node {
	hasht *keys;
	int curr_nextV;
	int size;
	int real_key;	
}Knode;
	
// Global Variables
pthread_mutex_t lock;
pthread_mutex_t *lock_concurrent;
Mapper map_real;
Reducer reduce_real;
Partitioner partition_real;
int num_partitioners;
Knode *allKeys;
unsigned long lfsr = 1;
int filecount;

//Wrapper Functions
void
Pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg)
{
        int rc = pthread_create(thread, attr, start_routine, arg);
        assert(rc == 0);
}

void
Pthread_join(pthread_t thread, void **value_ptr)
{
        int rc = pthread_join(thread, value_ptr);
        assert(rc == 0);
}

void
Pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr)
{
        int rc = pthread_mutex_init(mutex, attr);
        assert(rc == 0);
}

void
Pthread_mutex_lock(pthread_mutex_t *mutex)
{
	int rc = pthread_mutex_lock(mutex);
	assert(rc == 0);
}

void
Pthread_mutex_unlock(pthread_mutex_t *mutex)
{
	int rc = pthread_mutex_unlock(mutex);
	assert(rc == 0);
}


char *
get_next(char *key, int partition_number) {
	Knode *curr_keys = &allKeys[partition_number];
	int curr = curr_keys->curr_nextV;
	char *curr_value;
	if(curr_keys->keys[curr].values->counter > 0){
		curr_value = curr_keys->keys[curr].values->head->value;
		curr_keys->keys[curr].values->head = curr_keys->keys[curr].values->head->next;
		curr_keys->keys[curr].values->counter--;
		return curr_value;
	}
	curr_keys->curr_nextV++;
	return NULL;
}

int
Knode_cmp(const void *p1, const void *p2)
{
	hasht *key1 = (hasht *)p1;
	hasht *key2 = (hasht *)p2;
	if (!key1->key ){
		return 1;
	}
	if (!key2->key ){
		return -1;
	}
	return strcmp(key1->key, key2->key);
}

// Worker functions
void *
worker_map(void *ARG)
{
	argument *args = (argument *)ARG;
	char *file;
	while (1) {
		Pthread_mutex_lock(&lock);
		filecount++;
		if(filecount >= args->argc){
			Pthread_mutex_unlock(&lock);
			break;
		}
		file = args->argv[filecount];
		Pthread_mutex_unlock(&lock);
		map_real(file);
	}
	return NULL;
}


void *
worker_sort(void *index)
{
	unsigned long i;
	Knode *curr_keys;
	i = (unsigned long)index;
	curr_keys = &allKeys[i];
	if(curr_keys->real_key == 0){
		return NULL;
	}
	hasht *clean_keys = (hasht *)malloc(curr_keys->real_key *sizeof(hasht ));
	int counter = 0;
	for (int j = 0; j < curr_keys->size; j++){
		hasht *currKey = curr_keys->keys[j].head;
		if(currKey == NULL){
			continue;
		}
		while(currKey != NULL){
			clean_keys[counter] = *currKey;
			hasht *temp = currKey->next;
			free(currKey);
			counter++;
			currKey = temp;
		}
	}
	//here
	free(curr_keys->keys);
	curr_keys->keys = clean_keys;
	qsort((void *)curr_keys->keys, curr_keys->real_key, sizeof(hasht), Knode_cmp);

	return NULL;
}


void *
worker_reduce(void *index)
{
	unsigned long count;
	Knode *curr_keys;
	count = (unsigned long) index;
	curr_keys = &allKeys[count];
	if(curr_keys->real_key == 0){
		return NULL;
	}
	while(curr_keys->curr_nextV <curr_keys->real_key){
		reduce_real(curr_keys->keys[curr_keys->curr_nextV].key, get_next, (int)count);
	}
	for (int j = 0; j < curr_keys->real_key; j++){
		Vnode *currV = curr_keys->keys[j].values->head;
		while(currV != NULL){
			Vnode *temp = currV->next;
			free(currV);
			currV = temp;
		}
		free(curr_keys->keys[j].values);
	}
	return NULL;
}



// External functions: these are what you must define
void
MR_Emit(char *key, char *value)
{
	unsigned long  bucket;
	unsigned long key_bucket;
	Knode *curr_keys;
	bucket = partition_real(key, num_partitioners);
	curr_keys = &allKeys[bucket];
	key_bucket = MR_DefaultHashPartition(key, curr_keys->size);
	Pthread_mutex_lock(&curr_keys->keys[key_bucket].keylock);
	hasht *curr = curr_keys->keys[key_bucket].head;
	while(1){	
		if(curr == NULL){
			hasht *newKey = malloc(sizeof(hasht));
			newKey->key = strdup(key);
			Pthread_mutex_lock(&lock_concurrent[bucket]);
			curr_keys->real_key += 1;
			Pthread_mutex_unlock(&lock_concurrent[bucket]);
			Vlist *valuelist = (Vlist *)malloc(sizeof(Vlist));
			Vnode *values = (Vnode *)malloc(sizeof(Vnode));
			valuelist->head = values;
			values->value = strdup(value);
			valuelist->counter = 1;
			values->next = NULL;
			newKey->values = valuelist;
			newKey->next = curr_keys->keys[key_bucket].head;
			newKey->values = valuelist;
			curr_keys->keys[key_bucket].head = newKey;
			break;
		}else if (strcmp(curr->key, key) == 0){
			curr->values->counter++;
			Vnode *values_next = (Vnode *)malloc(sizeof(Vnode));
			values_next->value = strdup(value);
			values_next->next = curr->values->head;
			 curr->values->head = values_next;
			break;
		} else {
			curr = curr->next;			
		}
	}
	Pthread_mutex_unlock(&curr_keys->keys[key_bucket].keylock);
	
}

unsigned long
MR_DefaultHashPartition(char *key, int num_buckets)
{
	unsigned long hash = 5381;
	int c;
	while ((c = *key++) != '\0')
		hash = hash * 33 + c;
	return hash % num_buckets;
}

void
MR_Run(int argc, char *argv[], 
	    Mapper map, int num_mappers, 
	    Reducer reduce, int num_reducers, 
	    Partitioner partition)
{
	argument args;
	filecount = 0;
	map_real = map;
	reduce_real = reduce;
	args.argc = argc;
	args.argv = argv;

	// Threads for map and reduce functions
	pthread_t p_map[num_mappers];
	pthread_t p_reduce[num_reducers];
	lock_concurrent = (pthread_mutex_t *)malloc(num_reducers * sizeof(pthread_mutex_t));
	num_partitioners = num_reducers;

	// Initialize the lock
	Pthread_mutex_init(&lock, NULL);
	for (int i = 0; i < num_reducers; i++) {
		Pthread_mutex_init(&lock_concurrent[i], NULL);
	}
	if(partition == NULL){
		partition_real = MR_DefaultHashPartition;
	} else {
		partition_real = partition;
	}

	// Concurrent Work
	allKeys = (Knode *)malloc(num_partitioners * sizeof(Knode));
	for(int i = 0; i < num_partitioners; i++){
		allKeys[i].size = 7919;
		allKeys[i].keys = (hasht *)malloc(allKeys[i].size * sizeof(hasht));
		for(int j = 0; j < allKeys[i].size; j++){
			Pthread_mutex_init(&allKeys[i].keys[j].keylock,NULL);
			allKeys[i].keys[j].head = NULL;
		}
		allKeys[i].curr_nextV = 0;
		allKeys[i].real_key = 0;
	}
	// Mapper threads
	for (int i = 0; i < num_mappers; i++){
		Pthread_create(&p_map[i], NULL, worker_map, (void *)&args);
	}
	for (int i = 0; i < num_mappers; i++){
		Pthread_join(p_map[i], NULL);
	}

	// Sort threads
        for (unsigned long i = 0; i < num_reducers; i++){
                Pthread_create(&p_reduce[i], NULL, worker_sort, (void *)i);
        }
        for (int i = 0; i < num_reducers; i++){
                 Pthread_join(p_reduce[i], NULL);
        }

	// Reducers threads
	for (unsigned long i = 0; i < num_reducers; i++){
		Pthread_create(&p_reduce[i], NULL, worker_reduce, (void *)i);
	}
	for (int i = 0; i < num_reducers; i++){
		Pthread_join(p_reduce[i], NULL);
	}

	// Destroy locks
	pthread_mutex_destroy(&lock);
	for (int i = 0; i < num_reducers; i++) {
		pthread_mutex_destroy(&lock_concurrent[i]);
	}
	for (int i = 0; i < num_reducers; i++) {
			free(allKeys[i].keys);
	}
		
	free(allKeys);
	free(lock_concurrent);
} 
