/*
 * LibSunshine
 *
 * Function & type definitions for linked list implementation.
 */

#ifndef List_h
#define List_h

#include <threads.h>

#ifdef __cplusplus
#define INLINE static
extern "C" {
#else
#define INLINE static
#endif

typedef struct List_s_
{
    void * data;
    struct List_s_ * Link;
} List_t_;

typedef struct List_s
{
    List_t_ * List;
    mtx_t Lock;
} List_t;

List_t * List_new ();

void List_add (List_t * n, void * data);
void List_del (List_t * n, void * data);
void List_destroy (List_t * n);

INLINE long List_count (List_t * n)
{
    long length = 0;
    List_t_ * tmp;
    if (!n)
        return 0;
    mtx_lock (&n->Lock);
    for (List_t_ * e = n->List; (e) && (tmp = (e)->Link, 1); (e) = tmp)
        length++;
    mtx_unlock (&n->Lock);
    return length;
}

List_t_ * List_begin (List_t * n);
void List_iterator_next (List_t_ ** it);

void * List_lpop (List_t * n);

#define LIST_ITERATE_OPEN(list)                                                \
    List_t_ * tmp;                                                             \
    void * e_data;                                                             \
    mtx_lock (&(list)->Lock);                                                  \
    for (List_t_ * e = (list)->List;                                           \
         (e) && (tmp = (e)->Link, 1) && (e_data = e->data); (e) = tmp)         \
    {

#define LIST_ITERATE_CLOSE(list)                                               \
    }                                                                          \
    mtx_unlock (&(list)->Lock);

#define ListGenForNameType(name, type)                                         \
    typedef List_t * name##_list;                                              \
    INLINE void name##_list_add (List_t * n, type * data)                      \
    {                                                                          \
        List_add (n, (void *)data);                                            \
    }                                                                          \
    INLINE void name##_list_del (List_t * n, type * data)                      \
    {                                                                          \
        List_del (n, (void *)data);                                            \
    }                                                                          \
    INLINE type * name##_list_lpop (List_t * n)                                \
    {                                                                          \
        return (type *)List_lpop (n);                                          \
    }                                                                          \
    typedef struct                                                             \
    {                                                                          \
        type * val;                                                            \
        struct List_s_ * Link;                                                 \
    } * name##_list_iterator;                                                  \
    INLINE name##_list_iterator name##_list_begin (List_t * n)                 \
    {                                                                          \
        return (name##_list_iterator)List_begin (n);                           \
    }                                                                          \
    INLINE void name##_list_iterator_next (name##_list_iterator * n)           \
    {                                                                          \
        List_iterator_next ((List_t_ **)n);                                    \
    }

#ifdef __cplusplus
}
#endif

#endif
