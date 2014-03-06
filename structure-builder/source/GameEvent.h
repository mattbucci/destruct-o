#pragma once

#include "stdafx.h"

#include "GameEventInterface.h"
#include "GameEventSubscriber.h"

//T should be a type of std::function 
template <class T>
class GameEvent : public GameEventTemplateInterface<T> {
	typedef function<T> eventType;
	struct subscriber {
		GameEventSubscriber * object;
		eventType subscribedFunction;
	};
	vector<subscriber> subscriberList;
public:
	GameEvent() {

	}
	~GameEvent() {
		//On destruction notify everyone subscribed to you
		//that they no longer have to worry about cleanup
		for (auto subscriber : subscriberList) 
			subscriber.object->Unsubscribe(this);
	}

	//Subscribe to this event with the intent of never unsubscribing
	//only do this if your system is never destroyed
	void Subscribe(eventType toCall) {
		subscriber s;
		s.object = NULL;
		s.subscribedFunction = toCall;
		subscriberList.push_back(s);
	}

	//Subscribe to this event with the ability to unsubcribe
	//by calling Unsubscribe(object)
	//multiple subscriptions by the same object are allowed
	//but they will all be removed during a single unsubscribe()
	void Subscribe(GameEventSubscriber * object, eventType toCall) override {
		subscriber s;
		s.object = object;
		s.subscribedFunction = toCall;
		subscriberList.push_back(s);
	}

	//Remove all subscriptions to this event owned by the given object
	void Unsubscribe(GameEventSubscriber * object) override {
		vector<subscriber> tempList;
		//Rebuild the list without any of the events
		//by that object
		for (auto subscriber : subscriberList) {
			if (subscriber.object != object)
				tempList.push_back(subscriber);
		}
		subscriberList = tempList;
	}


	//Fire this event
	//for each subscribed member the passed function will be called
	//that function should call the member with the parameters
	//the supplied function should NOT modify any of the parameters during
	//its execution
	void Fire(function<void(eventType)> mapCallToArguments) {
		for (auto subscriber : subscriberList) {
			mapCallToArguments(subscriber.subscribedFunction);
		}
	}
};