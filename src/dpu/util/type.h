#ifndef __TYPE_H
#define __TYPE_H

#define DEFAULT 0
#define BOOL 1	  // in C: bool
#define INT32 2  // in C: int32_t

/* HYPERSPARSE:
    If row A(i,:) has any entries, then i = Ah [k] for some
    Row A(i,:) is held in two parts: the column indices are in Ai[Ap [k]...Ap [k+1]-1],
    and the numerical values are in the same positions in Ax.
*/
#define HYPERSPARSE 1   // store matrix in hypersparse form

/* SPARSE:
    Row A(i,:) is held in two parts: the column indices are in Ai [Ap [i]...Ap [i+1]-1]
*/
#define SPARSE      2   // store matrix as sparse form (compressed vector)

#endif