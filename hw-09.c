#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NAME_LEN 50
#define MAX_LINE_LEN 200
#define NUM_ALGOS 9
#define NUM_CRITERIA 8
#define REPEAT 10
#define QS_CUTOFF 10

typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    char gender;
    int korean;
    int english;
    int math;
} Student;

long long bubble_sort(Student arr[], int n, int (*cmp)(Student, Student), long long* mem) {
    long long comparisons = 0;
    *mem = sizeof(Student);

    for (int i = 0; i < n - 1; i++) {
        int swapped = 0;
        for (int j = 0; j < n - 1 - i; j++) {
            comparisons++;
            if (cmp(arr[j], arr[j + 1]) > 0) {
                Student tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
                swapped = 1;
            }
        }
        if (!swapped) break;
    }
    return comparisons;
}

long long selection_sort(Student arr[], int n, int (*cmp)(Student, Student), long long* mem) {
    long long comparisons = 0;
    *mem = sizeof(Student);
    for (int i = 0; i < n - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < n; j++) {
            comparisons++;
            if (cmp(arr[j], arr[min_idx]) < 0) min_idx = j;
        }
        if (min_idx != i) {
            Student tmp = arr[i];
            arr[i] = arr[min_idx];
            arr[min_idx] = tmp;
        }
    }
    return comparisons;
}

