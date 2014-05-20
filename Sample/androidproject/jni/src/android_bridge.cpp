#include <jni.h>
#include <sys/types.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


void doFrame(int width, int height);

extern "C" 
{
    JNIEXPORT void JNICALL Java_org_libsdl_app_SDLActivity_nativeDrawframe( JNIEnv * env, jobject obj,  jint width, jint height );
}

JNIEXPORT void JNICALL Java_org_libsdl_app_SDLActivity_nativeDrawframe( JNIEnv * env, jobject obj,  jint width, jint height )
{
    doFrame( width, height );
}
