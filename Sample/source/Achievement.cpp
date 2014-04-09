//
//  Achievement.cpp
//  VoxEngine
//
//  Created by Matthew Bucci on 4/8/14.
//
//

#include "stdafx.h"
#include "Achievement.h"


void Achievement::SetValue(float value) {
    currentvalue=value;
}

void Achievement::IncrementValue(float value) {
    currentvalue+=value;
}

bool Achievement::IsComplete() {
    return currentvalue >= targetvalue;
}

float Achievement::GetProgress(){
    if(targetvalue != 0)
        return currentvalue / targetvalue;
    else return 0;
}