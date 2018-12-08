//
//  Board.h
//  KanoodleC
//
//  Created by Andrew Ward on 12/07/2013.
//  Copyright (c) 2013 Andrew Ward. All rights reserved.
//

#ifndef KanoodleC_Board_h
#define KanoodleC_Board_h

#include "Shapes.h"


#define BOARDX 11
#define BOARDY 5
#define BOARDDIM (BOARDX * BOARDY)


#define GETBOARDROW(Board, y) ((Board) >> ((y) * BOARDX)) & ((1 << BOARDX) - 1)
#define SETBOARDROW(Board, y, Row) (Board) |= ((Row) << ((y) * BOARDX))


extern unsigned int BoardShapesCnt;
extern uint64_t *BoardShapes;
extern unsigned int BoardShapeCnt[SHAPECNT];
extern uint64_t *BoardShapeStart[SHAPECNT];

extern unsigned int ShapesAtPositionCnt[BOARDX * BOARDY];
extern unsigned int ShapesAtPositionOffset[BOARDX * BOARDY];
extern unsigned int *ShapesAtPosition;


extern void CreateBoardShapes(void);
extern void DumpBoard(uint64_t Board);


#endif
