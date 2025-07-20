/*
 * charmap is a trie structure to quickly lookup names for completion
 */

#include <stdbool.h>

typedef struct cmap_node {
  char value;
  bool end;
  struct node_list *next_possible_chars;
} cmap_node;

typedef struct node_list {
  struct cmap_node chr_node;
  struct node_list *next_child;
} node_list;

typedef struct cmap {
  int max_len;
  struct node_list possible_chars;
} cmap;

void add_to_cmap(cmap *cmap, const char *str);
node_list *find_str_node(node_list *node, const char *str);
char *traverse(cmap *cmap, node_list *node, int min_len, bool *can_continue);
void free_node_list(node_list *n);
