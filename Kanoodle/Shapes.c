//
//  Shapes.c
//  KanoodleC
//
//  Created by Andrew Ward on 12/07/2013.
//  Copyright (c) 2013 Andrew Ward. All rights reserved.
//

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>

#include "Shapes.h"

//#define DEBUGTRANSFORMS 1

uint64_t PackLayout(unsigned int Segments, unsigned int Layout[][2]);
uint64_t ShiftShape(uint64_t ShapeIn);
uint64_t FlipShapeX(uint64_t ShapeIn);
uint64_t FlipShapeY(uint64_t ShapeIn);
uint64_t RotateShape(uint64_t ShapeIn);
unsigned int ShapeWidth(uint64_t Shape);
unsigned int ShapeHeight(uint64_t Shape);
void AddLayout(struct ShapeDef *Shape, uint64_t NewShape);
void DumpLayouts(struct ShapeDef *Shape);
void DumpShape(uint64_t Shape);

struct ShapeDef Shapes[SHAPECNT] = {
    {
        .Name = "S",
        .Segments = 4,
        .Layout = {
            {1,1},{2,1},
            {1,2},{2,2},
        }},
    {
        .Name = "X",
        .Segments = 5,
        .Layout = {
            {2,1},
            {1,2},{2,2},{3,2},
            {2,3}
        }},
    {
        .Name = "W",
        .Segments = 5,
        .Layout = {
            {1,1},
            {1,2},{2,2},
            {2,3},{3,3}
        }},
    {
        .Name = "V",
        .Segments = 5,
        .Layout = {
            {1,1},
            {1,2},
            {1,3},{2,3},{3,3}
        }},
    {
        .Name = "U",
        .Segments = 5,
        .Layout = {
            {1,1},      {3,1},
            {1,2},{2,2},{3,2}
        }},
    {
        .Name = "i",
        .Segments = 3,
        .Layout = {
            {1,1},
            {1,2},{2,2}
        }},
    {
        .Name = "N",
        .Segments = 5,
        .Layout = {
            {2,1},
            {2,2},
            {1,3},{2,3},
            {1,4}
        }},
    {
        .Name = "L",
        .Segments = 5,
        .Layout = {
            {1,1},
            {1,2},
            {1,3},
            {1,4},{2,4}
        }},
    {
        .Name = "I",
        .Segments = 4,
        .Layout = {
            {1,1},
            {1,2},
            {1,3},
            {1,4},
        }},
    {
        .Name = "Y",
        .Segments = 5,
        .Layout = {
            {1,1},
            {1,2},
            {1,3},{2,3},
            {1,4}
        }},
    {
        .Name = "P",
        .Segments = 5,
        .Layout = {
            {1,1},{2,1},
            {1,2},{2,2},
            {1,3}
        }},
    {
        .Name = "l",
        .Segments = 4,
        .Layout = {
            {1,1},
            {1,2},
            {1,3},{2,3}
        }},
};

void InitShapes()
{
    int i, j, k;
    struct ShapeDef *Shape;
    uint64_t CurShape;
    
    for(i = 0; i < SHAPECNT; i++){
        Shape = &Shapes[i];
        printf("Shape %d (%s):\n", i, Shape->Name);
        Shape->PackedLayout = PackLayout(Shape->Segments, Shape->Layout);
        Shape->LayoutCnt = 0;
        Shape->FirstLayout = NULL;
        
        for(k = 0; k < 3; k++){
            CurShape = Shape->PackedLayout;
            
            switch(k){
                case 0:
                    CurShape = ShiftShape(CurShape);
#ifdef DEBUGTRANSFORMS
                    printf("After ShiftShape\n");
                    DumpShape(CurShape);
#endif
                    break;
                case 1:
                    CurShape = FlipShapeX(CurShape);
#ifdef DEBUGTRANSFORMS
                    printf("After FlipShapeX\n");
                    DumpShape(CurShape);
#endif
                    break;
                case 2:
                    CurShape = FlipShapeY(CurShape);
#ifdef DEBUGTRANSFORMS
                    printf("After FlipShapeY\n");
                    DumpShape(CurShape);
#endif
                    break;
            }
            for(j = 0; j < 4; j++){
                AddLayout(Shape, CurShape);
                CurShape = RotateShape(CurShape);
#ifdef DEBUGTRANSFORMS
                printf("After RotateShape\n");
                DumpShape(CurShape);
#endif
            }
        }
        
        DumpLayouts(Shape);
    }
}

int DuplicateLayout(struct ShapeDef *Shape, uint64_t ChkShape)
{
    int Duplicate = 0;
    struct ShapeLayout *Layout;
    
    Layout = Shape->FirstLayout;
    while(Layout){
        if(Layout->Layout == ChkShape){
            Duplicate = 1;
            break;
        }
        Layout = Layout->NextLayout;
    }
    
    return Duplicate;
}

void AddLayout(struct ShapeDef *Shape, uint64_t NewShape)
{
    int Duplicate = 0;
    struct ShapeLayout *LastLayout = NULL;
    struct ShapeLayout *Layout;
    struct ShapeLayout *NewLayout;
    
    Layout = Shape->FirstLayout;
    while(Layout){
        if(Layout->Layout == NewShape){
            Duplicate = 1;
            break;
        }
        LastLayout = Layout;
        Layout = Layout->NextLayout;
    }
    
    if(!Duplicate){
        NewLayout = (struct ShapeLayout *) malloc(sizeof(struct ShapeLayout));
        
        NewLayout->Layout = NewShape;
        NewLayout->NextLayout = NULL;
        NewLayout->w = ShapeWidth(NewShape);
        NewLayout->h = ShapeHeight(NewShape);
        
        if(LastLayout == NULL) Shape->FirstLayout = NewLayout;
        else LastLayout->NextLayout = NewLayout;
        ++Shape->LayoutCnt;
    }
}

