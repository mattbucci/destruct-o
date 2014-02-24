#include "stdafx.h"
#include "DemoWindow.h"
#include "Demo.h"

DemoWindow::DemoWindow(Demo * d) {
	//Size yourself
	position = Rect(10,100,600,200);
	//Add all the buttons in the appropriate places
	//Center all the buttons vertically
	makePyramid.vPin = fireBlock.vPin = makeFire.vPin = increaseAcid.vPin = decreaseAcid.vPin = Control::CENTER;
	//Now place each at a different horizontal position
	makePyramid.position =	Rect(20+116*0,0,105,170);
	fireBlock.position =	Rect(20+116*1,0,105,170);
	makeFire.position =		Rect(20+116*2,0,105,170);
	increaseAcid.position = Rect(20+116*3,0,105,170);
	decreaseAcid.position = Rect(20+116*4,0,105,170);
	//Label each button
	makePyramid.SetText("Pyramid");
	fireBlock.SetText("Launch");
	makeFire.SetText("Fire");
	increaseAcid.SetText("Dose");
	decreaseAcid.SetText("Undose");
	//Add each button to the window
	AddControl(&makePyramid);
	AddControl(&fireBlock);
	AddControl(&makeFire);
	AddControl(&increaseAcid);
	AddControl(&decreaseAcid);

	//Now subscribe to each button
	Subscribe<void(Button*)>(&makePyramid.EventClicked,[d](Button * b) {
		d->QueueTouchEvent('3');
	});
	Subscribe<void(Button*)>(&fireBlock.EventClicked,[d](Button * b) {
		d->QueueTouchEvent('f');
	});
	Subscribe<void(Button*)>(&makeFire.EventClicked,[d](Button * b) {
		d->QueueTouchEvent('r');
	});
	Subscribe<void(Button*)>(&increaseAcid.EventClicked,[d](Button * b) {
		d->QueueTouchEvent('y');
	});
	Subscribe<void(Button*)>(&decreaseAcid.EventClicked,[d](Button * b) {
		d->QueueTouchEvent('h');
	});
}
DemoWindow::~DemoWindow() {

}