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

#define MAXSOLNS 1000

struct SolutionMgr {
    unsigned int Cnt;
    unsigned int File;
    long FPos;
    FILE *Output;
};

void InitSolutionMgr(struct SolutionMgr *SolutionOutput);
void DumpSolution(struct SolutionMgr *SolutionOutput, int *Solution);

#endif
