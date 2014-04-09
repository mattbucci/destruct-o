//
//  Achievement.h
//  VoxEngine
//
//  Created by Matthew Bucci on 4/8/14.
//
//

#ifndef __VoxEngine__Achievement__
#define __VoxEngine__Achievement__

#include <iostream>
#include <string>

using namespace std;

class Achievement{
    float currentvalue;
    float targetvalue;
    string displaytext;
    string type;
    
public:
    
    Achievement(){
        currentvalue = 0;
        targetvalue = 0;
        displaytext = "";
        type="Other";
    }
    
    Achievement(float currentvalue, float targetvalue, string displaytext, string type){
        this->currentvalue = currentvalue;
        this->targetvalue =  targetvalue;
        this->displaytext = displaytext;
        this->type = type;
    }
    
    void SetValue(float value);
    void IncrementValue(float value);
    bool IsComplete();
    float GetProgress();
};
#endif /* defined(__VoxEngine__Achievement__) */
