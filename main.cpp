#include <iostream>
#include "BTrack.h"

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World to ja!\n";
    //initializing with frame size 1024 and hop size 512
    BTrack b;
    
    //while() ! EOF signal
    
    //frame size 1024
    //get frame from mesqueue1
    double *frame = NULL;
    // do something here to fill the frame with audio samples
    
    
    b.processAudioFrame(frame);
    
    if (b.beatDueInCurrentFrame())
    {
        // do something on the beat
        // push 1 to mesqueue2 to signal beat
    }
    else
    {
        // push 0 to mesqueue2 to signal no beat
    }
    return 0;
}
