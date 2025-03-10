#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    struct list_head *curr = head->next;
    while (curr != head) {
        struct list_head *temp = curr->next;
        element_t *elem = list_entry(curr, element_t, list);
        q_release_element(elem);
        curr = temp;
    }
    test_free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;

    element_t *new = malloc(sizeof(element_t));
    if (!new)
        return false;
    new->value = strdup(s);
    if (!new->value) {
        free(new);
        return false;
    }

    INIT_LIST_HEAD(&new->list);

    list_add(&new->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *new_elem = malloc(sizeof(element_t));
    if (!new_elem)
        return false;

    new_elem->value = strdup(s);
    if (!new_elem->value) {
        test_free(new_elem);
        return false;
    }
    INIT_LIST_HEAD(&new_elem->list);
    list_add_tail(&new_elem->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || head->next == head)
        return NULL;

    struct list_head *node = head->next;

    head->next = node->next;
    node->next->prev = head;

    element_t *elem = list_entry(node, element_t, list);

    if (sp && bufsize) {
        strncpy(sp, elem->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return elem;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || head->prev == head)
        return NULL;

    struct list_head *node = head->prev;

    node->prev->next = head;
    head->prev = node->prev;

    element_t *elem = list_entry(node, element_t, list);

    if (sp && bufsize) {
        strncpy(sp, elem->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return elem;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int count = 0;
    struct list_head *curr = head->next;

    while (curr != head) {
        count++;
        curr = curr->next;
    }
    return count;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || head->next == head)
        return false;

    struct list_head *slow = head->next;
    struct list_head *fast = head->next;
    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }
    slow->prev->next = slow->next;
    slow->next->prev = slow->prev;
    list_del(slow);
    q_release_element(list_entry(slow, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || head->next == head)
        return true;
    struct list_head *curr = head->next;
    struct list_head *prev = head;
    while (curr != head) {
        struct list_head *nxt = curr->next;
        const element_t *e_curr = list_entry(curr, element_t, list);
        if (strcmp(e_curr->value, list_entry(nxt, element_t, list)->value) ==
            0) {
            while (strcmp(e_curr->value,
                          list_entry(nxt, element_t, list)->value) == 0) {
                nxt = nxt->next;
            }
            prev->next = nxt;
            nxt->prev = prev;
            curr = nxt;
        } else {
            prev = curr;
            curr = nxt;
        }
    }
    return true;
}
/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    struct list_head *prior = head;
    struct list_head *curr = head->next;
    if (!head || head->next == head)
        return;
    while (curr != head && curr->next != head) {
        struct list_head *first = curr;
        struct list_head *second = curr->next;
        struct list_head *nxt = second->next;

        first->next = second->next;
        second->next = first;
        first->prev = second;
        nxt->prev = first;
        second->prev = prior;
        prior->next = second;

        curr = first->next;
        prior = first;
    }
    return;
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || head->next == head)
        return;

    struct list_head *curr = head;
    do {
        struct list_head *nxt = curr->next;
        curr->next = curr->prev;
        curr->prev = nxt;
        curr = nxt;
    } while (curr != head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    struct list_head *node;
    int i;

    node = head->next;
    for (i = 0; i < k && node != head; i++)
        node = node->next;
    if (i < k)
        return;

    struct list_head tmp;
    INIT_LIST_HEAD(&tmp);

    for (i = 0; i < k; i++) {
        struct list_head *curr_node = head->next;
        list_del(curr_node);
        list_add(curr_node, &tmp);
    }

    q_reverseK(head, k);

    list_splice_tail(head, &tmp);

    head->next = tmp.next;
    head->prev = tmp.prev;
    head->next->prev = head;
    head->prev->next = head;
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || head->next == head)
        return 0;
    int count = 0;
    struct list_head *curr, *next;
    const char *min_val = NULL;
    element_t *elem;

    curr = head->prev;
    elem = list_entry(curr, element_t, list);
    min_val = elem->value;
    count = 1;
    curr = curr->prev;
    while (curr != head) {
        next = curr->prev;
        elem = list_entry(curr, element_t, list);
        if (strcmp(elem->value, min_val) > 0) {
            list_del(curr);
            q_release_element(elem);
        } else {
            min_val = elem->value;
            count++;
        }
        curr = next;
    }
    return count;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || head->next == head)
        return 0;
    int count = 0;
    struct list_head *curr, *next;
    const char *max_val = NULL;
    element_t *elem;

    curr = head->prev;
    elem = list_entry(curr, element_t, list);
    max_val = elem->value;
    count = 1;
    curr = curr->prev;
    while (curr != head) {
        next = curr->prev;
        elem = list_entry(curr, element_t, list);
        if (strcmp(elem->value, max_val) < 0) {
            list_del(curr);
            q_release_element(elem);
        } else {
            max_val = elem->value;
            count++;
        }
        curr = next;
    }
    return count;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