long long insertion_sort(Student arr[], int n, int (*cmp)(Student, Student), long long* mem) {
    long long comparisons = 0;
    *mem = sizeof(Student);
    for (int i = 1; i < n; i++) {
        Student key = arr[i];
        int j = i - 1;
        while (j >= 0) {
            comparisons++;
            if (cmp(arr[j], key) <= 0) break;
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
    return comparisons;
}

long long shell_sort(Student arr[], int n, int (*cmp)(Student, Student), long long* mem) {
    long long comparisons = 0;
    *mem = sizeof(Student);

    int gaps[] = { 701, 301, 132, 57, 23, 10, 4, 1 };
    int num_gaps = sizeof(gaps) / sizeof(gaps[0]);

    for (int gi = 0; gi < num_gaps; gi++) {
        int gap = gaps[gi];
        if (gap >= n) continue;
        for (int i = gap; i < n; i++) {
            Student temp = arr[i];
            int j = i;
            while (j >= gap) {
                comparisons++;
                if (cmp(arr[j - gap], temp) <= 0) break;
                arr[j] = arr[j - gap];
                j -= gap;
            }
            arr[j] = temp;
        }
    }
    return comparisons;
}

static void insertion_sort_range(Student arr[], int left, int right, int (*cmp)(Student, Student), long long* comparisons) {
    for (int i = left + 1; i <= right; i++) {
        Student key = arr[i];
        int j = i - 1;
        while (j >= left) {
            (*comparisons)++;
            if (cmp(arr[j], key) <= 0) break;
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

static int median3(Student arr[], int left, int right, int (*cmp)(Student, Student), long long* comparisons) {
    int mid = (left + right) / 2;

    (*comparisons)++;
    if (cmp(arr[left], arr[mid]) > 0) {
        Student tmp = arr[left];
        arr[left] = arr[mid];
        arr[mid] = tmp;
    }
    (*comparisons)++;
    if (cmp(arr[left], arr[right]) > 0) {
        Student tmp = arr[left];
        arr[left] = arr[right];
        arr[right] = tmp;
    }
    (*comparisons)++;
    if (cmp(arr[mid], arr[right]) > 0) {
        Student tmp = arr[mid];
        arr[mid] = arr[right];
        arr[right] = tmp;
    }

    Student tmp = arr[mid];
    arr[mid] = arr[right - 1];
    arr[right - 1] = tmp;
    return right - 1;
}

static void quick_sort_recursive(Student arr[], int left, int right, int (*cmp)(Student, Student), long long* comparisons) {
    if (right - left <= QS_CUTOFF) {
        insertion_sort_range(arr, left, right, cmp, comparisons);
        return;
    }

    int pivotIndex = median3(arr, left, right, cmp, comparisons);
    Student pivot = arr[pivotIndex];

    int i = left;
    int j = right - 1;

    for (;;) {
        while (1) {
            (*comparisons)++;
            if (cmp(arr[++i], pivot) >= 0) break;
        }
        while (1) {
            (*comparisons)++;
            if (cmp(arr[--j], pivot) <= 0) break;
        }
        if (i < j) {
            Student tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
        }
        else break;
    }

    Student tmp = arr[i];
    arr[i] = arr[right - 1];
    arr[right - 1] = tmp;

    if (left < i - 1) quick_sort_recursive(arr, left, i - 1, cmp, comparisons);
    if (i + 1 < right) quick_sort_recursive(arr, i + 1, right, cmp, comparisons);
}

long long quick_sort_wrapper(Student arr[], int n, int (*cmp)(Student, Student), long long* mem) {
    long long comparisons = 0;
    *mem = sizeof(Student);
    quick_sort_recursive(arr, 0, n - 1, cmp, &comparisons);
    return comparisons;
}

static void heapify(Student arr[], int n, int i, int (*cmp)(Student, Student), long long* comparisons) {
    int largest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;

    if (l < n) {
        (*comparisons)++;
        if (cmp(arr[l], arr[largest]) > 0) largest = l;
    }
    if (r < n) {
        (*comparisons)++;
        if (cmp(arr[r], arr[largest]) > 0) largest = r;
    }
    if (largest != i) {
        Student tmp = arr[i];
        arr[i] = arr[largest];
        arr[largest] = tmp;
        heapify(arr, n, largest, cmp, comparisons);
    }
}

long long heap_sort(Student arr[], int n, int (*cmp)(Student, Student), long long* mem) {
    long long comparisons = 0;
    *mem = sizeof(Student);

    for (int i = n / 2 - 1; i >= 0; i--) heapify(arr, n, i, cmp, &comparisons);
    for (int i = n - 1; i > 0; i--) {
        Student tmp = arr[0];
        arr[0] = arr[i];
        arr[i] = tmp;
        heapify(arr, i, 0, cmp, &comparisons);
    }
    return comparisons;
}

static void merge(Student arr[], int left, int mid, int right, int (*cmp)(Student, Student), long long* mem, long long* comparisons) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    long long needed = (long long)(sizeof(Student) * (n1 + n2));
    if (needed > *mem) *mem = needed;

    Student* L = (Student*)malloc(sizeof(Student) * n1);
    Student* R = (Student*)malloc(sizeof(Student) * n2);
    if (!L || !R) {
        perror("malloc failed in merge");
        exit(1);
    }

    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        (*comparisons)++;
        if (cmp(L[i], R[j]) <= 0) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    free(L);
    free(R);
}

static void merge_sort_recursive(Student arr[], int left, int right, int (*cmp)(Student, Student), long long* mem, long long* comparisons) {
    if (left >= right) return;
    int mid = (left + right) / 2;
    merge_sort_recursive(arr, left, mid, cmp, mem, comparisons);
    merge_sort_recursive(arr, mid + 1, right, cmp, mem, comparisons);
    merge(arr, left, mid, right, cmp, mem, comparisons);
}

long long merge_sort_wrapper(Student arr[], int n, int (*cmp)(Student, Student), long long* mem) {
    long long comparisons = 0;
    *mem = 0;
    merge_sort_recursive(arr, 0, n - 1, cmp, mem, &comparisons);
    return comparisons;
}

long long radix_sort(Student arr[], int n, int (*cmp)(Student, Student), long long* mem) {
    long long comparisons = 0;
    *mem = 0;
    if (n <= 1) return 0;

    int ascending = 1;
    if (cmp != NULL) {
        Student s1 = { 1, "", 'M', 0, 0, 0 };
        Student s2 = { 2, "", 'M', 0, 0, 0 };
        int r = cmp(s1, s2);
        if (r > 0) ascending = 0;
    }

    int maxID = arr[0].id;
    for (int i = 1; i < n; i++) {
        comparisons++;
        if (arr[i].id > maxID) maxID = arr[i].id;
    }

    Student* output = (Student*)malloc(sizeof(Student) * n);
    if (!output) {
        perror("malloc failed");
        exit(1);
    }
    *mem = sizeof(Student) * n;

    int* count = (int*)calloc(10, sizeof(int));
    if (!count) {
        perror("calloc failed");
        exit(1);
    }
    *mem += sizeof(int) * 10;

    for (int exp = 1; maxID / exp > 0; exp *= 10) {
        for (int i = 0; i < 10; i++) count[i] = 0;
        for (int i = 0; i < n; i++) {
            int d = (arr[i].id / exp) % 10;
            count[d]++;
        }
        if (ascending) {
            for (int i = 1; i < 10; i++) count[i] += count[i - 1];
        }
        else {
            for (int i = 8; i >= 0; i--) count[i] += count[i + 1];
        }
        for (int i = n - 1; i >= 0; i--) {
            int d = (arr[i].id / exp) % 10;
            int pos = count[d] - 1;
            output[pos] = arr[i];
            count[d]--;
        }
        for (int i = 0; i < n; i++) arr[i] = output[i];
    }

    free(output);
    free(count);
    return comparisons;
}

typedef struct TreeNode {
    Student data;
    struct TreeNode* left;
    struct TreeNode* right;
    int height;
} TreeNode;

int node_height(TreeNode* n) {
    return n ? n->height : 0;
}

int get_balance(TreeNode* n) {
    if (!n) return 0;
    return node_height(n->left) - node_height(n->right);
}

TreeNode* right_rotate(TreeNode* y) {
    if (!y || !y->left) return y;
    TreeNode* x = y->left;
    TreeNode* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = 1 + (node_height(y->left) > node_height(y->right) ? node_height(y->left) : node_height(y->right));
    x->height = 1 + (node_height(x->left) > node_height(x->right) ? node_height(x->left) : node_height(x->right));

    return x;
}

TreeNode* left_rotate(TreeNode* x) {
    if (!x || !x->right) return x;
    TreeNode* y = x->right;
    TreeNode* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = 1 + (node_height(x->left) > node_height(x->right) ? node_height(x->left) : node_height(x->right));
    y->height = 1 + (node_height(y->left) > node_height(y->right) ? node_height(y->left) : node_height(y->right));

    return y;
}

static TreeNode* avl_insert(TreeNode* node, const Student* data, int (*cmp)(Student, Student), long long* mem, long long* compare) {
    if (node == NULL) {
        TreeNode* newNode = (TreeNode*)malloc(sizeof(TreeNode));
        if (!newNode) {
            perror("malloc failed");
            exit(1);
        }
        newNode->data = *data;
        newNode->left = newNode->right = NULL;
        newNode->height = 1;
        *mem += sizeof(TreeNode);
        return newNode;
    }

    (*compare)++;
    if (cmp(*data, node->data) < 0) {
        node->left = avl_insert(node->left, data, cmp, mem, compare);
    }
    else {
        node->right = avl_insert(node->right, data, cmp, mem, compare);
    }

    node->height = 1 + (node_height(node->left) > node_height(node->right) ? node_height(node->left) : node_height(node->right));

    int balance = get_balance(node);

    if (balance > 1 && cmp(*data, node->left->data) < 0)
        return right_rotate(node);

    if (balance < -1 && cmp(*data, node->right->data) > 0)
        return left_rotate(node);

    if (balance > 1 && cmp(*data, node->left->data) > 0) {
        node->left = left_rotate(node->left);
        return right_rotate(node);
    }

    if (balance < -1 && cmp(*data, node->right->data) < 0) {
        node->right = right_rotate(node->right);
        return left_rotate(node);
    }

    return node;
}

static void inorder_iter_dynamic_stack(TreeNode* root, Student arr[], int* idx) {
    if (!root) return;

    int cap = 1024;
    TreeNode** stack = (TreeNode**)malloc(sizeof(TreeNode*) * cap);
    int top = -1;
    TreeNode* cur = root;

    while (cur != NULL || top != -1) {
        while (cur != NULL) {
            if (top + 1 >= cap) {
                cap *= 2;
                stack = (TreeNode**)realloc(stack, sizeof(TreeNode*) * cap);
                if (!stack) {
                    perror("realloc failed");
                    exit(1);
                }
            }
            stack[++top] = cur;
            cur = cur->left;
        }
        cur = stack[top--];
        arr[(*idx)++] = cur->data;
        cur = cur->right;
    }

    free(stack);
}

static void free_tree_dynamic(TreeNode* root) {
    if (!root) return;

    int cap = 1024;
    TreeNode** stack = (TreeNode**)malloc(sizeof(TreeNode*) * cap);
    int top = -1;
    TreeNode* lastVisited = NULL;
    TreeNode* cur = root;

    while (cur != NULL || top != -1) {
        if (cur != NULL) {
            if (top + 1 >= cap) {
                cap *= 2;
                stack = (TreeNode**)realloc(stack, sizeof(TreeNode*) * cap);
                if (!stack) {
                    perror("realloc failed");
                    exit(1);
                }
            }
            stack[++top] = cur;
            cur = cur->left;
        }
        else {
            TreeNode* peek = stack[top];
            if (peek->right != NULL && lastVisited != peek->right) {
                cur = peek->right;
            }
            else {
                top--;
                lastVisited = peek;
                free(peek);
            }
        }
    }
    free(stack);
}

long long tree_sort(Student arr[], int n, int (*cmp)(Student, Student), long long* mem) {
    *mem = 0;
    long long compare = 0;
    TreeNode* root = NULL;

    for (int i = 0; i < n; i++) {
        root = avl_insert(root, &arr[i], cmp, mem, &compare);
    }

    int idx = 0;
    inorder_iter_dynamic_stack(root, arr, &idx);
    free_tree_dynamic(root);

    return compare;
}

int cmp_id_asc(Student a, Student b) { return a.id - b.id; }
int cmp_id_desc(Student a, Student b) { return b.id - a.id; }
int cmp_name_asc(Student a, Student b) { return strcmp(a.name, b.name); }
int cmp_name_desc(Student a, Student b) { return strcmp(b.name, a.name); }
int cmp_gender_asc(Student a, Student b) { return (int)a.gender - (int)b.gender; }
int cmp_gender_desc(Student a, Student b) { return (int)b.gender - (int)a.gender; }

int cmp_grade_sum_asc(Student a, Student b) {
    int sa = a.korean + a.english + a.math;
    int sb = b.korean + b.english + b.math;
    if (sa != sb) return sa - sb;
    if (a.korean != b.korean) return b.korean - a.korean;
    if (a.english != b.english) return b.english - a.english;
    if (a.math != b.math) return b.math - a.math;
    return 0;
}

int cmp_grade_sum_desc(Student a, Student b) {
    int sa = a.korean + a.english + a.math;
    int sb = b.korean + b.english + b.math;
    if (sa != sb) return sb - sa;
    if (a.korean != b.korean) return b.korean - a.korean;
    if (a.english != b.english) return b.english - a.english;
    if (a.math != b.math) return b.math - a.math;
    return 0;
}

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
        strncpy(s.name, token, MAX_NAME_LEN);

        token = strtok(NULL, ",");
        s.gender = token[0];

        token = strtok(NULL, ",");
        s.korean = atoi(token);

        token = strtok(NULL, ",");
        s.english = atoi(token);

        token = strtok(NULL, ",");
        s.math = atoi(token);

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

int main(void) {
    int count = 0;
    Student* original = load_students("dataset_id_ascending.csv", &count);
    if (!original) {
        fprintf(stderr, "Failed to load dataset.\n");
        return 1;
    }

    int (*criteria_cmp[NUM_CRITERIA])(Student, Student) = {
        cmp_id_asc, cmp_id_desc,
        cmp_name_asc, cmp_name_desc,
        cmp_gender_asc, cmp_gender_desc,
        cmp_grade_sum_asc, cmp_grade_sum_desc
    };

    const char* criteria_name[NUM_CRITERIA] = {
        "ID ASC", "ID DESC",
        "NAME ASC", "NAME DESC",
        "GENDER ASC", "GENDER DESC",
        "GRADE SUM ASC", "GRADE SUM DESC"
    };

    long long (*algos[NUM_ALGOS])(Student*, int, int (*)(Student, Student), long long*) = {
        bubble_sort, selection_sort, insertion_sort, shell_sort, quick_sort_wrapper,
        heap_sort, merge_sort_wrapper, radix_sort, tree_sort
    };

    const char* algo_name[NUM_ALGOS] = {
        "Bubble Sort", "Selection Sort", "Insertion Sort", "Shell Sort", "Quick Sort",
        "Heap Sort", "Merge Sort", "Radix Sort", "Tree Sort"
    };

    printf("=== Sorting Benchmarks (%d records) ===\n", count);

    Student* arr = (Student*)malloc(sizeof(Student) * count);
    if (!arr) {
        perror("malloc failed");
        free(original);
        return 1;
    }

    for (int c = 0; c < NUM_CRITERIA; c++) {
        printf("\n--- Criterion: %s ---\n", criteria_name[c]);
        for (int a = 0; a < NUM_ALGOS; a++) {
            int is_gender = (c == 4 || c == 5);
            int is_stable_algo = (strcmp(algo_name[a], "Bubble Sort") == 0) ||
                (strcmp(algo_name[a], "Insertion Sort") == 0) ||
                (strcmp(algo_name[a], "Merge Sort") == 0);
            if (is_gender && !is_stable_algo) {
                printf("%-15s : SKIPPED (GENDER uses only Bubble/Insertion/Merge)\n", algo_name[a]);
                continue;
            }

            int has_duplicate = 0;
            for (int i = 1; i < count; i++) {
                if (criteria_cmp[c](original[i - 1], original[i]) == 0) {
                    has_duplicate = 1;
                    break;
                }
            }
            if ((strcmp(algo_name[a], "Heap Sort") == 0 || strcmp(algo_name[a], "Tree Sort") == 0) && has_duplicate) {
                printf("%-15s : SKIPPED (duplicate data present)\n", algo_name[a]);
                continue;
            }

            long long total_comp = 0;
            long long total_mem = 0;
            for (int r = 0; r < REPEAT; r++) {
                memcpy(arr, original, sizeof(Student) * count);
                long long mem_used = 0;
                long long comp = algos[a](arr, count, criteria_cmp[c], &mem_used);
                total_comp += comp;
                total_mem += mem_used;
            }
            printf("%-15s : Avg Comparisons = %lld, Avg Memory = %lld bytes\n",
                algo_name[a], total_comp / REPEAT, total_mem / REPEAT);
        }
    }

    free(arr);
    free(original);
    return 0;
}
