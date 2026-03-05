#ifndef WINDOW_H
#define WINDOW_H

typedef enum {
    LEAF_NODE,
    SPLIT_NODE
} nodeType;

typedef enum {
    VERTICAL,
    HORIZONTAL
} splitType;

typedef struct frameNode {
    nodeType type;

    union {
        buffer* buf;
        splitType split;
    } item;

    struct frameNode* parent;
    struct frameNode* left;
    struct frameNode* right;
} frameNode;

frameNode* newLeaf(buffer*, frameNode*);
frameNode* splitLeaf(frameNode*, splitType, buffer*);
void drawNode(frameNode*);

#endif
