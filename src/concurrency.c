#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <gtk/gtk.h>
#include "mills.h"
#include "gui.h"
#include "concurrency.h"

#define PIPE_1_TO_2 "1to2"
#define PIPE_2_TO_1 "2to1"

struct pipes {
    FILE *fifo_in, *fifo_out;
};

static FILE *open_out_pipe(char *name);
static FILE *open_in_pipe(char *name);

void close_pipes(PipesPtr pipes) 
{ 
  fclose(pipes->fifo_in); 
  fclose(pipes->fifo_out); 
  free(pipes); 
}

PipesPtr init_pipes(Player player_no)
{
  struct stat st;
  if (stat(PIPE_1_TO_2, &st) != 0) {
    mkfifo(PIPE_1_TO_2, 0664);
  }
  if (stat(PIPE_2_TO_1, &st) != 0) {
    mkfifo(PIPE_2_TO_1, 0664);
  }
  
  PipesPtr pipes = (PipesPtr) malloc(sizeof(struct pipes));
  
  if (pipes == NULL) {
    fprintf(stderr, "Memory allocation error\n");
  } else {
      pipes->fifo_out = open_out_pipe(player_no == PlayerWhite ? PIPE_1_TO_2 : PIPE_2_TO_1);
      pipes->fifo_in = open_in_pipe(player_no == PlayerWhite ? PIPE_2_TO_1 : PIPE_1_TO_2);
  }
  
  return pipes;
}

static FILE *open_out_pipe(char *name) {
    FILE *pipe = fopen(name, "w+");
    if (pipe == NULL)
    {
        signal_error("Error in creating output pipe");
        exit(-1);
    }
    return pipe;
}

static FILE *open_in_pipe(char *name){
    FILE *pipe = fopen(name, "r+");
    if (pipe == NULL)
    {
        signal_error("Error in creating input pipe");
        exit(-1);
    }
    int flags, fd;
    fd = fileno(pipe);
    flags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    return pipe;
}

void send_string_to_pipe(PipesPtr pipes, const char *data)
{
    int result = fprintf(pipes->fifo_out,"%s\n",data);
    
    fflush(pipes->fifo_out);
    if (result == 0)
        signal_error("Failed to send data");
}

bool get_string_from_pipe(PipesPtr pipes, char *buffer, size_t size)
{
    char *result = fgets(buffer,size,pipes->fifo_in);
    fflush(pipes->fifo_in);
    //if (result == NULL) pokazBlad("Failed to read data");
    return result != NULL;
}
