#include "stdafx.h"
#include "DemoWindow.h"
#include "Demo.h"

DemoWindow::DemoWindow(Demo * d) {
	//Size yourself
	position = Rect(0,20,105,580);
	hPin = Control::MAX;
	//Add all the buttons in the appropriate places
	//Center all the buttons vertically
	makePyramid.hPin = fireBlock.hPin = makeFire.hPin = increaseAcid.hPin = decreaseAcid.hPin = makeMesh.hPin = Control::CENTER;
	makePyramid.vPin = fireBlock.vPin = makeFire.vPin = increaseAcid.vPin = decreaseAcid.vPin = makeMesh.vPin = Control::MIN;
	//Now place each at a different horizontal position
	makePyramid.position =	Rect(0,10+95*0,85,85);
	fireBlock.position =	Rect(0,10+95*1,85,85);
	makeFire.position =		Rect(0,10+95*2,85,85);
	increaseAcid.position = Rect(0,10+95*3,85,85);
	decreaseAcid.position = Rect(0,10+95*4,85,85);
	makeMesh.position =     Rect(0,10+95*5,85,85);
	//Label each button
	makePyramid.SetText("Pyramid");
	fireBlock.SetText("Launch");
	makeFire.SetText("Fire");
	increaseAcid.SetText("Dose");
	decreaseAcid.SetText("Undose");
    makeMesh.SetText("Mesh");
    
	//Add each button to the window
	AddControl(&makePyramid);
	AddControl(&fireBlock);
	AddControl(&makeFire);
	AddControl(&increaseAcid);
	AddControl(&decreaseAcid);
    AddControl(&makeMesh);
    
	//Now subscribe to each button
	Subscribe<void(Button*)>(&makePyramid.EventClicked,[d](Button * b) {
		d->QueueTouchEvent('3');
	});
	Subscribe<void(Button*)>(&fireBlock.EventClicked,[d](Button * b) {
		d->QueueTouchEvent('f');
	});
	Subscribe<void(Button*)>(&makeFire.EventClicked,[d](Button * b) {
		d->QueueTouchEvent('x');
	});
	Subscribe<void(Button*)>(&increaseAcid.EventClicked,[d](Button * b) {
		d->QueueTouchEvent('y');
	});
	Subscribe<void(Button*)>(&decreaseAcid.EventClicked,[d](Button * b) {
		d->QueueTouchEvent('h');
	});
	Subscribe<void(Button*)>(&makeMesh.EventClicked,[d](Button * b) {
		d->QueueTouchEvent('0');
	});
}
DemoWindow::~DemoWindow() {

}