#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <cstring>
#include <cstdlib>
#include <cstdio>
static inline char **new_argv = nullptr;
static inline void cleanup_for_new_args() {
  if (not new_argv)
    return;
  for (auto i = 0; (new_argv + i) && *(new_argv + i); i++)
    free(*(new_argv + i));
  free(new_argv);
}
static bool is_command_present(const int *const argc,
                               char ***argv,
                               const char *const short_cmd,
                               const char *const long_cmd) {
  for (auto i = 0; i < *argc; i++)
    if (strstr(*(*argv + i), long_cmd) || strstr(*(*argv + i), short_cmd))
      return true;
  return false;
}
static void add_command_if_not_present(int *const argc,
                                       char ***argv,
                                       const char *const short_cmd,
                                       const char *const long_cmd,
                                       const char *const default_arg) {
  if (is_command_present(argc, argv, short_cmd, long_cmd))
    return;

  new_argv = (char **)malloc(sizeof(char *) * (*argc + 2));
  for (auto i = 0; i < *argc; i++) {
    (*(new_argv + i)) = (char *)malloc(strlen(*(*argv + i)) + 1);
    strcpy(*(new_argv + i), *(*argv + i));
  }
  (*(new_argv + *argc)) = (char *)malloc(strlen(default_arg) + 1);
  strcpy((*(new_argv + *argc)), default_arg);
  (*(new_argv + *argc + 1)) = nullptr;
  ++(*argc);
  *argv = new_argv;
  atexit(cleanup_for_new_args);
}
#ifdef __cplusplus
}
#endif
