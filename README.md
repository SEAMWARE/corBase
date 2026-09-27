# corBase - Core Utilities Library

The foundation every other library of the stack stands on: string, file and time
helpers, the error stack, and the **library log** - the callback through which a
library reaches the log of the executable it runs in.

## Where it comes from

corBase is **kbase** under the cor prefix, copied, not forked: kbase itself is
untouched and keeps serving its own users.

The renames: every `k*` function → `cor*` (`kFileRead` → `corFileRead`,
`kStringSplit` → `corStringSplit`, `kTimeGet` → `corTimeGet`, ...), the files with
them; `KErrorStack` → `CorErrorStack` (and `kError*` → `corError*`); the macros
`K_VEC_SIZE`/`K_MAX`/`K_MIN`/`K_FT`/`K_SET` → `COR_*`; `KBool`/`KTRUE`/`KFALSE` →
`bool`/`true`/`false`, so `kTypes.h` is gone. The tools (`atoh`, `atoi`, `htoa`,
`htoi`, `itoh`) keep their names. Left behind: `kStringInList.h`, declared but never
implemented, the `scripts/` of the k-lib era, and a test that tested its own copy
of `kStringSplit`.

The one redesign is the library log. kbase had two mechanisms, neither working: the
`KLOG_*` macros called a `kjLogFunction` that did not exist (they compiled only with
`K_LOG_ON`), and `kInit()`, which set the `kLogFunction` pointer, was never called -
so no library line ever reached a log. `kBasicLog`'s `KBL_*` were bare `printf`s,
which never reach the executable's log file either. corBase has ONE mechanism: the
`COR_LIB_*` macros and `corBaseInit()` (see [Library log](#library-log-corliblogh)).

## Features

- **String utilities** - Split, sort, replace, join, search
- **File operations** - Read files, extract suffixes, get program name
- **Time functions** - Get time, calculate diffs, accumulate durations
- **Error stack** - Collect and report errors with file/line/function context
- **Library log** - `COR_LIB_*` macros that hand each line to the executable's log

## API Reference

### Macros (corMacros.h)

```c
COR_VEC_SIZE(a) // Get array size in items
COR_MAX(a, b)   // Maximum of two values
COR_MIN(a, b)   // Minimum of two values
COR_FT(b)       // bool to "true"/"false" string
COR_SET(b)      // bool to "set"/"unset" string
```

### String Functions

#### corStringSplit

```c
int corStringSplit(char* s, char delimiter, char** sVec, int sVecLen);
```

Splits string into vector of substrings by delimiter. **In-place operation** - replaces delimiters with null terminators. Returns number of items.

```c
char str[] = "a,b,c";
char* parts[10];
int count = corStringSplit(str, ',', parts, 10);
// count=3, parts[0]="a", parts[1]="b", parts[2]="c"
```

#### corStringSort

```c
void corStringSort(char* s);
```

Sorts characters within a string alphabetically (in-place).

#### corStringReplace

```c
char* corStringReplace(const char* in, const char* replace, const char* with,
                       char* out, int outLen);
```

Replaces all occurrences of substring. Returns pointer to output buffer.

#### corStringArrayJoin

```c
void corStringArrayJoin(char* output, int outputLen, char** stringV,
                        int vecItems, const char* separator);
```

Joins array of strings with separator.

#### corStringArrayLookup

```c
int corStringArrayLookup(char** stringV, int vecItems, const char* needle);
```

Linear search for string in array. Returns index or -1 if not found.

#### corStrEq

```c
bool corStrEq(const char* s1, const char* s2);
```

String equality comparison. Returns true if equal.

#### corCharCount

```c
int corCharCount(char* haystack, char needle);
```

Counts occurrences of character in string.

#### corCharChecksum

```c
char corCharChecksum(void* buf, int bufLen);
```

Computes simple checksum (sum of bytes) of buffer.

### File Functions

#### corFileRead

```c
int corFileRead(char* base, char* relPath, char** bufP, int* bufLenP);
```

Reads entire file into allocated buffer. Returns 0 on success.

#### corFileSuffixExtract

```c
char* corFileSuffixExtract(char* fileName, int len, bool destructive);
```

Extracts file extension from filename.

#### corProgName

```c
char* corProgName(char* argv0);
```

Extracts program name from argv[0], stripping directory path. No allocation needed.

### Float Functions

#### corFloatTrim

```c
void corFloatTrim(char* floatString, double d);
```

Converts double to string, removes trailing zeros. "14.12300" becomes "14.123".

### Time Functions

#### corTimeGet

```c
int corTimeGet(struct timespec* tP);
```

Gets current time using `CLOCK_REALTIME`.

#### corTimeDiff

```c
void corTimeDiff(struct timespec* startP, struct timespec* endP,
                 struct timespec* diffP, float* fP);
```

Calculates time difference. Outputs as timespec and optionally as float (seconds).

#### corTimeAccumulate

```c
void corTimeAccumulate(struct timespec* accumulatedP, struct timespec* partP, float* fP);
```

Adds timespec to accumulator. Useful for measuring total elapsed time.

### Error Handling

#### Data Structures

```c
typedef struct CorErrorItem {
    const char* file;
    int         line;
    const char* function;
    int         code;
    const char* title;
    const char* detail;
} CorErrorItem;

typedef struct CorErrorStack {
    CorErrorItem* errorV;
    int         errorSize;
    int         ix;
} CorErrorStack;
```

#### Functions

```c
void corErrorInit(CorErrorStack* esP, int maxErrorItems);
void corErrorPush(CorErrorStack* esP, int code, const char* title, const char* detail);
void corErrorFlushToScreen(CorErrorStack* esP);
void corErrorFree(CorErrorStack* esP);
```

### Library log (corLibLog.h)

A library cannot know where its executable logs, so it never logs itself: every
line goes to the log function the executable hands to `corBaseInit()`.

```c
typedef void (*CorLibLogFunction)(const char* fileName, int lineNo, const char* functionName,
                                  char type, int aux, const char* format, ...);

void corBaseInit(CorLibLogFunction logFunction);   // corBaseInit.h
```

The signature is corLog's `corLogOut`, so an executable that logs with corLog calls
`corBaseInit(corLogOut)`. `type` is `'E'`, `'W'`, `'I'`, `'V'`, `'T'` or `'X'`;
`aux` is the trace level for `'T'`, the exit code for `'X'`, `-1` otherwise.

```c
COR_LIB_E(fmt, ...)              // Error
COR_LIB_W(fmt, ...)              // Warning
COR_LIB_I(fmt, ...)              // Info
COR_LIB_V(fmt, ...)              // Verbose
COR_LIB_T(level, fmt, ...)       // Trace, with its trace level
COR_LIB_X(exitCode, fmt, ...)    // Error, then exit(exitCode)
COR_LIB_RE(retVal, fmt, ...)     // Error, then return retVal
COR_LIB_RVE(fmt, ...)            // Error, then return (void function)
```

Before `corBaseInit()` a line takes `corLibLogFallback`: errors, warnings and exits
go to stderr (option parsing runs before any log file exists, and its errors must
still be seen), everything else is dropped.

**Trace levels** are the executable's number space, shared by every library: each
library takes its own hundred (corRest 100-199, corNgsild 200-299, corAlloc 300-399;
coraine's own start at 400).

## Building

```bash
make          # Build the library, obj/<build>/corBaseTest and the tools
make clean    # Remove build artifacts
make install  # Build, and copy the tools to bin/
```

## Usage Example

```c
#include "corBase/corProgName.h"
#include "corBase/corStringSplit.h"
#include "corBase/corFileRead.h"
#include "corBase/corTime.h"

int main(int argc, char* argv[])
{
    // Get program name
    char* progName = corProgName(argv[0]);

    // Split a string
    char str[] = "one:two:three";
    char* parts[10];
    int count = corStringSplit(str, ':', parts, 10);

    // Read a file
    char* content;
    int len;
    if (corFileRead("/etc", "hostname", &content, &len) == 0) {
        printf("Hostname: %s\n", content);
        free(content);
    }

    // Measure time
    struct timespec start, end, diff;
    float elapsed;
    corTimeGet(&start);
    // ... do work ...
    corTimeGet(&end);
    corTimeDiff(&start, &end, &diff, &elapsed);
    printf("Elapsed: %.3f seconds\n", elapsed);

    return 0;
}
```

## Design Philosophy

- **No unnecessary allocation** - Functions like `corStringSplit` and `corProgName` avoid malloc
- **In-place operations** - String functions modify input buffers where appropriate
- **C standard library conventions** - NULL checks omitted in performance-critical functions
- **Macro-based logging** - the macros capture file/line/function; the executable decides where the line goes

## License

[Apache 2.0](LICENSE) &copy; 2016-2026 Ken Zangelin
