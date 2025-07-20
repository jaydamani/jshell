#include "chartrie.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

node_list *create_empty_node() {
  struct node_list *node = malloc(sizeof(node_list));
  node->chr_node.value = '\0';
  node->chr_node.next_possible_chars = NULL;
  node->chr_node.end = false;
  return node;
}

cmap_node *get_char_node(node_list *list, char c) {
  node_list *p = list;
  while (p->chr_node.value != '\0') {
    if (p->chr_node.value == c)
      return &p->chr_node;
    p = p->next_child;
  }
  p->chr_node.value = c;
  p->chr_node.next_possible_chars = create_empty_node();
  p->next_child = create_empty_node();
  return &p->chr_node;
}

void add_to_cmap(cmap *cmap, const char *str) {
  int len = strlen(str);
  node_list *list = &cmap->possible_chars;
  cmap_node *node;
  while (*str != '\0') {
    node = get_char_node(list, *str);
    list = node->next_possible_chars;
    str++;
  }
  node->end = true;
  if (cmap->max_len < len)
    cmap->max_len = len;
}

node_list *find_str_node(node_list *node, const char *str) {
  while (*str != '\0') {
    while (node->chr_node.value != *str) {
      if (node->chr_node.value == '\0')
        return NULL;
      node = node->next_child;
    }
    node = node->chr_node.next_possible_chars;
    str++;
  }
  return node;
}

char *traverse(cmap *cmap, node_list *node, int min_len, bool *can_continue) {
  char *buffer = malloc(cmap->max_len);
  int len = 0;
  while (node->chr_node.value != '\0') {
    buffer[len] = node->chr_node.value;
    len++;
    if (node->chr_node.end && len > min_len) break;
    node = node->chr_node.next_possible_chars;
  }
  *can_continue = node->chr_node.value != '\0';
  char *res = strndup(buffer, len);
  free(buffer);
  return res;
}

void free_node_list(node_list *n) {
    while (n->chr_node.value != '\0') {
        free_node_list(n->chr_node.next_possible_chars);
        node_list *tmp = n;
        n = n->next_child;
        free(tmp);
    }
    free(n);
}

void free_cmap(cmap *m) {
    free_node_list(m->possible_chars.next_child);
    free_node_list(m->possible_chars.chr_node.next_possible_chars);
}

// int main(int argc, char **arg) {
//   cmap m = {.max_len = 0};
//   add_to_cmap(&m, "echo");
//   add_to_cmap(&m, "history");
//   add_to_cmap(&m, "exit");
//   add_to_cmap(&m, "his");
//   bool cont;
//   node_list *n = &m.possible_chars;
//   char *e = traverse(&m, n, 0, &cont);
//   printf(">>>>>>>%s<<<<<<<<<<<<<<<<<<<\n", e);
//   free(e);
// }
