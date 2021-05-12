#include "var_linked_list.h"

void var_list_init(char** envp) {
    var_list_append("?", "0");
    char* first_env_var = "LS_COLOR";
    char* first_env_var_val = strchr(*envp, '=');
    var_list_append(first_env_var, first_env_var_val);

    int cur = 1;
    while (*(envp + cur)) {
        char* name = strtok(*(envp + cur), "=");
        char* val = strtok(NULL, "=");
        var_list_append(name, val);
        cur += 1;
    }
}

void var_list_append(char* name, char* value) {
    struct var_list_node* list_node = malloc(sizeof(struct var_list_node));
    list_node->name = name;
    list_node->value = value;
    var_list = g_slist_append(var_list, (void*) list_node);
}

char* var_list_get(char* name) {
    struct var_list_node list_node = {
        .name = name,
        .value = NULL
    };
    GSList* res = g_slist_find_custom(var_list, (void*) &list_node, var_list_get_helper);
    if (!res) return NULL;
    struct var_list_node* ans_list_node = (struct var_list_node*) res->data;
    return ans_list_node->value;
}

int var_list_set(char* name, char* value) {
    struct var_list_node list_node = {
        .name = name,
        .value = value
    };

    // list_node is the user_data node in this helper
    g_slist_foreach(var_list, var_list_set_helper, (void*) &list_node);
    return strcmp(list_node.name, "");
}

void print_var_list() {
    printf("Printing var_list\n");
    g_slist_foreach(var_list, print_var_list_helper, NULL);
}

void free_var_list() {
    g_slist_foreach(var_list, free_var_list_helper, NULL);
    g_slist_free(var_list);
}


// All of the glist foreach and find functions are under here
gint var_list_get_helper(gconstpointer a, gconstpointer b) {
    struct var_list_node* one = (struct var_list_node*) a;
    struct var_list_node* two = (struct var_list_node*) b;

    if (!(strcmp(one->name, two->name))) {
        return 0;
    }
    return one < two;
}

void var_list_set_helper(gpointer data, gpointer user_data) {
    struct var_list_node* list_node = (struct var_list_node*) data;
    struct var_list_node* new_node = (struct var_list_node*) user_data;

    if (!(strcmp(list_node->name, new_node->name))) {
        list_node->value = new_node->value;
        new_node->name = "";
    }
}

void print_var_list_helper(gpointer data, gpointer user_data) {
    struct var_list_node* list_node = (struct var_list_node*) data;
    printf("%s=%s\n", list_node->name, list_node->value);
}

void free_var_list_helper(gpointer data, gpointer user_data) {
    struct var_list_node* list_node = (struct var_list_node*) data;
    free(list_node);
}