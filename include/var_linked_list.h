#ifndef VAR_LINKED_LIST_H
#define VAR_LINKED_LIST_H

#include <stdio.h>
#include <gmodule.h>
#include <string.h>

GSList* var_list;

struct var_list_node {
    char* name;
    char* value;
};

void var_list_init();
void var_list_append(char* name, char* value);
char* var_list_get(char* name);
int var_list_set(char* name, char* value);
void print_var_list();
void free_var_list();

gint var_list_get_helper(gconstpointer a, gconstpointer b);
void var_list_set_helper(gpointer data, gpointer user_data);
void print_var_list_helper(gpointer data, gpointer user_data);
void free_var_list_helper(gpointer data, gpointer user_data);

#endif