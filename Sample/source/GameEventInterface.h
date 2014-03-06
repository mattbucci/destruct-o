
#pragma once

class GameEventSubscriber;


//Game events must be able to be unsubscribed from
class GameEventInterface {
public:
	virtual ~GameEventInterface() {}

	virtual void Unsubscribe(GameEventSubscriber * object) = 0;

};

//This second template interface class seems unnecessary
//but its actually necessary to resolve a circulate template dependency
//This template interface allows GameEvent to rely on GameEventSubscriber
//which only relies on this template interface instead of back on GameEvent

template <class T>
class GameEventTemplateInterface : public GameEventInterface {
public:
	virtual void Subscribe(GameEventSubscriber * object, function<T> toCall) = 0;
};