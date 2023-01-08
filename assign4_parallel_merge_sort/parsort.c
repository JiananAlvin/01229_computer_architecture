/*
 * Implementation of parallel merge sort
 * CSF Assignment 4
 * Jianan Xu
 * jxu147@jhu.edu
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*
 * Merges two sorted array.
 *
 * Parameters:
 *   arr     - source array
 *   begin   - current start index (start from 0)
 *   mid     - end of the first merged array (size - 1)
 *   end     - end of the second merged array (size - 1)
 *   temparr - a temporary array to store sorting result
 */
void merge(int64_t *arr, size_t begin, size_t mid, size_t end, int64_t *temparr)
{
  size_t i = begin;
  size_t j = mid + 1;
  size_t current_index_of_temparr = 0;
  while (i <= mid && j <= end)
  {
    if (arr[i] < arr[j])
    {
      temparr[current_index_of_temparr++] = arr[i++];
    }
    else
    {
      temparr[current_index_of_temparr++] = arr[j++];
    }
  }
  // if the first array still left somthing
  while (i <= mid)
  {
    temparr[current_index_of_temparr++] = arr[i++];
  }
  // if the second array still left something
  while (j <= end)
  {
    temparr[current_index_of_temparr++] = arr[j++];
  }
}

/*
 * Compares two elements.
 *
 * Parameters:
 *   a    - left element to compare
 *   b    - right element to compare
 *
 * Returns:
 *   1    - left > right
 *  -1    - left < right
 *   0    - left = right
 */
int cmpfunc(const void *a, const void *b)
{
  // avoid overflow
  if (*(int64_t *)a > 0 && *(int64_t *)b < 0)
  {
    return 1;
  }
  // avoid overflow
  if (*(int64_t *)a < 0 && *(int64_t *)b > 0)
  {
    return -1;
  }

  if (*(int64_t *)a - *(int64_t *)b < 0)
  {
    return -1;
  }
  if (*(int64_t *)a - *(int64_t *)b > 0)
  {
    return 1;
  }

  return 0;
}

/*
 * Core function to do parallel merge sort.
 *
 * Parameters:
 *   arr         - source array
 *   begin       - start index (start from 0)
 *   end         - end index (size-1)
 *   threshold   - the number of elements below (inclusive) which the program should use a sequential sort
 */
void merge_sort(int64_t *arr, size_t begin, size_t end, size_t threshold)
{
  int count = end + 1 - begin;

  // if current size smaller than threshold, do it sequentially
  if (count <= threshold)
  {
    qsort(&arr[begin], count, sizeof(int64_t), cmpfunc);
  }
  else
  {
    pid_t pid_left = fork();
    pid_t pid_right = -1;
    if (pid_left == -1)
    {
      fprintf(stderr, "Error: create left child thread failed!\n");
      exit(1);
      // fork failed to start a new process
      // handle the error and exit
    }

    if (pid_left == 0)
    {
      // this is now in the left child process
      // call merge_sort recursively
      merge_sort(arr, begin, (begin + end) / 2, threshold);
      exit(0);
    }
    else
    {
      pid_right = fork();

      if (pid_right == -1)
      {
        fprintf(stderr, "Error: create right child thread failed!\n");
        exit(1);
        // fork failed to start a new process
        // handle the error and exit
      }
      if (pid_right == 0)
      {
        // this is now in the right child process
        // call merge_sort recursively
        merge_sort(arr, (begin + end) / 2 + 1, end, threshold);
        exit(0);
      }
    }
    // if pid is not 0, we are in the parent process
    // WARNING, if the child process path can get here, things will quickly break very badly

    int wstatus_left;
    int wstatus_right;
    // blocks until the process indentified by pid_to_wait_for completes
    pid_t actual_pid_left = waitpid(pid_left, &wstatus_left, 0);
    if (actual_pid_left == -1)
    {
      // handle waitpid failure
      fprintf(stderr, "Error: handle waitpid failure!\n");
      exit(1);
    }
    if (!WIFEXITED(wstatus_left))
    {
      // subprocess crashed, was interrupted, or did not exit normally
      // handle as error
      fprintf(stderr, "Error: subprocess crashed, was interrupted, or did not exit normally!\n");
      exit(1);
    }
    if (WEXITSTATUS(wstatus_left) != 0)
    {
      // subprocess returned a non-zero exit code
      // if following standard UNIX conventions, this is also an error
      fprintf(stderr, "Error: subprocess returned a non-zero exit code!\n");
      exit(1);
    }

    pid_t actual_pid_right = waitpid(pid_right, &wstatus_right, 0);
    if (actual_pid_right == -1)
    {
      // handle waitpid failure
      fprintf(stderr, "Error: handle waitpid failure!\n");
      exit(1);
    }
    if (!WIFEXITED(wstatus_right))
    {
      // subprocess crashed, was interrupted, or did not exit normally
      // handle as error
      fprintf(stderr, "Error: subprocess crashed, was interrupted, or did not exit normally!\n");
      exit(1);
    }
    if (WEXITSTATUS(wstatus_right) != 0)
    {
      // subprocess returned a non-zero exit code
      // if following standard UNIX conventions, this is also an error
      fprintf(stderr, "Error: subprocess returned a non-zero exit code!\n");
      exit(1);
    }

    int64_t *tmp = (int64_t *)malloc((end + 1) * sizeof(int64_t));
    merge(arr, begin, (begin + end) / 2, end, tmp);
    memcpy(&arr[begin], tmp, (end - begin + 1) * sizeof(int64_t));
    free(tmp);
  }
}

int main(int argc, char **argv)
{
  // check for correct number of command line arguments
  if (argc != 3)
  {
    fprintf(stderr, "Usage: %s <filename> <sequential threshold>\n", argv[0]);
    return 1;
  }

  // process command line arguments
  const char *filename = argv[1];
  char *end;
  size_t threshold = (size_t)strtoul(argv[2], &end, 10);

  if (end != argv[2] + strlen(argv[2]))
  /* report an error (threshold value is invalid) */
  {
    fprintf(stderr, "Error: threshold value is invalid\n");
    return 1;
  }

  // open the file
  int file_num = open(filename, O_RDWR);

  if (file_num < 0)
  {
    fprintf(stderr, "Error: Openning file failed!\n");
    return 1;
  }

  // use fstat to determine the size of the file
  struct stat buffer;
  int status = fstat(file_num, &buffer);
  if (status != 0)
  {
    fprintf(stderr, "Error: get file status failed!\n");
    return 1;
  }
  size_t file_size_in_bytes = buffer.st_size;

  // map the file into memory using mmap
  int64_t *data = mmap(0, file_size_in_bytes, PROT_READ | PROT_WRITE, MAP_SHARED, file_num, 0);
  if (data == MAP_FAILED)
  {
    fprintf(stderr, "Error: mmap failed!\n");
    return 2;
  }

  // sort the data
  merge_sort(data, 0, (buffer.st_size / sizeof(int64_t)) - 1, threshold);

  // unmap and close the file
  munmap(data, (buffer.st_size / sizeof(int64_t)) - 1);
  close(file_num);

  // exit with a 0 exit code if sort was successful
  return 0;
}
