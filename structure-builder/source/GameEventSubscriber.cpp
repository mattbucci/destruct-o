#include "stdafx.h"
#include "GameEventSubscriber.h"
#include "GameEventInterface.h"

void GameEventSubscriber::TrackSubscription(GameEventInterface * eventToSubscribe) {
	//Add the subscription to the list if its not already there
	if (subscriptions.find(eventToSubscribe) == subscriptions.end())
		subscriptions.insert(eventToSubscribe);
}
void GameEventSubscriber::Unsubscribe(GameEventInterface * eventToUnsubscribe) {
	//Unsubscribe and remove from the subscription list
	eventToUnsubscribe->Unsubscribe(this);
	if (subscriptions.find(eventToUnsubscribe) != subscriptions.end())
		subscriptions.erase(eventToUnsubscribe);
}
GameEventSubscriber::GameEventSubscriber() {

}
GameEventSubscriber::~GameEventSubscriber() {
	//Unsubscribe from everything
	for (auto sub : subscriptions) {
		sub->Unsubscribe(this);
	}
}
