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

void StartHtmlFile(FILE *Output, char *Title);
void EndHtmlFile(FILE *Output);

void InitSolutionMgr(struct SolutionMgr *SolutionOutput)
{
    FILE *Contents;
    
    SolutionOutput->TotCnt = 0;
    SolutionOutput->Cnt = 0;
    SolutionOutput->File = 0;
    SolutionOutput->Output = NULL;
    SolutionOutput->SolPerFile = 1000;
    
    Contents = fopen("Results-0.html", "w+");
    if(Contents == NULL){
        printf("Error opening contents file\n");
        exit(1);
    }
    SolutionOutput->Contents = Contents;    

    StartHtmlFile(Contents, "Results Contents");
    SolutionOutput->CntFPos = ftell(Contents);
    EndHtmlFile(Contents);
}

void FiniSolutionMgr(struct SolutionMgr *SolutionOutput)
{
    if(SolutionOutput->Output != NULL){
        fclose(SolutionOutput->Output);
    }
    
    if(SolutionOutput->Contents != NULL){
        fclose(SolutionOutput->Contents);
    }
}

void StartHtmlFile(FILE *Output, char *Title)
{
    // Output html header
    fprintf(Output, "<!DOCTYPE html>\n");
    fprintf(Output, "<html>\n");
    fprintf(Output, "<head>\n");
    fprintf(Output, " <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n");
    fprintf(Output, " <title>%s</title>\n", Title);
    fprintf(Output, " <link rel=\"stylesheet\" type=\"text/css\" href=\"Kanoodle.css\"/>\n");
    fprintf(Output, "</head>\n");
    fprintf(Output, "<body>\n");    
}

void EndHtmlFile(FILE *Output)
{
    fprintf(Output, "</body>\n");
    fprintf(Output, "</html>\n");
}

void DumpSolutionToFile(FILE *Output, int *Solution)
{
    unsigned int s, x, y;
    int Board[BOARDY][BOARDX];
    uint64_t BoardShape;
    uint64_t Row;

    // Initialise the output board
    for(y = 0; y < BOARDY; y++){
        for(x = 0; x < BOARDX; x++){
            Board[y][x] = -1;
        }
    }

    // Build the output board
    for(s = 0; s < SHAPECNT; s++){
        if(Solution[s] >= 0){
            BoardShape = *(BoardShapeStart[s] + Solution[s]);
            for(y = 0; y < BOARDY; y++){
                Row = GETBOARDROW(BoardShape, y);
                for(x = 0; x < BOARDX; x++){
                    if(Row & (1 << x)) Board[y][x] = s;
                }
            }
        }
    }

    // Output the solution
    fprintf(Output, "<table class=\"st\">\n");
    fprintf(Output, " <tbody>\n");
    for(y = 0; y < BOARDY; y++){
        fprintf(Output, "  <tr class=\"sr\">\n");
        for(x = 0; x < BOARDX; x++){
            fprintf(Output, "   <td class=\"sc");
            
            // Output line styles
            if(x == 0 || Board[y][x - 1] != Board[y][x]) fprintf(Output, " bls");
            else fprintf(Output, " bld");
            if(x == BOARDX - 1 || Board[y][x + 1] != Board[y][x]) fprintf(Output, " brs");
            else fprintf(Output, " brd");
            if(y == 0 || Board[y - 1][x] != Board[y][x]) fprintf(Output, " bts");
            else fprintf(Output, " btd");
            if(y == BOARDY - 1 || Board[y + 1][x] != Board[y][x]) fprintf(Output, " bbs");
            else fprintf(Output, " bbd");
            
            // Output shape style
            if(Board[y][y] == -1) fprintf(Output, " none\"</td>\n");
            else fprintf(Output, " c%s\"</td>\n", Shapes[Board[y][x]].Name);
        }
        fprintf(Output, "  </tr>\n");
    }
    fprintf(Output, " </tbody>\n");
    fprintf(Output, "</table>\n");
}

void DumpSolution(struct SolutionMgr *SolutionOutput, int *Solution)
{
    FILE *Output, *Contents;
    char FileName[128];
    char Title[128];
    
#ifdef DUMPSOL
    unsigned int i;
    
    for(i = 0; i < SHAPECNT; i++){
        printf("%u,", Solution[i]);
    }
    printf("\n");
#endif
    
    // Close the file if we have reached maximum solutions per file
    if(SolutionOutput->Cnt == SolutionOutput->SolPerFile){
        fclose(SolutionOutput->Output);
        SolutionOutput->Output = NULL;
        SolutionOutput->Cnt = 0;
    };
    
    ++SolutionOutput->TotCnt;
    ++SolutionOutput->Cnt;
        
    // Make sure we have an output file
    if(SolutionOutput->Output == NULL) {
        // No - create the next one
        ++SolutionOutput->File;
        sprintf(FileName, "Results-%d.html", SolutionOutput->File);
        
        Output = fopen(FileName, "w+");
        if(Output == NULL){
            printf("Failed to open output file %s\n",FileName);
            exit(1);
        }
        printf("Outputting to %s\n", FileName);
        
        SolutionOutput->Output = Output;
        
        // Output html header
        sprintf(Title, "Kanoodle Results Page %d", SolutionOutput->File);
        StartHtmlFile(Output, Title);
        
        // Save the file position
        SolutionOutput->FPos = ftell(Output);

        // Add this solution to the contents
        Contents = SolutionOutput->Contents;
        
        // Seek to last end location
        fseek(Contents, SolutionOutput->CntFPos, SEEK_SET);
        
        // Start link
        fprintf(Contents, "<a href=\"Results-%d.html\">", SolutionOutput->File);
        
        // Write the solution
        DumpSolutionToFile(Contents, Solution);
        
        // End link
        fprintf(Contents, "</a>");

        // Save current file position for next solution
        SolutionOutput->CntFPos = ftell(Contents);
        
        // Save a sane trailer so file is likely readable after being interrupted
        EndHtmlFile(Contents);
        
        // Flush the output   
        fflush(Contents);
    }
    else{
        // Got one already - use it
        Output = SolutionOutput->Output;
    }
    
    // Seek to last end location
    fseek(Output, SolutionOutput->FPos, SEEK_SET);

    // Write the solution
    DumpSolutionToFile(Output, Solution);
 
    // Save current file position for next solution
    SolutionOutput->FPos = ftell(Output);
    
    // Save a sane trailer so file is likely readable after being interrupted
    EndHtmlFile(Output);
    
    // Flush the output   
    fflush(Output);
}
