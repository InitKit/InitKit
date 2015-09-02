/*
 * LibSunshine
 *
 * Function & type definitions for linked list implementation.
 */

#ifndef List_h
#define List_h

#ifdef __cplusplus
extern "C" {
#endif

#include <threads.h>

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
    inline void name##_list_add (List_t * n, type * data)                      \
    {                                                                          \
        List_add (n, (void *)data);                                            \
    }                                                                          \
    inline void name##_list_del (List_t * n, type * data)                      \
    {                                                                          \
        List_del (n, (void *)data);                                            \
    }                                                                          \
    inline type * name##_list_lpop (List_t * n)                                \
    {                                                                          \
        return (type *)List_lpop (n);                                          \
    }                                                                          \
    typedef struct                                                             \
    {                                                                          \
        type * val;                                                            \
        struct List_s_ * Link;                                                 \
    } name##_list_iterator;                                                    \
    inline name##_list_iterator * name##_list_begin (List_t * n)               \
    {                                                                          \
        return (name##_list_iterator *)List_begin (n);                         \
    }                                                                          \
    inline void name##_list_iterator_next (name##_list_iterator ** n)          \
    {                                                                          \
        List_iterator_next ((List_t_ **)n);                                    \
    }

#ifdef __cplusplus
}
#endif

#endif
