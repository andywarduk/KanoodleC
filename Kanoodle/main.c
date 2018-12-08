//
//  main.c
//  KanoodleC
//
//  Created by Andrew Ward on 12/07/2013.
//  Copyright (c) 2013 Andrew Ward. All rights reserved.
//

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <getopt.h>

#include "Shapes.h"
#include "Board.h"
#include "SolutionMgr.h"
#include "ShapeFit.h"

int main()
{
    struct SolutionMgr SolutionOutput;

    InitShapes();
    
    CreateBoardShapes();
    
    InitSolutionMgr(&SolutionOutput);

    FitShapes(&SolutionOutput);
    
    FiniSolutionMgr(&SolutionOutput);
    
    return 0;
}

