#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 800;
    const int BLOCK_SIZE = 20;

    enum Direction {UP, DOWN, LEFT, RIGHT};

    struct SnakeSegment{
        int x, y;
    };

    class SnakeGame{
        std::vector<SnakeSegment> snake;
        public:
            SnakeGame() : direction(RIGHT), score(0), running(true){
                snake.push_back(SnakeSegment{SCREEN_WIDTH/2, SCREEN_HEIGHT/2});
                spawnFood();
            }

            void spawnFood(){
                food.x = (rand() % (SCREEN_WIDTH / BLOCK_SIZE)) * BLOCK_SIZE;
                food.y = (rand() % (SCREEN_HEIGHT / BLOCK_SIZE)) * BLOCK_SIZE;
            }

            void update(){
                if (!running) return;

                //Move Snake
                SnakeSegment newHead = snake.front();
                switch(direction){
                    case UP: newHead.y -= BLOCK_SIZE; break;
                    case DOWN: newHead.y += BLOCK_SIZE; break;
                    case LEFT: newHead.x -= BLOCK_SIZE; break;
                    case RIGHT: newHead.x += BLOCK_SIZE; break;
                }

                //Check collision with food
                if (newHead.x == food.x && newHead.y == food.y){
                    score++;
                    snake.insert(snake.begin(), newHead); // Grow snake
                    spawnFood();
                } else{
                    snake.insert(snake.begin(), newHead);
                    snake.pop_back(); // Move snake without growing
                }

                //Check collision with walls or itself
                if (newHead.x < 0 || newHead.y >= SCREEN_WIDTH || newHead.y < 0 || newHead.y >= SCREEN_HEIGHT){
                    running = false; // Collided with wall
                }
                for (size_t i = 1; i < snake.size(); i++){
                    if (newHead.x == snake[i].x && newHead.y == snake[i].y){
                        running = false; // Collided with self
                    }
                }
            }
            void handleInput(){
                SDL_Event event;
                while (SDL_PollEvent(&event)){
                    if (event.type == SDL_QUIT){
                        running = false;
                    } else if (event.type == SDL_KEYDOWN){
                        switch(event.key.keysym.sym){
                            case SDLK_UP:
                                if (direction != DOWN) direction = UP; break;
                            case SDLK_DOWN:
                                if (direction != UP) direction = DOWN; break;
                            case SDLK_LEFT: 
                                if (direction != RIGHT) direction = LEFT; break;
                            case SDLK_RIGHT:
                                if (direction != LEFT) direction = RIGHT; break;
                        }
                    }
                }
            }
            void render(SDL_Renderer* renderer) {
                // Clear screen
                SDL_SetRenderDrawColor(renderer, 0, 0, 0,SDL_ALPHA_OPAQUE);
                SDL_RenderClear(renderer);

                // Draw snake
                SDL_SetRenderDrawColor(renderer, 0, 255, 0,SDL_ALPHA_OPAQUE);
                for (const auto& segment : snake) {
                    SDL_Rect rect = { segment.x, segment.y, BLOCK_SIZE, BLOCK_SIZE };
                    SDL_RenderFillRect(renderer, &rect);
                }

                // Draw food
                SDL_SetRenderDrawColor(renderer, 255, 0, 0,SDL_ALPHA_OPAQUE);
                SDL_Rect foodRect = { food.x, food.y, BLOCK_SIZE, BLOCK_SIZE };
                SDL_RenderFillRect(renderer, &foodRect);

                // Present renderer
                SDL_RenderPresent(renderer);
            }

        bool runnings() const { return running; }

        private:
            Direction direction;
            SnakeSegment food;
            int score;
            bool running;

    };

    int main(int argc, char* argv[]) {
        srand(static_cast<unsigned>(time(0)));

        // Initialize SDL
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
            return 1;
        }

        // Create a window
        SDL_Window* window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
        if (window == nullptr) {
            std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
            SDL_Quit();
            return 1;
        }

        // Create a renderer
        SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer == nullptr) {
            std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 1;
        }

        // Game loop
        SnakeGame game;
        while (game.runnings()) {
            game.handleInput();
            game.update();
            game.render(renderer);
            SDL_Delay(100); // Control the speed of the snake
        }

        // Clean up
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();

        return 0;
}