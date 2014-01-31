#pragma once

#include "stdafx.h"
#include "Window.h"
#include "Button.h"
#include "Label.h"

//A window witha  title, an error message, and an ok button
//automatically destroys itself when ok is pressed
class ErrorWindow : public Window {
	Button okButton;
	Label errorMessage;
public:

	function<void()> OnOk;

	ErrorWindow(string title, string error);
	~ErrorWindow();
};