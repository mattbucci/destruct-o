package org.libsdl.app;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.app.Activity;
import android.content.Context;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;

public class ClearActivity extends Activity {
	public ClearActivity() {
		SDLActivity.mSingleton = this;
	}
	
	@Override
	public void onBackPressed() {
		//We only care about the back button
		//button codes listed on line 42 of SDL_androidkeyboard.c
		SDLActivity.onNativeKeyDown(4);
		SDLActivity.onNativeKeyUp(4);
	}
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mGLView = new MyGLSurfaceView(this);
        setContentView(mGLView);
    }
 // Events
    @Override
    protected void onPause() {
        Log.v("SDL", "onPause()");
        super.onPause();
        SDLActivity.handlePause();
        mGLView.onPause();
    }

    @Override
    protected void onResume() {
        Log.v("SDL", "onResume()");
        super.onResume();
        SDLActivity.handleResume();
        mGLView.onResume();
    }

/*
    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        Log.v("SDL", "onWindowFocusChanged(): " + hasFocus);

        SDLActivity.mHasFocus = hasFocus;
        if (hasFocus) {
            SDLActivity.handleResume();
        }
    }
    */
    private GLSurfaceView mGLView;
}
class MyGLSurfaceView extends GLSurfaceView implements View.OnTouchListener {
    public MyGLSurfaceView(Context context){
        super(context);
        
        //Handle touch events
        setOnTouchListener(this); 
        
        //Set appropriate settings to handle touch events
        setFocusable(true);
        setFocusableInTouchMode(true);
        requestFocus();
        
        // Create an OpenGL ES 2.0 context
        setEGLContextClientVersion(2);
        
        //Preserve the context as best as possible
        setPreserveEGLContextOnPause(true);
        
        // Set the Renderer for drawing on the GLSurfaceView
        setRenderer(new ClearRenderer(this.getWidth(),this.getHeight()));
    }

	@Override
	public boolean onTouch(View v, MotionEvent event) {
        final int touchDevId = event.getDeviceId();
        final int pointerCount = event.getPointerCount();
        // touchId, pointerId, action, x, y, pressure
        int actionPointerIndex = (event.getAction() & MotionEvent.ACTION_POINTER_ID_MASK) >> MotionEvent.ACTION_POINTER_ID_SHIFT; /* API 8: event.getActionIndex(); */
        int pointerFingerId = event.getPointerId(actionPointerIndex);
        int action = (event.getAction() & MotionEvent.ACTION_MASK); /* API 8: event.getActionMasked(); */

        float x = event.getX(actionPointerIndex) / (float)this.getWidth();
        float y = event.getY(actionPointerIndex) / (float)this.getHeight();
        float p = event.getPressure(actionPointerIndex);

        if (action == MotionEvent.ACTION_MOVE && pointerCount > 1) {
           // TODO send motion to every pointer if its position has
           // changed since prev event.
           for (int i = 0; i < pointerCount; i++) {
               pointerFingerId = event.getPointerId(i);
               x = event.getX(i) / (float)this.getWidth();
               y = event.getY(i) / (float)this.getHeight();
               p = event.getPressure(i);
               SDLActivity.onNativeTouch(touchDevId, pointerFingerId, action, x, y, p);
           }
        } else {
           SDLActivity.onNativeTouch(touchDevId, pointerFingerId, action, x, y, p);
        }
        return true;
	}
}
class ClearRenderer implements GLSurfaceView.Renderer {
	private boolean ranOnce = false;
	
	public ClearRenderer(int startWidth, int startHeight) {
		myWidth = startWidth;
		myHeight = startHeight;
	}
	
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
    	Log.v("_SDL", "onSurfaceCreated");
    	if (ranOnce) {
    		//Right now there's no way to reinitialize the opengl context without trashing global state
    		//so we're going to have to restart the app entirely and then do something fancy to try and bring
    		//the user exactly back to where they were
    		Log.v("_SDL", "Attempting to app restart, this will probably not work");
    	}
    	ranOnce = false;

    }
    public int myWidth;
    public int myHeight;
    public void onSurfaceChanged(GL10 gl, int width, int height) {
    	//Check if the app has started up, if not start it up with the correct size
    	if (!ranOnce) {
	   	   	 // Set the background frame color
	       	Log.v("_SDL", "Surface constructed with size[" + width + "," + height + "]");
	       	SDLActivity.nativeInit(width,height);	
	       	ranOnce = true;
    	}
    	
    	
    	Log.v("_SDL", "onSurfaceChanged [" + width + "," + height + "]");
    	myWidth = width;
    	myHeight = height;
    	SDLActivity.onNativeResize(width, height, 0x16462004);
    }

    public void onDrawFrame(GL10 gl) {
    	//GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT | GLES20.GL_DEPTH_BUFFER_BIT);
    	//Log.v("_SDL", "Imma rendering");
    	SDLActivity.nativeDrawframe(myWidth,myHeight);
    	
    }
}