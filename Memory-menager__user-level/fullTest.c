#include "arena.h"
// #include "minunit.h"
#include "malloc.h"
#include <unistd.h>
#include "validate.h"
#include "fullTest.h"
#include "stdio.h"

void stop() {
    return;
}

int main() {
    initMallocs();
    for (int i=0; i < MALLOCS_SIZE; i++) {
        void *ptr = NULL;
        switch (data[i][0]) {
            case 0:
                ptr = randomAddrMallocs(data[i][1]);
                if (ptr != NULL) {
                    removeMallocs(ptr);
                    ptr = foo_realloc(ptr, data[i][1]);
                    insertMallocs(ptr, data[i][1], 0);
                }
                break;
            case 1:
            case 2:
                ptr = foo_malloc(data[i][1]);
                insertMallocs(ptr, data[i][1], 0);
                break;
            case 3:
                ptr = randomAddrMallocs(data[i][1]);
                if (ptr != NULL) {
                    foo_free(ptr);
                    removeMallocs(ptr);
                    
                }
                break;
            case 4:
            case 5:
                foo_posix_memalign(&ptr, data[i][2], data[i][1]);
                insertMallocs(ptr, data[i][1], data[i][2]);
                break;
        }
        // mdump();
        validate_menager();
    }
    stop();
	return 0;
}

