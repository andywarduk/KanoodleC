//
//  SolutionMgr.c
//  KanoodleC
//
//  Created by Andrew Ward on 13/07/2013.
//  Copyright (c) 2013 Andrew Ward. All rights reserved.
//

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "SolutionMgr.h"
#include "Board.h"

// #define DUMPSOL

void InitSolutionMgr(struct SolutionMgr *SolutionOutput)
{
    SolutionOutput->Cnt = 0;
    SolutionOutput->File = 0;
    SolutionOutput->Output = NULL;
}

void DumpSolution(struct SolutionMgr *SolutionOutput, int *Solution)
{
    unsigned int x, y, s;
    unsigned int Board[BOARDY][BOARDX];
    uint64_t BoardShape;
    uint64_t Row;
    FILE *Output;
    char FileName[128];
    
#ifdef DUMPSOL
    unsigned int i;
    
    for(i = 0; i < SHAPECNT; i++){
        printf("%u,", Solution[i]);
    }
    printf("\n");
#endif
    
    if(SolutionOutput->Cnt == MAXSOLNS){
        fclose(SolutionOutput->Output);
        SolutionOutput->Output = NULL;
        SolutionOutput->Cnt = 0;
    };
    ++SolutionOutput->Cnt;
    
    for(s = 0; s < SHAPECNT; s++){
        BoardShape = *(BoardShapeStart[s] + Solution[s]);
        for(y = 0; y < BOARDY; y++){
            Row = GETBOARDROW(BoardShape, y);
            for(x = 0; x < BOARDX; x++){
                if(Row & (1 << x)) Board[y][x] = s;
            }
        }
    }
    
    if(SolutionOutput->Output == NULL) {
        ++SolutionOutput->File;
        sprintf(FileName, "Results-%d.html", SolutionOutput->File);
        
        Output = fopen(FileName, "w+");
        if(Output == NULL){
            printf("Failed to open output file %s\n",FileName);
            exit(1);
        }
        printf("Outputting to %s\n", FileName);
        
        SolutionOutput->Output = Output;
        
        fprintf(Output, "<!DOCTYPE html>\n");
        fprintf(Output, "<html>\n");
        fprintf(Output, "<head>\n");
        fprintf(Output, " <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n");
        fprintf(Output, " <title>Kanoodle</title>\n");
        fprintf(Output, " <link rel=\"stylesheet\" type=\"text/css\" href=\"Kanoodle.css\"/>\n");
        fprintf(Output, "</head>\n");
        fprintf(Output, "<body>\n");
        
        SolutionOutput->FPos = ftell(Output);
    }
    else{
        Output = SolutionOutput->Output;
    }
    
    fseek(Output, SolutionOutput->FPos, SEEK_SET);
    
    fprintf(Output, "<table class=\"st\">\n");
    fprintf(Output, " <tbody>\n");
    for(y = 0; y < BOARDY; y++){
        fprintf(Output, "  <tr class=\"sr\">\n");
        for(x = 0; x < BOARDX; x++){
            fprintf(Output, "   <td class=\"sc");
            if(x == 0 || Board[y][x - 1] != Board[y][x]) fprintf(Output, " bls");
            else fprintf(Output, " bld");
            if(x == BOARDX - 1 || Board[y][x + 1] != Board[y][x]) fprintf(Output, " brs");
            else fprintf(Output, " brd");
            if(y == 0 || Board[y - 1][x] != Board[y][x]) fprintf(Output, " bts");
            else fprintf(Output, " btd");
            if(y == BOARDY - 1 || Board[y + 1][x] != Board[y][x]) fprintf(Output, " bbs");
            else fprintf(Output, " bbd");
            fprintf(Output, " c%s\"</td>\n", Shapes[Board[y][x]].Name);
        }
        fprintf(Output, "  </tr>\n");
    }
    fprintf(Output, " </tbody>\n");
    fprintf(Output, "</table>\n");
    
    SolutionOutput->FPos = ftell(Output);
    
    fprintf(Output, "</body>\n");
    fprintf(Output, "</html>\n");
    
    fflush(Output);
}
