#include <stdlib.h>
#include <limits.h>
#include <ncurses.h>
#include "window.h"
#include "buffer.h"

/*
 * Returns a new leaf.
 */
frameNode* newLeaf(buffer* buf, frameNode* parent)
{
    if (!buf) return NULL;

    frameNode* n = calloc(1, sizeof(frameNode));
    if (!n) return NULL;

    n->type = LEAF_NODE;
    n->item.buf = buf;
    if (parent) n->parent = parent;

    return n;
}

/*
 * Returns a new split node whose left node is `node` and right node is a new node.
 */
frameNode* newSplit(frameNode* node, splitType split, buffer* newBuf)
{
    if (!node || !newBuf || node->type != LEAF_NODE) return NULL;

    frameNode* parent = node->parent;
    frameNode* n = calloc(1, sizeof(frameNode));
    if (!n) return NULL;
    frameNode* tempNode = calloc(1, sizeof(frameNode));
    if (!tempNode) {
        free(n);
        return NULL;
    }

    n->type = SPLIT_NODE;
    n->item.split = split;
    n->parent = parent;
    n->left = node;
    n->right = tempNode;

    tempNode->type = LEAF_NODE;
    tempNode->item.buf = newBuf;
    tempNode->parent = n;
    node->parent = n;

    if (parent) { // TODO: add better management of the root node
        if (parent->left == node)
            parent->left = n;
        else if (parent->right == node)
            parent->right = n;
    }

    return n;
}

/*
 * Draw an individual node. Recurses if `node->type == SPLIT_NODE`.
 */
void drawNode(frameNode* node, int x, int y, int w, int h)
{
    if (!node) return;

    if (node->type == LEAF_NODE) {
        node->x = x;
        node->y = y;
        node->width = w;
        node->height = h;
        buffer* buf = node->item.buf;
        for (int i = 0; i < buf->numrows && i < h - 1; i++) {
            move(y + i, x);
            for (int j = 0; j < buf->rows[i].length && j < w; j++)
                addch(buf->rows[i].line[j]);
        }
        for (int i = 0; i < w; i++)
            mvaddch(y + h - 1, x + i, '-');
    } else if (node->type == SPLIT_NODE) {
        if (node->item.split == VERTICAL) {
            int wLeft = w * 0.5; // TODO: add better ratios
            int wRight = w - wLeft - 1; // -1 to account for the border
            drawNode(node->left, x, y, wLeft, h);
            for (int i = 0; i < h; i++)
                mvaddch(y + i, x + wLeft, '|');
            drawNode(node->right, x + wLeft + 1, y, wRight, h);
        } else { // node->type == SPLIT_NODE
            int hTop = h * 0.5;
            int hBot = h - hTop;
            drawNode(node->left, x, y, w, hTop);
            drawNode(node->right, x, y + hTop, w, hBot);
        }
    }
}

/*
 * Returns closest neighbor.
 */
frameNode* neighborInDir(frameNode** leaves, int count, frameNode* focused, int dir)
{
    // dir: 0=left, 1=down, 2=up, 3=right
    frameNode* best = NULL;
    int bestDist = INT_MAX;
    // int fcx = focused->x + focused->width / 2;
    // int fcy = focused->y + focused->height / 2;

    for (int i = 0; i < count; i++) {
        frameNode* l = leaves[i];
        if (l == focused) continue;
        // int lcx = l->x + l->width / 2;
        // int lcy = l->y + l->height / 2;
        int dist;
        switch (dir) {
            case 0: if (l->x + l->width > focused->x) continue; dist = focused->x - l->x; break;
            case 1: if (l->y < focused->y + focused->height) continue; dist = l->y - focused->y; break;
            case 2: if (l->y + l->height > focused->y) continue; dist = focused->y - l->y; break;
            case 3: if (l->x < focused->x + focused->width) continue; dist = l->x - focused->x; break;
            default: continue;
        }
        if (dist < bestDist) { bestDist = dist; best = l; }
    }
    return best;
}

void collectLeaves(frameNode* node, frameNode** leaves, int* count)
{
    if (!node) return;
    if (node->type == LEAF_NODE)
        leaves[(*count)++] = node;
    else {
        collectLeaves(node->left, leaves, count);
        collectLeaves(node->right, leaves, count);
    }
}

frameNode* getRoot(frameNode* node)
{
    while (node->parent)
        node = node->parent;
    return node;
}
