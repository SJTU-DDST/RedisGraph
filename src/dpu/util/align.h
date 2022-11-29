#ifndef __ALIGN_H
#define __ALIGN_H

size_t align8(size_t s)
{
    if ((s & 0x7) == 0)
        return s;
    return ((s >> 3) + 1) << 3;
}

#endif