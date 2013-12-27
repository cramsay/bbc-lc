#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
 FILE *fp;
  int status;
  char path[1035];

  /* Open the command for reading. */
  fp = popen("/bin/ls /etc/", "r");
  if (fp == NULL) {
    printf("Failed to run command\n" );
    exit;
  }

  /* Read the output a line at a time - output it. */
  while (fgets(path, sizeof(path)-1, fp) != NULL) {
    printf("%s", path);
  }

  /* close */
  pclose(fp);

  return 0;
}
