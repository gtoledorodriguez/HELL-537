#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

int threads;

typedef struct zipContent {
  char *content;
  int length;
  int index;
} zip_t;


typedef struct zip_output {
    char *content;
    int size;
    int last_letterNum;
    char last_letter;
} output_t;


typedef struct queue {
  pthread_mutex_t lock;
  int size, head, cap;
  void **arr;
  pthread_cond_t en;
  pthread_cond_t de;
} queue_t;

typedef struct thread_node {
  queue_t *q;
  output_t *outputs;
} node_t;



void queue_init(queue_t *q, int cap) {
  q->cap = cap;
  q->head = 0;
  q->size = 0;
  q->arr = malloc(cap * sizeof(void *));
  pthread_mutex_init(&q->lock, NULL);
  pthread_cond_init(&q->en, NULL);
  pthread_cond_init(&q->de, NULL);
}

void enqueue(queue_t *q, void *zc) {
  pthread_mutex_lock(&(q->lock));
  while(q->size == q->cap) {
    pthread_cond_wait(&q->de, &q->lock);
  }
  q->arr[(q->head + q->size) % q->cap] = zc;
  q->size++;
  pthread_cond_signal(&q->en);
  pthread_mutex_unlock(&q->lock);
}

void *dequeue(queue_t *q) {
    void *temp;
    pthread_mutex_lock(&q->lock);
    while (q->size == 0) {
        pthread_cond_wait(&q->en, &q->lock);
    }
    temp = q->arr[q->head];
    q->head = (q->head + 1) % q->cap;
    q->size--;
    pthread_cond_signal(&q->de);
    pthread_mutex_unlock(&q->lock);
    return temp;
}

void zip (zip_t *input, output_t *output) {
   int m = 0;
   int i = 0;
   int letterNum = 0;
   char letter = '\0';
   char last_letter = letter;
   int last_letterNum = letterNum;
   int content_size = input->length ;
   char *content = malloc(content_size);
   while(m < input->length && input != NULL) {
             if(letter == '\0'){
		  letter = input->content[m];
                  letterNum++;
                  m++;
             }
	    else if(input->content[m] == letter){
                 letterNum++;
                 m++;
             }
            else{
                 if( i >= content_size-1){
                    content_size*=2;
                    content = realloc(content, content_size);
                    if(content ==NULL){
                      printf("%s", "realloc error");
                      exit(1);
                   }
                 }
	         *((int *) &content[i]) = letterNum;
                 content[i+sizeof(int)] = letter;
                 last_letterNum = letterNum;
                 letterNum = 0;
                 last_letter = letter;
                 letter = '\0';
                 i+=(sizeof(int) + 1); 
            }         
  } 
  if(letterNum > 1){
                 *((int *) (content + i) )= letterNum;
                 *((char *) (content + i + sizeof(int)) )= letter;
                 last_letterNum = letterNum;
                 letterNum = 0;
                 last_letter = letter;
                 letter = '\0';
                 i+=(sizeof(int) + sizeof(char)); 
   } 
     content = realloc(content, i);
     *output = (output_t) {content, i, last_letterNum, last_letter};
}


void *mythread (void *arg) { 
  node_t *n = (node_t *)arg;
  queue_t *q = n->q;
  output_t *outputs = n->outputs;
  zip_t *dq;
  while((dq = dequeue(q)) != NULL) {
    zip(dq, &outputs[dq->index]);
    munmap(dq->content, dq->length);
    if(dq != NULL)
      free(dq);
  }
  enqueue(q, NULL);
  return NULL;
}

int main (int argc, char *argv[]){
  // argv[0] -> pzip
  if(argc >= 2){
    int i = 1;
    int chunkSize = getpagesize();
    int chunkCounter = 0;
    int numberofchunks = 0;
    struct stat buf;
    output_t *output;
    zip_t *reading;
    queue_t q;
    for(int j = 0; j<argc; j++){
      if (stat(argv[i],&buf) < 0) {
        printf("Error: Unable to determine file size\n");
        exit(1);
     }
     numberofchunks += (buf.st_size + chunkSize - 1 ) / chunkSize;
    }
    output_t *outputs = malloc(numberofchunks * sizeof(output_t));
    node_t n = {&q, outputs};
    threads = get_nprocs();
    queue_init(&q, 2*threads);
    pthread_t *work_thread = calloc(threads, sizeof(pthread_t));
    for (int j = 0; j < threads; j++) {
        pthread_create(&work_thread[j], NULL, &mythread, &n);
    }
   
    while(i < argc){
     int fd = open(argv[i], O_RDONLY);
     if (fd == -1) {
       printf("pzip: cannot open file\n");
       exit(1);
     }
     if (fstat(fd,&buf) == -1) {
        printf("Error: Unable to determine file size\n");
        exit(1);
     }  
     for (int j = 0; j < buf.st_size; j+= chunkSize) {
       reading = malloc(sizeof(*reading));
       if(chunkSize > buf.st_size - j){
         reading-> length = buf.st_size -j;
       }
       else {
         reading -> length = chunkSize;
       }
       if(chunkSize == 0){
         break;
       }
       reading->index = chunkCounter++;
       reading->content = (char *)mmap(NULL, chunkSize, PROT_READ, MAP_PRIVATE, fd, j);   
       if(reading->content == MAP_FAILED){
         printf("map failed\n");
         exit(1);
       }
       enqueue(&q, reading);
      }
     close(fd);
     i++;
    }
    enqueue(&q, NULL);
    for(int j = 0; j < threads; j++) {
      pthread_join(work_thread[j], NULL);
    }
    int last_letter = '\0';
    int last_letterNum = 0;
    for(int j = 0; j < numberofchunks; j++) {
      output = &outputs[j];
      if(output != NULL){
        if(output->size == 0){;
          continue;
        }
        else{
          if(output->content[4] == last_letter){
             *((int *) (output->content))+=last_letterNum;
          }
          else if(last_letterNum != 0){
           fwrite(&last_letterNum, sizeof(int), 1, stdout);
           fwrite(&last_letter, sizeof(char), 1, stdout);
           last_letterNum = 0;
           last_letter = '\0';
          }
          if(output->size > 5){
            fwrite(output->content, (size_t) (output->size - 5), 1, stdout);
          }
          else{
             output->last_letterNum =  *((int *) (output->content));
          }
          last_letter = output->last_letter;
          last_letterNum = output->last_letterNum;
        }
      }
        if(output->content != NULL)
          free(output->content);
    }
    if(last_letterNum != 0){
           fwrite(&last_letterNum, sizeof(int), 1, stdout);
           fwrite(&last_letter, sizeof(char), 1, stdout);
           last_letterNum = 0;
           last_letter = '\0';
    }
}
  else if(argc < 2){
     printf("pzip: file1 [file2 ...]\n");
       exit(1);
  }
  else{
    exit(0);
  }
  return 0;
}

