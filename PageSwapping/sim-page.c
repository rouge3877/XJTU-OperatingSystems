#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>

// Configurations
#define RANDOM_PAGE_UB 20
#define RESET "\033[0m"
#define RED_BACKGROUND_BOLD "\033[41;1m"
#define YELLOW_BOLD "\033[33;1m"
#define BLUE_BOLD "\033[34;1m"

// Macro for printing hit or miss
#define PRINT_HIT_OR_MISS(hit) \
    printf(hit ? " " YELLOW_BOLD "hit  😀" RESET "\n" : " " BLUE_BOLD "miss 😱" RESET "\n")


// Function to check if a page is in memory
bool is_in_memory(int *frames, int frame_size, int page) {
    for (int i = 0; i < frame_size; i++) {
        if (frames[i] == page) {
            return true;
        }
    }
    return false;
}

// Function to print the current frame state with highlighted changes
void print_frames(int *frames, int frame_size, int changed_index) {
    printf("[ ");
    for (int i = 0; i < frame_size; i++) {
        if (i == changed_index) {
            // Highlight the changed frame
            printf(RED_BACKGROUND_BOLD "%2d" RESET " ", frames[i]);
        } else if (frames[i] == -1) {
            printf(" - ");
        } else {
            printf("%2d ", frames[i]);
        }
    }
    printf("]");
}

// FIFO Page Replacement Algorithm
void fifo(int *pages, int num_pages, int frame_size) {
    int *frames = (int *)malloc(frame_size * sizeof(int));
    int front = 0, page_faults = 0;

    for (int i = 0; i < frame_size; i++) {
        frames[i] = -1; // Initialize frames as empty
    }

    printf("\nFIFO Page Replacement:\n");
    for (int i = 0; i < num_pages; i++) {
        bool hit = is_in_memory(frames, frame_size, pages[i]);

        if (!hit) {
            // Page fault: replace the oldest page
            frames[front] = pages[i];
            front = (front + 1) % frame_size;
            page_faults++;
        }

        // Print current frame state
        printf("Page %2d -> ", pages[i]);
        print_frames(frames, frame_size, hit ? -1 : front == 0 ? frame_size - 1 : front - 1);
        PRINT_HIT_OR_MISS(hit);
    }

    printf("\nTotal Page Faults: %d\n", page_faults);
    printf("Page Miss Rate: %.2f%%\n", (page_faults / (float)num_pages) * 100);
    printf("Page Fault rate (without cold miss): %.2f%%\n", ((page_faults - frame_size) / (float)(num_pages - frame_size)) * 100);
    free(frames);
}

// LRU Page Replacement Algorithm
void lru(int *pages, int num_pages, int frame_size) {
    int *frames = (int *)malloc(frame_size * sizeof(int));
    int *timestamps = (int *)malloc(frame_size * sizeof(int)); // To track usage
    int time = 0, page_faults = 0;

    for (int i = 0; i < frame_size; i++) {
        frames[i] = -1; // Initialize frames as empty
        timestamps[i] = 0;
    }

    printf("\nLRU Page Replacement:\n");
    for (int i = 0; i < num_pages; i++) {
        time++;
        bool hit = false;
        int replaced_index = -1;

        // Check if the page is already in memory
        for (int j = 0; j < frame_size; j++) {
            if (frames[j] == pages[i]) {
                hit = true;
                timestamps[j] = time; // Update timestamp on hit
                break;
            }
        }

        if (!hit) {
            // Page fault: replace the least recently used page
            int lru_index = 0;
            for (int j = 1; j < frame_size; j++) {
                if (timestamps[j] < timestamps[lru_index]) {
                    lru_index = j;
                }
            }

            replaced_index = lru_index;
            frames[lru_index] = pages[i];
            timestamps[lru_index] = time;
            page_faults++;
        }

        // Print current frame state
        printf("Page %2d -> ", pages[i]);
        print_frames(frames, frame_size, hit ? -1 : replaced_index);
        PRINT_HIT_OR_MISS(hit);
    }

    printf("\nTotal Page Faults: %d\n", page_faults);
    printf("Page Miss Rate: %.2f%%\n", (page_faults / (float)num_pages) * 100);
    printf("Page Fault rate (without cold miss): %.2f%%\n", ((page_faults - frame_size) / (float)(num_pages - frame_size)) * 100);

    free(frames);
    free(timestamps);
}


// Generate a random page sequence
void generate_random_pages(int *pages, int num_pages, int max_page) {
    srand(time(NULL));
    for (int i = 0; i < num_pages; i++) {
        pages[i] = rand() % max_page + 1;
    }
}

// Main function
int main() {
    int algorithm_selection, is_random_choice_enabled;
    int total_page_count, physical_page_size;
    printf("Enter the number of pages: ");
    scanf("%d", &total_page_count);
    printf("Enter the size of frames: ");
    scanf("%d", &physical_page_size);

    assert(total_page_count > 0 && physical_page_size > 0);

    int *allocated_pages = (int *)malloc(total_page_count * sizeof(int));

    printf("\nChoose page sequence generation method:\n");
    printf("1. Randomly generate page sequence\n");
    printf("2. Manually input page sequence\n");
    printf("Enter your choice: ");
    scanf("%d", &is_random_choice_enabled);

    if (is_random_choice_enabled == 1) {
        generate_random_pages(allocated_pages, total_page_count, RANDOM_PAGE_UB);
        printf("Generated Page Sequence: ");
        for (int i = 0; i < total_page_count; i++) {
            printf("%d ", allocated_pages[i]);
        }
        printf("\n");
    } else if (is_random_choice_enabled == 2) {
        printf("Enter the page sequence: ");
        for (int i = 0; i < total_page_count; i++) {
            scanf("%d", &allocated_pages[i]);
        }
    } else {
        printf("Invalid choice.\n");
        free(allocated_pages);
        return 1;
    }

    printf("\nChoose the algorithm to run:\n");
    printf("1. FIFO\n");
    printf("2. LRU\n");
    printf("Enter your choice: ");
    scanf("%d", &algorithm_selection);

    if (algorithm_selection == 1) {
        fifo(allocated_pages, total_page_count, physical_page_size);
    } else if (algorithm_selection == 2) {
        lru(allocated_pages, total_page_count, physical_page_size);
    } else {
        printf("Invalid choice.\n");
    }

    free(allocated_pages);
    return 0;
}
