//
//  ShapeFit2.c
//  KanoodleC
//
//  Created by Andrew Ward on 17/07/2013.
//  Copyright (c) 2013 Andrew Ward. All rights reserved.
//

#include <stdio.h>
#include <stdint.h>

#include "Shapes.h"
#include "Board.h"
#include "SolutionMgr.h"

void FitShapeRec(uint64_t Board, unsigned int Position, int *Solution, unsigned int Depth, struct SolutionMgr *SolutionOutput);

void FitShapes(struct SolutionMgr *SolutionOutput)
{
    int Solution[SHAPECNT];
    unsigned int i;
    
    for(i = 0; i < SHAPECNT; i++) Solution[i] = -1;
    
    FitShapeRec(0, 0, Solution, 0, SolutionOutput);
}

void FitShapeRec(uint64_t Board, unsigned int Position, int *Solution, unsigned int Depth, struct SolutionMgr *SolutionOutput)
{
    uint64_t TmpBoard;
    unsigned int ShapeCnt;
    unsigned int *ShapesAtPos;
    unsigned int i;
    unsigned int Shape;
    unsigned int BoardShapeOffset;
    uint64_t BoardShape;
    
    // Work out next empty position
    TmpBoard = Board;
    TmpBoard >>= Position;
    while((TmpBoard & 1) == 1){
        ++Position;
        TmpBoard >>= 1;
    }
    
    // Get count of shapes at this position
    ShapeCnt = ShapesAtPositionCnt[Position];
    
    ShapesAtPos = &ShapesAtPosition[ShapesAtPositionOffset[Position]];
    for(i = ShapeCnt; i > 0; i--, ++ShapesAtPos) {
        Shape = *ShapesAtPos >> 16;
        BoardShapeOffset = *ShapesAtPos & ((1 << 16) - 1);
        
        // Has this shape already been placed?
        if(Solution[Shape] == -1){
            // No - does the shape fit on the board?
            BoardShape = BoardShapes[BoardShapeOffset];
            
            if((Board & BoardShape) == 0){
                // Shape fits on the board
                Solution[Shape] = BoardShapeOffset - ((unsigned int) (BoardShapeStart[Shape] - BoardShapes));

                if(Depth == SHAPECNT - 1){
                    // Got a solution
                    DumpSolution(SolutionOutput, Solution);
                }
                else{
                    // Recurse
                    FitShapeRec(Board | BoardShape, Position, Solution, Depth + 1, SolutionOutput);
                }
                
                Solution[Shape] = -1;
            }
        }
    }
}

