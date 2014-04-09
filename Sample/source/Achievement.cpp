//
//  Achievement.cpp
//  VoxEngine
//
//  Created by Matthew Bucci on 4/8/14.
//
//
#include "stdafx.h"
#include <sstream>

#include "Achievement.h"


void Achievement::SetValue(float value) {
    currentvalue=value;
    if(IsComplete() && !notified){
        stringstream output;
        output << "Achievement Unlocked: " << displaytext;
        interface->notify(output.str());
        notified = true;
    }
}

void Achievement::IncrementValue(float value) {
    currentvalue+=value;
    if(IsComplete() && !notified) {
        stringstream output;
        output << "Achievement Unlocked: " << displaytext;
        interface->notify(output.str());
        notified = true;
    }
}

bool Achievement::IsComplete() {
    return currentvalue >= targetvalue;
}

float Achievement::GetProgress(){
    if(targetvalue != 0)
        return currentvalue / targetvalue;
    else return 0;
}