/* It is possible that malloc() can fail when memory is exhausted.
 * We can recover a system from the inevitable crash this causes,
 * but we don't want to have to do that unless we *really* need to.
 * As such, herein is a simple memory manager that uses a 64 kilobyte
 * statically-allocated buffer, and may be initialised with a callback
 * function to notify the app of the failure.
 *
 * This code is adapted from memmgr by Eli Bendersky, who had released
 * it into the public domain */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "s16.h"

/* a linked list of memory headers */
union mem_header_union
{
    struct
    {
        union mem_header_union * next;
        unsigned long size;
    } s;

    unsigned long align_dummy;
};

typedef union mem_header_union mem_header_t;

// Initial empty list
//
static mem_header_t base;

// Start of free list
//
static mem_header_t * freep = 0;

// Static pool for new allocations
//
static unsigned char pool[POOL_SIZE] = {0};
static unsigned long pool_free_pos = 0;

void s16mem_init ()
{
    base.s.next = 0;
    base.s.size = 0;
    freep = 0;
    pool_free_pos = 0;
}

static mem_header_t * get_mem_from_pool (unsigned long nquantas)
{
    unsigned long total_req_size;

    mem_header_t * h;

    if (nquantas < MIN_POOL_ALLOC_QUANTAS)
        nquantas = MIN_POOL_ALLOC_QUANTAS;

    total_req_size = nquantas * sizeof (mem_header_t);

    if (pool_free_pos + total_req_size <= POOL_SIZE)
    {
        h = (mem_header_t *)(pool + pool_free_pos);
        h->s.size = nquantas;
        s16mem_free ((void *)(h + 1));
        pool_free_pos += total_req_size;
    }
    else
    {
        return 0;
    }

    return freep;
}

// Allocations are done in 'quantas' of header size.
// The search for a free block of adequate size begins at the point 'freep'
// where the last block was found.
// If a too-big block is found, it is split and the tail is returned (this
// way the header of the original needs only to have its size adjusted).
// The pointer returned to the user points to the free space within the block,
// which begins one quanta after the header.
//
void * s16mem_alloc (unsigned long nbytes)
{
    void * m;
    mem_header_t * p;
    mem_header_t * prevp;

    /* We first try to use the system malloc. */
    if ((m = malloc (nbytes)))
        return m;

    // Calculate how many quantas are required: we need enough to house all
    // the requested bytes, plus the header. The -1 and +1 are there to make
    // sure
    // that if nbytes is a multiple of nquantas, we don't allocate too much
    //
    unsigned long nquantas =
        (nbytes + sizeof (mem_header_t) - 1) / sizeof (mem_header_t) + 1;

    // First alloc call, and no free list yet ? Use 'base' for an initial
    // denegerate block of size 0, which points to itself
    //
    if ((prevp = freep) == 0)
    {
        base.s.next = freep = prevp = &base;
        base.s.size = 0;
    }

    for (p = prevp->s.next;; prevp = p, p = p->s.next)
    {
        // big enough ?
        if (p->s.size >= nquantas)
        {
            // exactly ?
            if (p->s.size == nquantas)
            {
                // just eliminate this block from the free list by pointing
                // its prev's next to its next
                //
                prevp->s.next = p->s.next;
            }
            else // too big
            {
                p->s.size -= nquantas;
                p += p->s.size;
                p->s.size = nquantas;
            }

            freep = prevp;
            return (void *)(p + 1);
        }
        // Reached end of free list ?
        // Try to allocate the block from the pool. If that succeeds,
        // get_mem_from_pool adds the new block to the free list and
        // it will be found in the following iterations.
        else if (p == freep)
        {
            if ((p = get_mem_from_pool (nquantas)) == 0)
            {
                /* now we really are in trouble. this should NEVER happen. */
                exit (1);
                fprintf (stderr, "!! Memory allocation failed !!\n");
                return 0;
            }
        }
    }
}

void * s16mem_calloc (size_t cnt, unsigned long nbytes)
{
    void * ptr = s16mem_alloc (cnt * nbytes);
    return memset (ptr, '\0', cnt * nbytes);
}

char * s16mem_strdup (const char * str)
{
    size_t len = strlen (str) + 1;
    void * ptr = s16mem_alloc (len);
    return (char *)memcpy (ptr, str, len);
}

// Scans the free list, starting at freep, looking the the place to insert the
// free block. This is either between two existing blocks or at the end of the
// list. In any case, if the block being freed is adjacent to either neighbor,
// the adjacent blocks are combined.
void s16mem_free (void * ap)
{
    mem_header_t * block;
    mem_header_t * p;

    // acquire pointer to block header
    block = ((mem_header_t *)ap) - 1;

    if (ap < (void *)&pool || ap > (void *)(&pool + POOL_SIZE))
    {
        /* We managed to use system malloc in this case.
         * That, or someone doesn't know what they are doing. */
        free (ap);
        return;
    }

    // Find the correct place to place the block in (the free list is sorted by
    // address, increasing order)
    for (p = freep; !(block > p && block < p->s.next); p = p->s.next)
    {
        // Since the free list is circular, there is one link where a
        // higher-addressed block points to a lower-addressed block.
        // This condition checks if the block should be actually
        // inserted between them
        if (p >= p->s.next && (block > p || block < p->s.next))
            break;
    }

    // Try to combine with the higher neighbor
    if (block + block->s.size == p->s.next)
    {
        block->s.size += p->s.next->s.size;
        block->s.next = p->s.next->s.next;
    }
    else
    {
        block->s.next = p->s.next;
    }

    // Try to combine with the lower neighbor
    if (p + p->s.size == block)
    {
        p->s.size += block->s.size;
        p->s.next = block->s.next;
    }
    else
    {
        p->s.next = block;
    }

    freep = p;
}