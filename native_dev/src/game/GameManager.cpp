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
	if (render_manager.initialise(600, 600) == false)
	{
		printf("ERROR: render manager failed to initialise\n");
		return false;
	}

	// initialise the game map
	game_map.initialise(200, 200);

	// set callback for event handler
	input_manager.set_handler(std::bind(&GameManager::handleEvent, this, std::placeholders::_1));

	/*
		TESTING
	*/
	// add a test object with id 0
	if (!render_manager.addTestObject()) return false;
	assert(render_manager.addMeshObject(0) == nullptr);
	assert(render_manager.setMeshData(0, game_map.getHeightData(), game_map.getWidth(), game_map.getHeight()) == false);

	if (!render_manager.addMeshObject(4)) return false;
	if (!render_manager.setMeshData(4, game_map.getHeightData(), game_map.getWidth(), game_map.getHeight())) return false;
	//if (!render_manager.addTexturedObject(4)) return false;
	//render_manager.setTexture(4, "Logo.png");

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

	if (e.type == E_TYPE::SCREEN_SIZE_UPDATE) {
		render_manager.setScreenSize(e.w, e.h);
	}

	if (e.type == E_TYPE::RENDER_EVENT)
	{
		render_manager.eventUpdate(e);
	}
}

bool GameManager::loop()
{
	//get events and send key updates to renderer
	input_manager.pollEvent();
	render_manager.keyUpdate(input_manager.getRenderKeys());

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