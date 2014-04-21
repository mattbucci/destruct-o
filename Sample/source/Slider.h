#pragma once
#include "stdafx.h"
#include "Control.h"
#include "Label.h"
#include "Button.h"
#include "Shape.h"

template <class T>
class Slider : public Control, private GameEventSubscriber {
	unsigned int max;
	unsigned int index;
	vector<pair<string, T>> values;

	T* value;

	Button down;
	Button up;
	Label text;
	Shape fill;

	void updateText();
	void updateValue();

protected:
	virtual void OnMousePress(vec2 mousePos, int Slider, bool down);
	virtual void OnMouseMove(vec2 mousePos);
	virtual void OnMouseLeave();
	virtual void OnMouseEnter();
	virtual void OnSliderPress();

public:
	Slider();
	Slider(Rect position);
	Slider(Rect position, string text);
	~Slider();

	GameEvent<void(Slider*)> EventClicked;

	void SetValue(T* value);
	void SetPosition(Rect p);
	void SetElements(const vector<pair<string, T>> in);
};

/**
 * Constructors
 */

template <class T>
Slider<T>::Slider() :
	up(Rect(0,0,25,25), "+"),
	down(Rect(0,0,25,25), "-"),
	fill(4)
{
	up.hPin = Control::MAX;
	text.hPin = text.vPin = Control::CENTER;
	color = vec4(.66,.66,.66,1);

	index = 0;
	max = 100;
	values.push_back(pair<string, T>("Default", 0));
	value = NULL;

	fill.SetToSquare();
	fill.position = Rect(25,0,0,0);
	fill.color = vec4(1,1,1,.5);

	AddChild(&up);
	AddChild(&down);
	AddChild(&fill);
	AddChild(&text);

	Subscribe<void(Button*)>(&up.EventClicked, [this](Button * b) {
		if(index < max) {
			index++;
			updateText();
			updateValue();
		}
	});
	Subscribe<void(Button*)>(&down.EventClicked, [this](Button * b) {
		if(index > 0) {
			index--;
			updateText();
			updateValue();
		}
	});

	updateText();
}
template <class T>
Slider<T>::Slider(Rect position)
{
	Slider::Slider();
	SetPosition(position);
}
template <class T>
Slider<T>::Slider(Rect position, string text)
{
	Slider::Slider();
	SetPosition(position);
}

/**
 * Destructor
 */

template <class T>
Slider<T>::~Slider() { }

/**
 * Functions
 */

template <class T>
void Slider<T>::OnMousePress(vec2 mousePos, int Button, bool down) {
	Control::OnMousePress(mousePos, Button, down);
}

template <class T>
void Slider<T>::OnMouseMove(vec2 mousePos) {
	Control::OnMouseMove(mousePos);

}

template <class T>
void Slider<T>::OnSliderPress() {
	EventClicked.Fire([this](function<void(Slider*)> eventSubscriber){
		eventSubscriber(this);
	});
}

template <class T>
void Slider<T>::OnMouseLeave() {
	Control::OnMouseLeave();
}

template <class T>
void Slider<T>::OnMouseEnter() {
	Control::OnMouseEnter();
}

template <class T>
void Slider<T>::SetPosition(Rect p) {
	position = p;
	up.position = down.position = Rect(0,0,p.Height,p.Height);
	fill.position = Rect(p.Height,0,(p.Width-2*p.Height)*index/(float)max,p.Height);
}

template <class T>
void Slider<T>::SetElements(const vector<pair<string, T>> in) {
	values = vector<pair<string, T>>(in);
	max = in.size()-1;
	index = 0;
	updateText();
	updateValue();
}

template <class T>
void Slider<T>::updateText() {
	text.SetText(values[index].first);
	fill.position = Rect(position.Height,0,(position.Width-2*position.Height)*index/(float)max,position.Height);
}

template <class T>
void Slider<T>::updateValue() {
	if(value != NULL) {
		*value = values[index].second;
	}
}

template <class T>
void Slider<T>::SetValue(T* value) {
	this->value = value;
}