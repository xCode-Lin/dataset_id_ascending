#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NAME_LEN 50
#define MAX_LINE_LEN 200

typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    char gender;
    int korean;
    int english;
    int math;
    long long product;
} Student;

Student* load_students(const char* filename, int* out_count) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror("Failed to open file");
        return NULL;
    }

    char line[MAX_LINE_LEN];
    int capacity = 10;
    int count = 0;
    Student* arr = malloc(sizeof(Student) * capacity);

    if (!arr) {
        perror("Memory allocation failed");
        fclose(fp);
        return NULL;
    }

    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp)) {
        if (count >= capacity) {
            capacity *= 2;
            Student* temp = realloc(arr, sizeof(Student) * capacity);
            if (!temp) {
                perror("Reallocation failed");
                free(arr);
                fclose(fp);
                return NULL;
            }
            arr = temp;
        }

        Student s;
        char* token = strtok(line, ",");
        s.id = atoi(token);

        token = strtok(NULL, ",");
        strncpy(s.name, token, MAX_NAME_LEN - 1);
        s.name[MAX_NAME_LEN - 1] = '\0';

        token = strtok(NULL, ",");
        s.gender = token[0];

        token = strtok(NULL, ",");
        s.korean = atoi(token);

        token = strtok(NULL, ",");
        s.english = atoi(token);

        token = strtok(NULL, ",");
        s.math = atoi(token);

        s.product = 0;

        arr[count++] = s;
    }

    fclose(fp);

    Student* tight = realloc(arr, sizeof(Student) * count);
    if (!tight) {
        fprintf(stderr, "Warning: Tight reallocation failed, using original memory.\n");
        *out_count = count;
        return arr;
    }

    arr = tight;
    
    *out_count = count;
    return arr;
}

long long linear_search(Student arr[], int size, int target, long long* count) {
    *count = 0;

    for (int i = 0; i < size; i++) {
        (*count)++;

        if (arr[i].product == target) {
            return i;
        }
    }

    return -1;
}

long long binary_search(Student arr[], int size, int target, long long* count) {
    int left = 0;
    int right = size - 1;
    *count = 0;

    while (left <= right) {
        int mid = (left + right) / 2;
        (*count)++;

        if (arr[mid].product == target) {
            return mid;
        }
        else if (arr[mid].product < target) {
            left = mid + 1;
        }
        else {
            right = mid - 1;
        }
    }

    return -1;
}

int compare_by_product(const void* a, const void* b) {
    const Student* s1 = (const Student*)a;
    const Student* s2 = (const Student*)b;

    if (s1->product < s2->product) return -1;
    if (s1->product > s2->product) return 1;
    return 0;
}

void sort_students(Student arr[], int size) {
    qsort(arr, size, sizeof(Student), compare_by_product);
}

int main() {
    srand((unsigned)time(NULL));

    int count = 0;
    Student* students = load_students("dataset_id_ascending.csv", &count);
    if (!students) {
        printf("Failed to load students.\n");
        return 1;
    }

    printf("Loaded %d students.\n", count);

    for (int i = 0; i < count; i++) {
        students[i].product = (long long)students[i].korean *
            students[i].english *
            students[i].math;
    }

    int target = rand() % 1000001;
    printf("Target value: %d\n", target);

    long long linear_count = 0;
    long long linear_result = linear_search(students, count, target, &linear_count);
    printf("Linear Search: result=%lld, comparisons=%lld\n", linear_result, linear_count);

    sort_students(students, count);

    long long binary_count = 0;
    long long binary_result = binary_search(students, count, target, &binary_count);
    printf("Binary Search: result=%lld, comparisons=%lld\n", binary_result, binary_count);

    free(students);
    return 0;
}
