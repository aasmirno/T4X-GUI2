#include <T4X/game/GameManager.h>

bool GameManager::initialise()
{
	printf("Engine Start:\n");
	// initialise all sdl modules
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		printf("ERROR: SDL failed to initialise %s\n", SDL_GetError());
		return false;
	}

	// try and initialise the rendering manager
	if (render_manager.initialise(1000, 1500) == false)
	{
		printf("ERROR: render manager failed to initialise\n");
		return false;
	}
	render_manager.set_handler(std::bind(&GameManager::handleEvent, this, std::placeholders::_1));

	// set callback for event handler
	input_manager.set_handler(std::bind(&GameManager::handleEvent, this, std::placeholders::_1));

	/*
		Post init actions
	*/

	// start the game
	running = true;
	run();

	// clean up
	printf("Engine shutting down\n");
	render_manager.shutdown();

	return true;
}

bool GameManager::run()
{
	while (running)
	{
		loop();
	}
	return true;
}

void GameManager::handleEvent(Event e)
{
	// menu event
	if (e.type == E_TYPE::MENU_EVENT) {
		switch (e.menu_data) {
		case MN_DATA::EXIT:
			running = false;
			break;
		default:
			break;
		}
	}

	// sdl event
	if (e.type == E_TYPE::SDL_EVENT)
	{
		switch (e.raw_event) {
		case SDL_QUIT:
			running = false;
			break;
		default:
			break;
		}
	}
	// screen update
	if (e.type == E_TYPE::SCREEN_SIZE_UPDATE) {
		render_manager.setScreenSize(e.w, e.h);
	}
	// render passthrough
	if (e.type == E_TYPE::RENDER_EVENT)
	{
		render_manager.eventUpdate(e);
	}
}

void GameManager::new_game() {
	// initialised map pair
	game_map.initialise(200, 200);
	render_manager.addMeshObject(1);
	render_manager.setMeshData(1, game_map.getHeightData(), game_map.getWidth(), game_map.getHeight());


	//TODO prep game structures

}

void GameManager::load_game() {
	// push load game menu
	//TODO load game function
}


bool GameManager::loop()
{
	//get events and send key updates to renderer
	input_manager.pollEvent();
	render_manager.keyUpdate(input_manager.getRenderKeys());

	// menu specific draw commands
	if (render_manager.menuActive()) {
		
	}

	//render current graphics state
	render_manager.render();

	return true;
}

void GameManager::sendKeys() {
	bool* keys = input_manager.getKeys();
	if (!keys) {
		printf("[ MANAGER ERROR ] input manager failed to return array\n");
		return;
	}
}