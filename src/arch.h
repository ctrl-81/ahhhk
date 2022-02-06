#ifndef ARCH_H
#define ARCH_H

typedef enum {
  NO_TARGET,
  NT_AMD64,
  LINUX_AMD64
} target_t;

#ifdef _WIN64
#define DEFAULT_TARGET NT_AMD64
#else
#define DEFAULT_TARGET LINUX_AMD64
#endif

#endif // ARCH_H
