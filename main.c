/****************************************************************************

お魚のシェル
チュートリアルに忠実

*****************************************************************************/

#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
  Function Declarations for builtin shell commands:
 組み込みシェルコマンドの関数宣言：
 */
int fish_cd(char **args);
int fish_help(char **args);
int fish_exit(char **args);
int fish_ebi(char **args);
int fish_sakana(char **args);


/*
  List of builtin commands, followed by their corresponding functions.
 組み込みコマンドのリスト。その後に対応する関数が続きます。
 */
char *builtin_str[] = {
    "cd",
    "help",
    "exit",
    "ebi",
    "sakana"
};

int (*builtin_func[]) (char **) = {
    &fish_cd,
    &fish_help,
    &fish_exit,
    &fish_ebi,
    &fish_sakana
};

int fish_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

/*
  Builtin function implementations.
 組み込み関数の実装。
*/

/**
   @brief Builtin command: change directory.
   @param args List of args.  args[0] is "cd".  args[1] is the directory.
   @return Always returns 1, to continue executing.
 */
int fish_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, ">><(((°> < fish: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror(">><(((°> < fish");
    }
  }
  return 1;
}

/**
   @brief Builtin command: print help.
   @param args List of args.  Not examined.
   @return Always returns 1, to continue executing.
 */
int fish_help(char **args)
{
  int i;
  printf("          |isokichi's fish\n");
  printf("          |Type program names and arguments, and hit enter.\n");
  printf("          |The following are built in:\n");

  for (i = 0; i < fish_num_builtins(); i++) {
    printf("          |  %s\n", builtin_str[i]);
  }

  printf(">><(((°> < Use the man command for information on other programs.\n");
  return 1;
}

/**
   @brief Builtin command: exit.
   @param args List of args.  Not examined.
   @return Always returns 0, to terminate execution.
 */
int fish_exit(char **args)
{
  return 0;
}

/**
エビの実装
 */
int fish_ebi(char **args)
{
    printf("(´￣￣￣￣￣￣￣￣￣\n");
    printf("  ＼\n");
    printf("　　＼_,\n");
    printf("　　 ヾ9＼\n");
    printf("　 __ｸ 　 ｀>､＿_\n");
    printf("　j广>rイ　 /　/ ｀>､\n");
    printf("　　 ｀ `　Ｙ┬く-く_,ハ、\n");
    printf("　　 　 　 ｀　`　′'`'ー＼\n");
    printf("　　　　 　 　 　 　 　 ヾ＼\n");
    printf("　　　　　　　　　　　 　｀^'\n");
    return 1;
}

/**
 魚の実装
 */
int fish_sakana(char **args)
{
    printf("　　　　　 　/,　.\n");
    printf("　　　 _,..//〃ー，_／(.　　　　　／\n");
    printf("　,イ';＾;;;;::\"\"'''　::\"〃,,__∠_/\n");
    printf("／;:◎'':;　）;＿＿___　　　　　　 (\n");
    printf("≧_ﾉ　　__ノ))三＝　　_..､'､\"^^^＼ヾ\n");
    printf("　~''ー＜　＿＿_､-~＼(\n");
    printf("　　　　　＼(\n");
    return 1;
}

/**
  @brief Launch a program and wait for it to terminate.
  @param args Null terminated list of arguments (including program).
  @return Always returns 1, to continue execution.
 */
int fish_launch(char **args)
{
  pid_t pid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror(">><(((°> < fish");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror(">><(((°> < fish");
  } else {
    // Parent process
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

/**
   @brief Execute shell built-in or launch program.
   @param args Null terminated list of arguments.
   @return 1 if the shell should continue running, 0 if it should terminate
 */
int fish_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < fish_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return fish_launch(args);
}

#define fish_RL_BUFSIZE 1024
/**
   @brief Read a line of input from stdin.
   @return The line from stdin.
 */
char *fish_read_line(void)
{
  int bufsize = fish_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, ">><(((°> < fish: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    // Read a character
    c = getchar();

    // If we hit EOF, replace it with a null character and return.
    if (c == EOF || c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
      bufsize += fish_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, ">><(((°> < fish: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

#define fish_TOK_BUFSIZE 64
#define fish_TOK_DELIM " \t\r\n\a"
/**
   @brief Split a line into tokens (very naively).
   @param line The line.
   @return Null-terminated array of tokens.
 */
char **fish_split_line(char *line)
{
  int bufsize = fish_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token, **tokens_backup;

  if (!tokens) {
    fprintf(stderr, ">><(((°> < fish: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, fish_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += fish_TOK_BUFSIZE;
      tokens_backup = tokens;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
		free(tokens_backup);
        fprintf(stderr, ">><(((°> < fish: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, fish_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

/**
   @brief Loop getting input and executing it.
 */
void fish_loop(void)
{
  char *line;
  char **args;
  int status;

  do {
    printf(">><(((°> ");
    line = fish_read_line();
    args = fish_split_line(line);
    status = fish_execute(args);

    free(line);
    free(args);
  } while (status);
}

/**
   @brief Main entry point.
   @param argc Argument count.
   @param argv Argument vector.
   @return status code
 */
int main(int argc, char **argv)
{
  // Load config files, if any.

  // Run command loop.
  fish_loop();

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}

