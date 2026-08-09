#ifndef __TYPE_H__
#define __TYPE_H__

enum PrimitiveType
{
    Number,
    Word
};

// extendable in the future, if supporting more types, can change this
// this will trigger compile time checks
using Type = PrimitiveType;

#endif
