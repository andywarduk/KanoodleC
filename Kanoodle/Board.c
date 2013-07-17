//
//  Board.c
//  KanoodleC
//
//  Created by Andrew Ward on 12/07/2013.
//  Copyright (c) 2013 Andrew Ward. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "Board.h"
#include "Shapes.h"

// #define DEBUGBOARDPOS 1
// #define DEBUGBOARDLAYOUTS 1
// #define DUMPSHAPEPOS

uint64_t ShapeToBoardAt(uint64_t Shape, unsigned int x, unsigned int y);

unsigned int BoardShapesCnt;
uint64_t *BoardShapes;

unsigned int BoardShapeCnt[SHAPECNT];
uint64_t *BoardShapeStart[SHAPECNT];

unsigned int ShapesAtPositionCnt[BOARDX * BOARDY];
unsigned int ShapesAtPositionOffset[BOARDX * BOARDY];
unsigned int *ShapesAtPosition;

void CreateBoardShapes()
{
    unsigned int i, s;
    unsigned int Combinations;
    unsigned int ShapePosCnt;
    struct ShapeLayout *Layout;
    uint64_t Shape;
    uint64_t *OutBoardShape;
    uint64_t *InBoardShape;
    unsigned int x1, y1, x2, y2;
    uint64_t Board;
    uint64_t Row;
    unsigned int TotPos;
    unsigned int Position;
    unsigned int *OutShapesAtPosition[BOARDDIM];
#ifdef DUMPSHAPEPOS
    unsigned int Tmp, j;
#endif
    
    // Zero shapes-at-position array
    for(i = 0; i < BOARDDIM; i++) ShapesAtPositionCnt[i] = 0;

    // Work out how many
    BoardShapesCnt = 0;
    for(s = 0; s < SHAPECNT; s++){

#ifdef DEBUGBOARDPOS
        printf("Shape %u:\n", s);
#endif
        ShapePosCnt = 0;
        
        Layout = Shapes[s].FirstLayout;
        while(Layout){
            Combinations = (BOARDX - Layout->w + 1) * (BOARDY - Layout->h + 1);
#ifdef DEBUGBOARDPOS
            printf("  %ux%u -> %u\n", Layout->w, Layout->h, Combinations);
#endif
            ShapePosCnt += Combinations;
            
            Layout = Layout->NextLayout;
        }
        
        printf("Shape %u (%s) has %u board positions\n", s, Shapes[s].Name, ShapePosCnt);

        BoardShapeCnt[s] = ShapePosCnt;
        BoardShapesCnt += ShapePosCnt;
    }
#ifdef DEBUGBOARDPOS
    printf("Total: %u", BoardShapesCnt);
#endif
    
    // Allocate board shapes
    BoardShapes = (uint64_t *) malloc(BoardShapesCnt * sizeof(uint64_t));

    // Create board shapes
    OutBoardShape = BoardShapes;
    for(s = 0; s < SHAPECNT; s++){
#ifdef DEBUGBOARDLAYOUTS
        printf("Shape %u\n", s);
#endif
        BoardShapeStart[s] = OutBoardShape;
        Layout = Shapes[s].FirstLayout;
        while(Layout){
            Shape = Layout->Layout;
#ifdef DEBUGBOARDLAYOUTS
            printf("----\n");
#endif
            for(y1 = 0; y1 < (BOARDY - Layout->h + 1); y1++){
                for(x1 = 0; x1 < (BOARDX - Layout->w + 1); x1++){
                    Board = ShapeToBoardAt(Shape, x1, y1);

#ifdef DEBUGBOARDLAYOUTS
                    DumpBoard(Board);
#endif
                    
                    // Work out what position this shape is at
                    for(y2 = 0; y2 < BOARDY; y2++){
                        Row = GETBOARDROW(Board, y2);
                        if(Row != 0){
                            for(x2 = 0; x2 < BOARDX; x2++){
                                if(Row & 1){
                                    Position = x2 + (y2 * BOARDX);
                                    ShapesAtPositionCnt[Position]++;
                                    break;
                                }
                                Row >>= 1;
                            }
                            break;
                        }
                    }

                    // Save the shape
                    *OutBoardShape = Board;
                    ++OutBoardShape;
                }
            }

            Layout = Layout->NextLayout;
        }
    }

    // Build shape-at-position offset table and calculate total combinations
    TotPos = 0;
    for(i = 0; i < BOARDDIM; i++){
        ShapesAtPositionOffset[i] = TotPos;
        TotPos += ShapesAtPositionCnt[i];
    }
    
    // Allocate array for lookup table
    ShapesAtPosition = (unsigned int *) malloc(sizeof(unsigned int) * TotPos);
    
    // Set up out pointer array
    for(i = 0; i < BOARDDIM; i++){
        OutShapesAtPosition[i] = ShapesAtPosition + ShapesAtPositionOffset[i];
    }
    
    // Fill the array
    InBoardShape = BoardShapes;
    for(s = 0; s < SHAPECNT; s++){
        for(i = 0; i < BoardShapeCnt[s]; i++, ++InBoardShape){
            Board = *InBoardShape;
            
            for(y1 = 0; y1 < BOARDY; y1++){
                Row = GETBOARDROW(Board, y1);
                if(Row != 0){
                    for(x1 = 0; x1 < BOARDX; x1++){
                        if(Row & 1){
                            Position = x1 + (y1 * BOARDX);
                            *(OutShapesAtPosition[Position]) = (unsigned int)((s << 16) | (InBoardShape - BoardShapes));
                            ++OutShapesAtPosition[Position];
                            break;
                        }
                        Row >>= 1;
                    }
                    break;
                }
            }
        }
    }
    
#ifdef DUMPSHAPEPOS
    for(i = 0; i < BOARDDIM; i++){
        printf("%u shapes at position %u:\n", ShapesAtPositionCnt[i], i);
        for(j = 0; j < ShapesAtPositionCnt[i]; j++){
            Tmp = ShapesAtPosition[ShapesAtPositionOffset[i] + j];
            printf("  Shape %u, layout %u\n", Tmp >> 16, (Tmp & ((1<<16)-1)));
        }
    }
#endif
}

uint64_t ShapeToBoardAt(uint64_t Shape, unsigned int x, unsigned int y)
{
    uint64_t Board = 0;
    uint64_t Row;
    unsigned int sy;
    
    for(sy = 0; sy < MAXSEGS; sy++){
        Row = GETSHAPEROW(Shape, sy);
        Row <<= x;
        SETBOARDROW(Board, y + sy, Row);
    }

    return Board;
}

void DumpBoard(uint64_t Board)
{
    unsigned int x, y;
    uint64_t Row;
    
    printf("  %llu:\n", Board);
    for(y = 0; y < BOARDY; y++){
        Row = GETBOARDROW(Board, y);
        printf("    ");
        for(x = 0; x < BOARDX; x++){
            if(Row & (1 << x)) printf("X");
            else printf(".");
        }
        printf("\n");
    }
}