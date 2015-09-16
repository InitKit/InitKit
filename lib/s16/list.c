/*
 * LibSunshine
 *
 * Implementation of linked lists.
 */

#include <assert.h>
#include <threads.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "list.h"

void * s16mem_alloc (unsigned long);
void * s16mem_calloc (size_t, unsigned long);
void s16mem_free (void *);

List_t * List_new ()
{
    List_t * new = s16mem_calloc (1, sizeof (List_t));
    if (mtx_init (&new->Lock, mtx_plain) == thrd_success)
        return new;
    else
    {
        fprintf (stderr, "mtx_init failed (list)\n");
        exit (EXIT_FAILURE);
        return 0; /* silences linter */
    }
}

void List_add (List_t * n, void * data)
{
    List_t_ *temp, *t;

    if (n == 0)
        n = List_new ();

    mtx_lock (&n->Lock);

    if (n->List == 0)
    {
        /* create new list */
        t = s16mem_alloc (sizeof (List_t_));
        t->data = data;
        t->Link = NULL;
        n->List = t;
        goto unlock;
    }
    else
    {
        t = n->List;
        temp = s16mem_alloc (sizeof (List_t_));
        while (t->Link != NULL)
            t = t->Link;
        temp->data = data;
        temp->Link = NULL;
        t->Link = temp;
    }

unlock:
    mtx_unlock (&n->Lock);
}

void List_del (List_t * n, void * data)
{
    List_t_ *current, *previous;

    previous = NULL;

    mtx_lock (&n->Lock);

    for (current = n->List; current != NULL;
         previous = current, current = current->Link)
    {
        if (current->data == data)
        {
            if (previous == NULL)
            {
                // correct the first
                n->List = current->Link;
            }
            else
            {
                // skip
                previous->Link = current->Link;
            }

            s16mem_free (current);
            goto unlock;
        }
    }
unlock:
    mtx_unlock (&n->Lock);
}

void List_destroy (List_t * n)
{
    if (!n)
        return;
    for (List_t_ *it = n->List, *tmp; it != NULL; it = tmp)
    {
        tmp = it->Link;
        s16mem_free (it);
    }
    mtx_destroy (&n->Lock);
    s16mem_free (n);
}

List_t_ * List_begin (List_t * n)
{
    if (!n)
        return 0;
    return n->List;
}

void List_iterator_next (List_t_ ** it) { *it = (*it)->Link; }

void * List_lpop (List_t * n)
{
    void * ret;
    List_t_ * tmp;

    if (n->List == NULL)
    {
        return 0;
    }
    else
    {
        mtx_lock (&n->Lock);
        ret = n->List->data;

        if (n->List->Link)
        {
            tmp = n->List->Link;
            s16mem_free (n->List);
            n->List = tmp;
        }
        else
        {
            s16mem_free (n->List);
            n->List = NULL;
        }

        mtx_unlock (&n->Lock);
        return ret;
    }
}

void * List_lget (List_t * n)
{
    if (n == NULL || n->List == NULL)
    {
        return 0;
    }
    else
    {
        return n->List->data;
    }
}

void List_print (List_t * n)
{
    List_t_ * t;

    t = n->List;

    if (t == NULL)
        printf ("Empty list\n");

    else
    {
        printf ("Begin list.\n");
        mtx_lock (&n->Lock);
        while (t != NULL)
        {
            printf ("%p\n", t->data);
            t = t->Link;
        }
        mtx_unlock (&n->Lock);
        printf ("End list.\n");
    }
    return;
}
