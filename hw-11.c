#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LEN 50
#define MAX_LINE_LEN 200

typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    char gender;
    int korean;
    int english;
    int math;
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

int cmp_id(Student a, Student b, long long* comp) {
    if (comp) (*comp)++;
    return a.id - b.id;
}

void unsorted_insert(Student** arr, int* count, Student s, long long* comp) {
    int c = *count;

    Student* temp = realloc(*arr, sizeof(Student) * (c + 1));
    if (!temp) {
        perror("unsorted_insert: realloc failed");
        return;
    }

    *arr = temp;
    (*arr)[c] = s;
    *count = c + 1;
}

void unsorted_delete(Student* arr, int* count, int id, long long* comp) {
    if (!arr || !count) return;

    int c = *count;
    if (c <= 0) return;

    Student key;
    key.id = id;

    int idx = -1;
    for (int i = 0; i < c; i++) {
        if (cmp_id(arr[i], key, comp) == 0) {
            idx = i;
            break;
        }
    }

    if (idx == -1) return;

    arr[idx] = arr[c - 1];
    *count = c - 1;
}

int unsorted_search(Student* arr, int count, int id, long long* comp) {
    if (!arr || count <= 0) return -1;

    Student key;
    key.id = id;

    for (int i = 0; i < count; i++) {
        if (cmp_id(arr[i], key, comp) == 0)
            return i;
    }

    return -1;
}

void sorted_insert(Student** arr, int* count, Student s, long long* comp) {
    if (!arr || !count) return;

    int c = *count;
    Student* a = *arr;

    Student* temp = realloc(a, sizeof(Student) * (c + 1));
    if (!temp) {
        perror("sorted_insert: realloc failed");
        return;
    }
    a = temp;
    *arr = temp;

    int pos = c;
    for (int i = 0; i < c; i++) {
        if (cmp_id(a[i], s, comp) > 0) {
            pos = i;
            break;
        }
    }

    for (int i = c; i > pos; i--)
        a[i] = a[i - 1];

    a[pos] = s;
    *count = c + 1;
}

void sorted_delete(Student* arr, int* count, int id, long long* comp) {
    if (!arr || !count) return;

    int c = *count;
    if (c <= 0) return;

    Student key;
    key.id = id;

    int left = 0, right = c - 1, idx = -1;

    while (left <= right) {
        int mid = (left + right) / 2;
        int r = cmp_id(arr[mid], key, comp);

        if (r == 0) {
            idx = mid;
            break;
        }
        if (r < 0) left = mid + 1;
        else right = mid - 1;
    }

    if (idx == -1) return;

    for (int i = idx; i < c - 1; i++)
        arr[i] = arr[i + 1];

    *count = c - 1;
}

int sorted_search(Student* arr, int count, int id, long long* comp) {
    if (!arr || count <= 0) return -1;

    Student key;
    key.id = id;

    int left = 0, right = count - 1;

    while (left <= right) {
        int mid = (left + right) / 2;
        int r = cmp_id(arr[mid], key, comp);

        if (r == 0) return mid;
        if (r < 0) left = mid + 1;
        else right = mid - 1;
    }

    return -1;
}

typedef struct AVLNode {
    Student data;
    struct AVLNode* left;
    struct AVLNode* right;
    int height;
} AVLNode;

int height(AVLNode* n) {
    return n ? n->height : 0;
}

int get_balance(AVLNode* n) {
    return n ? height(n->left) - height(n->right) : 0;
}

int maxValue(int a, int b) {
    return (a > b) ? a : b;
}

AVLNode* rotate_right(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = 1 + maxValue(height(y->left), height(y->right));
    x->height = 1 + maxValue(height(x->left), height(x->right));

    return x;
}

AVLNode* rotate_left(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = 1 + maxValue(height(x->left), height(x->right));
    y->height = 1 + maxValue(height(y->left), height(y->right));

    return y;
}

