// Exercise 8-7. malloc accepts a size request without checking its plausibility; free believes
// that the block it is asked to free contains a valid size field. Improve these routines so they
// make more pains with error checking.

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>

#define NALLOC 1024
#define MAXALLOC (1024 * 1024 * 1024)

typedef long Align;

typedef union header
{
    struct
    {
        union header *ptr;
        unsigned size;
    } s;
    Align x;
} My_Header;

static My_Header base;
static My_Header *freep = NULL;

void my_free(void *ap)
{

    if (ap == NULL)
        return; // if the pointer input is NULL, then do nothing

    if ((uintptr_t)ap % sizeof(My_Header) != 0)
        return; // always the malloc() gives aligned storage, so if this condition fails
                // then this memory is not allocated by malloc() , so return.

    My_Header *bp, *p;
    bp = (My_Header *)ap - 1;

    if (bp->s.size == 0 || bp->s.size > MAXALLOC / sizeof(My_Header))
        return; // if size is 0 or >1GB, this memory is not allocated by malloc(), so do nothing.

    for (p = freep;; p = p->s.ptr)
    {
        if (p == bp)
            return;             // already a free block means return
        if (p->s.ptr == freep)
            break;
    }

    /* point to block header */
    for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
        if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
            break; /* freed block at start or end of arena */

    if (bp + bp->s.size == p->s.ptr)
    {
        /* join to upper nbr */
        bp->s.size += p->s.ptr->s.size;
        bp->s.ptr = p->s.ptr->s.ptr;
    }
    else
        bp->s.ptr = p->s.ptr;
    if (p + p->s.size == bp)
    {
        /* join to lower nbr */
        p->s.size += bp->s.size;
        p->s.ptr = bp->s.ptr;
    }
    else
        p->s.ptr = bp;
    freep = p;
}

static My_Header *my_morecore(unsigned nu)
{
    char *cp, *sbrk(int);
    My_Header *up;
    if (nu < NALLOC)
        nu = NALLOC;
    cp = sbrk(nu * sizeof(My_Header));
    if (cp == (char *)-1)
        return NULL;
    up = (My_Header *)cp;
    up->s.size = nu;
    my_free((void *)(up + 1));
    return freep;
}

void *my_malloc(unsigned nbytes)
{

    if (nbytes == 0)
        return NULL; // reject zero size block allocation

    if (nbytes > MAXALLOC)
        return NULL; // reject oversized request (> 1GB )

    if (nbytes > UINT_MAX - sizeof(My_Header))
        return NULL; // integer overflow check

    My_Header *p, *prevp;
    My_Header *my_morecore(unsigned);
    unsigned nunits;

    nunits = (nbytes + sizeof(My_Header) - 1) / sizeof(My_Header) + 1;
    if ((prevp = freep) == NULL)
    {
        base.s.ptr = freep = prevp = &base;
        base.s.size = 0;
    }

    for (p = prevp->s.ptr;; prevp = p, p = p->s.ptr)
    {
        if (p->s.size >= nunits)
        {                            /* big enough */
            if (p->s.size == nunits) /* exactly */
                prevp->s.ptr = p->s.ptr;
            else
            {
                /* allocate tail end */
                p->s.size -= nunits;
                p += p->s.size;
                p->s.size = nunits;
            }
            freep = prevp;
            return (void *)(p + 1);
        }
        if (p == freep) /* wrapped around free list */
            if ((p = my_morecore(nunits)) == NULL)
                return NULL;
        /* none left */
    }
}

int main()
{
    int *arr = (int *)my_malloc(10 * sizeof(int));
    for (int i = 1; i <= 10; i++)
    {
        arr[i - 1] = i+15;
    }
    my_free(arr);

    for (int i = 1; i <= 10; i++)
    {
        printf("%d\n", arr[i - 1]);
    }


    return 0;
}