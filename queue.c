#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
struct list_head *q_new()
{
    struct list_head *q = malloc(sizeof(struct list_head));
    if (q == NULL)
        return NULL;
    INIT_LIST_HEAD(q);
    return q;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (l == NULL)
        return;
    while (list_empty(l) == 0) {
        element_t *target = list_first_entry(l, element_t, list);
        list_del(l->next);
        q_release_element(target);
    }
    free(l);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(struct list_head *head, char *s)
{
    if (head == NULL)
        return false;
    element_t *newnode = malloc(sizeof(element_t));
    if (newnode == NULL)
        return false;
    int length = strlen(s) + 1;
    newnode->value = malloc(sizeof(char) * (length));
    if (newnode->value == NULL) {  // Cannot allocate space for value;
        free(newnode);
        return false;
    }
    strncpy(newnode->value, s, length);
    list_add(&newnode->list, head);  // why &??
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (head == NULL)
        return false;
    element_t *newnode = malloc(sizeof(element_t));
    if (newnode == NULL)
        return false;
    int length = strlen(s) + 1;
    newnode->value = malloc(sizeof(char) * (length));
    if (newnode->value == NULL) {  // Cannot allocate space for value;
        free(newnode);
        return false;
    }
    strncpy(newnode->value, s, length);
    list_add_tail(&newnode->list, head);  // why &??
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return target element.
 * Return NULL if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 *
 * NOTE: "remove" is different from "delete"
 * The space used by the list element and the string should not be freed.
 * The only thing "remove" need to do is unlink it.
 *
 * REF:
 * https://english.stackexchange.com/questions/52508/difference-between-delete-and-remove
 */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (head == NULL || list_empty(head) || sp == NULL)
        return NULL;
    element_t *target = list_first_entry(head, element_t, list);
    list_del(head->next);

    int len_tarvalue = strlen(target->value);
    int length = len_tarvalue <= bufsize - 1 ? len_tarvalue : bufsize - 1;
    strncpy(sp, target->value, length);
    sp[length] = '\0';

    return target;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (head == NULL || list_empty(head) || sp == NULL)
        return NULL;
    element_t *target = list_last_entry(head, element_t, list);
    list_del(head->prev);
    int len_tarvalue = strlen(target->value);
    int length = len_tarvalue <= bufsize - 1 ? len_tarvalue : bufsize - 1;
    strncpy(sp, target->value, length);
    sp[length] = '\0';
    return target;
}

/*
 * WARN: This is for external usage, don't modify it
 * Attempt to release element.
 */
void q_release_element(element_t *e)
{
    free(e->value);
    free(e);
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(struct list_head *head)
{
    if (head == NULL || list_empty(head))
        return 0;

    unsigned int count = 0;
    struct list_head *node = NULL;
    list_for_each (node, head)
        count++;
    return count;
}

/*
 * Delete the middle node in list.
 * The middle node of a linked list of size n is the
 * ⌊n / 2⌋th node from the start using 0-based indexing.
 * If there're six element, the third member should be return.
 * Return true if successful.
 * Return false if list is NULL or empty.
 */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (head == NULL || list_empty(head))
        return false;
    struct list_head *slow = NULL, *fast = NULL;
    for (slow = fast = head->next; fast != head && fast->next != head;
         slow = slow->next, fast = fast->next->next) {
    }
    list_del(slow);
    q_release_element(list_entry(slow, element_t, list));
    return true;
}

/*
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,
 * list is guaranteed to be sorted in ascending order.
 */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (head == NULL)
        return false;
    element_t *entry = NULL, *safe = NULL;
    char *pre_value = "";
    struct list_head *same = NULL;
    bool last_dup = false;

    list_for_each_entry_safe (entry, safe, head, list) {
        if (strcmp(pre_value, entry->value) == 0) {
            last_dup = true;
            list_del(&entry->list);
            q_release_element(entry);
        } else {
            if (last_dup == true) {
                list_del(same);
                q_release_element(list_entry(same, element_t, list));
                last_dup = false;
            }
            pre_value = entry->value;
            same = &entry->list;
        }
    }
    return true;
}

/*
 * Attempt to swap every two adjacent nodes.
 */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (head == NULL)
        return;
    struct list_head *node = NULL;
    for (node = head->next; node != head && node->next != head;
         node = node->next) {
        struct list_head *tmp = node->next;
        list_move_tail(tmp, node);
    }
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(struct list_head *head)
{
    if (head == NULL || list_empty(head))
        return;
    struct list_head *node = NULL, *safe = NULL, *tmp = NULL;
    list_for_each_safe (node, safe, head) {
        tmp = node->next;
        node->next = node->prev;
        node->prev = tmp;
    }
    tmp = head->next;
    head->next = head->prev;
    head->prev = tmp;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */

struct list_head *merge(struct list_head *list1, struct list_head *list2)
{
    // merge with recursive
    if (list1 == NULL)
        return list2;
    if (list2 == NULL)
        return list1;

    element_t *list1_entry = list_entry(list1, element_t, list);
    element_t *list2_entry = list_entry(list2, element_t, list);
    if (strcmp(list1_entry->value, list2_entry->value) < 0) {
        list1->next = merge(list1->next, list2);
        return list1;
    } else {
        list2->next = merge(list1, list2->next);
        return list2;
    }
}

struct list_head *mergeSortList(struct list_head *list)
{
    if (!list || !list->next)
        return list;

    // split list
    struct list_head *fast = list->next;
    struct list_head *slow = list;
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
    }
    fast = slow->next;  // Right hand side
    slow->next = NULL;  // Left hand side's tail

    // Sort each list
    struct list_head *l1 = mergeSortList(list);
    struct list_head *l2 = mergeSortList(fast);

    // merge sorted l1 and sorted l2
    return merge(l1, l2);
}

void q_sort(struct list_head *head)
{
    if (head == NULL || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *list = head->next;
    head->prev->next = NULL;
    head->next = mergeSortList(list);

    struct list_head *curr = head, *next = head->next;
    while (next != NULL) {
        next->prev = curr;
        curr = next;
        next = next->next;
    }
    curr->next = head;
    head->prev = curr;
}