AVLNode* new_node(Student s) {
    AVLNode* node = (AVLNode*)malloc(sizeof(AVLNode));
    if (node == NULL) {
        fprintf(stderr, "Error: memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    node->data = s;
    node->left = node->right = NULL;
    node->height = 1;
    return node;
}

AVLNode* avl_insert(AVLNode* root, Student s, long long* comp) {
    if (!root) return new_node(s);

    int r = cmp_id(s, root->data, comp);

    if (r < 0)
        root->left = avl_insert(root->left, s, comp);
    else if (r > 0)
        root->right = avl_insert(root->right, s, comp);
    else
        return root;

    root->height = 1 + maxValue(height(root->left), height(root->right));

    int balance = get_balance(root);

    if (balance > 1 && cmp_id(s, root->left->data, comp) < 0)
        return rotate_right(root);

    if (balance < -1 && cmp_id(s, root->right->data, comp) > 0)
        return rotate_left(root);

    if (balance > 1 && cmp_id(s, root->left->data, comp) > 0) {
        root->left = rotate_left(root->left);
        return rotate_right(root);
    }

    if (balance < -1 && cmp_id(s, root->right->data, comp) < 0) {
        root->right = rotate_right(root->right);
        return rotate_left(root);
    }

    return root;
}

AVLNode* min_value_node(AVLNode* node) {
    AVLNode* current = node;
    while (current->left != NULL)
        current = current->left;
    return current;
}

AVLNode* avl_delete(AVLNode* root, int id, long long* comp) {
    if (root == NULL) return root;

    Student key;
    key.id = id;

    int r = cmp_id(key, root->data, comp);

    if (r < 0) {
        root->left = avl_delete(root->left, id, comp);
    }
    else if (r > 0) {
        root->right = avl_delete(root->right, id, comp);
    }
    else {
        if (root->left == NULL || root->right == NULL) {
            AVLNode* temp = root->left ? root->left : root->right;

            if (temp == NULL) {
                temp = root;
                root = NULL;
            }
            else {
                *root = *temp;
            }

            free(temp);
        }
        else {
            AVLNode* temp = min_value_node(root->right);
            root->data = temp->data;
            root->right = avl_delete(root->right, temp->data.id, comp);
        }
    }

    if (root == NULL) return root;

    root->height = 1 + maxValue(height(root->left), height(root->right));

    int balance = get_balance(root);

    if (balance > 1 && get_balance(root->left) >= 0)
        return rotate_right(root);

    if (balance > 1 && get_balance(root->left) < 0) {
        root->left = rotate_left(root->left);
        return rotate_right(root);
    }

    if (balance < -1 && get_balance(root->right) <= 0)
        return rotate_left(root);

    if (balance < -1 && get_balance(root->right) > 0) {
        root->right = rotate_right(root->right);
        return rotate_left(root);
    }

    return root;
}

AVLNode* avl_search(AVLNode* root, int id, long long* comp) {
    Student key;
    key.id = id;

    while (root) {
        int r = cmp_id(key, root->data, comp);

        if (r == 0) return root;
        if (r < 0) root = root->left;
        else root = root->right;
    }

    return NULL;
}

void free_tree(AVLNode* root) {
    if (!root) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

int main() {
    int n;
    Student* original = load_students("dataset_id_ascending.csv", &n);
    if (!original) {
        fprintf(stderr, "Failed to load data\n");
        return 1;
    }
    printf("Loaded %d students.\n\n", n);

    Student* unsorted = malloc(sizeof(Student) * n);
    Student* sorted   = malloc(sizeof(Student) * n);
    memcpy(unsorted, original, sizeof(Student) * n);
    memcpy(sorted,   original, sizeof(Student) * n);

    int count_unsorted = n;
    int count_sorted   = n;

    AVLNode* root = NULL;

    long long u_ins = 0, u_del = 0, u_srh = 0;
    long long s_ins = 0, s_del = 0, s_srh = 0;
    long long a_ins = 0, a_del = 0, a_srh = 0;

    Student dummy = { 999999, "Dummy", 'M', 100, 100, 100 };
    int target_id = original[n/2].id;

    unsorted_insert(&unsorted, &count_unsorted, dummy, &u_ins);
    unsorted_search(unsorted, count_unsorted, target_id, &u_srh);
    unsorted_delete(unsorted, &count_unsorted, target_id, &u_del);

    sorted_insert(&sorted, &count_sorted, dummy, &s_ins);
    sorted_search(sorted, count_sorted, target_id, &s_srh);
    sorted_delete(sorted, &count_sorted, target_id, &s_del);

    for (int i = 0; i < n; i++)
        root = avl_insert(root, original[i], &a_ins);

    root = avl_insert(root, dummy, &a_ins);
    avl_search(root, target_id, &a_srh);
    root = avl_delete(root, target_id, &a_del);

    printf("=== Unsorted Array ===\n");
    printf("Insert comparisons: %lld\n", u_ins);
    printf("Delete comparisons: %lld\n", u_del);
    printf("Search comparisons: %lld\n\n", u_srh);

    printf("=== Sorted Array ===\n");
    printf("Insert comparisons: %lld\n", s_ins);
    printf("Delete comparisons: %lld\n", s_del);
    printf("Search comparisons: %lld\n\n", s_srh);

    printf("=== AVL Tree ===\n");
    printf("Insert comparisons: %lld\n", a_ins);
    printf("Delete comparisons: %lld\n", a_del);
    printf("Search comparisons: %lld\n\n", a_srh);

    free(original);
    free(unsorted);
    free(sorted);
    free_tree(root);

    return 0;
}