uint64_t ShiftShape(uint64_t ShapeIn)
{
    uint64_t ShapeOut = ShapeIn;
    unsigned int y;
    int ZeroFilled;
    uint64_t Row;
    
    // Shift X
    while(ShapeIn != 0){
        ZeroFilled = 0;
        for(y = 0; y < MAXSEGS; y++){
            // Get row
            Row = GETSHAPEROW(ShapeIn, y);
            if(Row & 1){
                ZeroFilled = 1;
                break;
            }
        }
        if(ZeroFilled) break;
        
        // Do the shift
        ShapeOut = 0;
        for(y = 0; y < MAXSEGS; y++){
            // Get row
            Row = GETSHAPEROW(ShapeIn, y);
            
            // Shift it
            Row >>= 1;
            
            // Pack row in to result
            SETSHAPEROW(ShapeOut, y, Row);
        }
        ShapeIn = ShapeOut;
    }
    
    // Shift Y
    while(ShapeIn != 0 && (ShapeIn & ((1 << MAXSEGS) - 1)) == 0){
        // Do the shift
        ShapeOut = 0;
        for(y = 1; y < MAXSEGS; y++){
            // Get row
            Row = GETSHAPEROW(ShapeIn, y);
            
            // Pack row in to result
            SETSHAPEROW(ShapeOut, y - 1, Row);
        }
        ShapeIn = ShapeOut;
    }
    
    return ShapeOut;
}

uint64_t FlipShapeX(uint64_t ShapeIn)
{
    uint64_t ShapeOut = 0;
    unsigned int x, y;
    uint64_t RowIn;
    uint64_t RowOut;
    
    for(y = 0; y < MAXSEGS; y++){
        // Get row
        RowIn = GETSHAPEROW(ShapeIn, y);
        
        // Flip it
        RowOut = 0;
        for(x = 0; x < MAXSEGS; x++){
            RowOut <<= 1;
            if(RowIn & 1) RowOut |= 1;
            RowIn >>= 1;
        }
        
        // Pack row in to result
        SETSHAPEROW(ShapeOut, y, RowOut);
    }
    
    return ShiftShape(ShapeOut);
}

uint64_t FlipShapeY(uint64_t ShapeIn)
{
    uint64_t ShapeOut = 0;
    unsigned int y;
    uint64_t Row;
    
    for(y = 0; y < MAXSEGS; y++){
        // Get row
        Row = GETSHAPEROW(ShapeIn, y);
        
        // Pack row in to result
        SETSHAPEROW(ShapeOut, MAXSEGS - (y + 1), Row);
    }
    
    return ShiftShape(ShapeOut);
}

uint64_t RotateShape(uint64_t ShapeIn)
{
    unsigned int x, y;
    uint64_t Row;
    uint64_t ShapeOut = 0;
    
    for(y = 0; y < MAXSEGS; y++){
        Row = GETSHAPEROW(ShapeIn, y);
        for(x = 0; x < MAXSEGS; x++){
            if(Row & (1 << x)) SETSHAPEPOS(ShapeOut, MAXSEGS - 1 - y, x);
        }
    }
    
    return ShiftShape(ShapeOut);
}

unsigned int ShapeWidth(uint64_t Shape)
{
    unsigned int Width = 0;
    unsigned int x, y;
    uint64_t Row;
    
    for(y = 0; y < MAXSEGS; y++){
        Row = GETSHAPEROW(Shape, y);
        for(x = Width; x < MAXSEGS; x++){
            if(Row & (1 << x)) Width = x + 1;
        }
    }
    
    return Width;
}

unsigned int ShapeHeight(uint64_t Shape)
{
    unsigned int Height = 0;
    unsigned int y;
    uint64_t Row;
    
    for(y = 0; y < MAXSEGS; y++){
        Row = GETSHAPEROW(Shape, y);
        if(Row == 0) break;
        ++Height;
    }
    
    return Height;
}

uint64_t PackLayout(unsigned int Segments, unsigned int Layout[][2])
{
    unsigned int i;
    unsigned int x, y;
    uint64_t Result = 0;
    
    for(i = 0; i < Segments; i++){
        x = Layout[i][0];
        y = Layout[i][1];
        SETSHAPEPOS(Result, x - 1, y - 1);
    }
    
    return Result;
}

void DumpLayouts(struct ShapeDef *Shape)
{
    unsigned int x, y;
    uint64_t Row;
    struct ShapeLayout *Layout;
    
    printf("  %u layouts:\n", Shape->LayoutCnt);
    for(y = 0; y < MAXSEGS; y++){
        printf("    ");
        
        Layout = Shape->FirstLayout;
        while(Layout){
            Row = GETSHAPEROW(Layout->Layout, y);
            for(x = 0; x < MAXSEGS; x++){
                if(Row & (1 << x)) printf("X");
                else printf(".");
            }
            printf(" ");
            Layout = Layout->NextLayout;
        }
        printf("\n");
    }
    
    printf("    ");
    Layout = Shape->FirstLayout;
    while(Layout){
        printf("%dx%d   ", Layout->w, Layout->h);
        Layout = Layout->NextLayout;
    }
    printf("\n");
}

void DumpShape(uint64_t Shape)
{
    unsigned int x, y;
    uint64_t Row;
    
    printf("  %" PRIu64 ":\n", Shape);
    for(y = 0; y < MAXSEGS; y++){
        Row = GETSHAPEROW(Shape, y);
        printf("    ");
        for(x = 0; x < MAXSEGS; x++){
            if(Row & (1 << x)) printf("X");
            else printf(".");
        }
        printf("\n");
    }
    
}
