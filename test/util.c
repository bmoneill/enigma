#include <stdio.h>
#include <unistd.h>

#define TEST_DATA_DIR_1 "test/data/"
#define TEST_DATA_DIR_2 "data/"
#define TEST_DATA_DIR_3 "../test/data/"
#define TEST_DATA_DIR_4 "../../test/data/"

static const char* paths[] = { TEST_DATA_DIR_1, TEST_DATA_DIR_2, TEST_DATA_DIR_3, TEST_DATA_DIR_4 };
static char        path_buffer[64];

char*              get_path(const char* filename) {
    for (int i = 0; i < 4; i++) {
        sprintf(path_buffer, "%s%s", paths[i], filename);
        if (access(path_buffer, F_OK) == 0) {
            return path_buffer;
        }
    }
    return NULL;
}
