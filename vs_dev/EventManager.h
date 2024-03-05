#pragma once
#include <vector>
#include <iostream>

/*
	class Event Manager
	Handle global events and broadcast them to listeners

	methods:
		submit(Event e)
			submit an event to the global event queue
		get()
			get list of current events
		clear()
			remove one turn of events
*/

class EventManager {
public:
	enum EventType {WINDOW_EVENT, GAME_EVENT, MENUCHANGE_EVENT};
	struct StateEvent {
		EventType type;
		uint32_t tick;
		uint32_t menu_id;	//menu id to make active
	};
};
