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
#include "Notification.h"

using namespace std;

class Achievement{
    float currentvalue;
    float targetvalue;
    bool notified;
    string displaytext;
    Notification * interface;
    
public:

    string type;
    
    Achievement(){
        currentvalue = 0;
        targetvalue = 0;
        displaytext = "";
        type="Other";
    }
    
    Achievement(float currentvalue, float targetvalue, string displaytext, string type,bool notified,Notification * interface){
        this->currentvalue = currentvalue;
        this->targetvalue =  targetvalue;
        this->displaytext = displaytext;
        this->type = type;
        this->notified = notified;
        this->interface = interface;
    }
    
    void SetValue(float value);
    void IncrementValue(float value);
    bool IsComplete();
    float GetProgress();
};
#endif /* defined(__VoxEngine__Achievement__) */
