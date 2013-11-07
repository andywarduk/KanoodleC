//
//  SolutionMgr.h
//  KanoodleC
//
//  Created by Andrew Ward on 13/07/2013.
//  Copyright (c) 2013 Andrew Ward. All rights reserved.
//

#ifndef KanoodleC_SolutionMgr_h
#define KanoodleC_SolutionMgr_h

#include <stdio.h>

struct SolutionMgr {
    unsigned int TotCnt;
    unsigned int Cnt;
    unsigned int File;
    unsigned int SolPerFile;

    FILE *Output;
    long FPos;

    FILE *Contents;
    long CntFPos;
};

void InitSolutionMgr(struct SolutionMgr *SolutionOutput);
void FiniSolutionMgr(struct SolutionMgr *SolutionOutput);
void DumpSolution(struct SolutionMgr *SolutionOutput, int *Solution);

#endif
