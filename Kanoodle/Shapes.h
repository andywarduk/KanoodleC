//
//  Shapes.h
//  KanoodleC
//
//  Created by Andrew Ward on 12/07/2013.
//  Copyright (c) 2013 Andrew Ward. All rights reserved.
//

#ifndef KanoodleC_Shapes_h
#define KanoodleC_Shapes_h

#include <stdint.h>

#define SHAPECNT 12
#define MAXSEGS 5

void InitShapes();

struct ShapeLayout {
    uint64_t Layout;
    struct ShapeLayout *NextLayout;
    unsigned int w;
    unsigned int h;
};

struct ShapeDef {
    char *Name;
    unsigned int Segments;
    unsigned int Layout[MAXSEGS][2];
    uint64_t PackedLayout;
    unsigned int LayoutCnt;
    struct ShapeLayout *FirstLayout;
};

extern struct ShapeDef Shapes[SHAPECNT];

#define GETSHAPEROW(Shape, y) ((Shape) >> ((y) * MAXSEGS)) & ((1 << MAXSEGS) - 1)
#define SETSHAPEROW(Shape, y, Row) (Shape) |= ((Row) << ((y) * MAXSEGS))

#define SETSHAPEPOS(Shape, x, y) (Shape) |= (1 << (x)) << ((y) * MAXSEGS);

#endif
