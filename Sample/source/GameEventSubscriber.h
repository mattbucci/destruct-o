
#pragma once

#include "stdafx.h"

#include "GameEventInterface.h"

//Use this within objects which inherit GameEventSubscriber
#define SUBSCRIBE(event,function) event->Subscribe(this,function);TrackSubscription(event);
#define UNSUBSCRIBE(event) Unsubscribe(event);

//Used to track an object's subscriptions
class GameEventSubscriber {
	set<GameEventInterface*> subscriptions;
public:
	GameEventSubscriber();
	~GameEventSubscriber();

	void TrackSubscription(GameEventInterface * eventToSubscribe);
	void Unsubscribe(GameEventInterface * eventToUnsubscribe);

	template<class T>
	void Subscribe(GameEventTemplateInterface<T> * eventToSub, function<T> functionToCall) {
		eventToSub->Subscribe(this,functionToCall);
		TrackSubscription(eventToSub);
	}
};