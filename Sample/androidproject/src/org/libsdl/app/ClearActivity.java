package org.libsdl.app;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.app.Activity;
import android.content.Context;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;

public class ClearActivity extends Activity {
	public ClearActivity() {
		SDLActivity.mSingleton = this;
	}
	
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mGLView = new MyGLSurfaceView(this);
        setContentView(mGLView);
    }

    @Override
    protected void onPause() {
        super.onPause();
        mGLView.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mGLView.onResume();
    }

    private GLSurfaceView mGLView;
}
class MyGLSurfaceView extends GLSurfaceView {

    public MyGLSurfaceView(Context context){
        super(context);

     // Create an OpenGL ES 2.0 context
        setEGLContextClientVersion(2);
        
        // Set the Renderer for drawing on the GLSurfaceView
        setRenderer(new ClearRenderer());
    }
}
class ClearRenderer implements GLSurfaceView.Renderer {
	private boolean ranOnce = false;
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
    	/*if (ranOnce) {
    	   	 // Set the background frame color
        	Log.v("_SDL", "Surface constructed");
        	SDLActivity.nativeInit();		
    	}*/
    	SDLActivity.nativeInit();	
    	ranOnce = true;

    }
    public int myWidth;
    public int myHeight;
    public void onSurfaceChanged(GL10 gl, int width, int height) {
    	GLES20.glViewport(0, 0, width, height);
    	myWidth = width;
    	myHeight = height;
    }

    public void onDrawFrame(GL10 gl) {
    	//GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT | GLES20.GL_DEPTH_BUFFER_BIT);
    	//Log.v("_SDL", "Imma rendering");
    	SDLActivity.nativeDrawframe(myWidth,myHeight);
    	
    }
}