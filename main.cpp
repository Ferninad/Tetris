#include "common.h"
#include "cmath"
#include "vector"

bool Init();
void CleanUp();
void Run();
void SpawnPiece();
void Draw();
void TranslatePiece();
void RotatePiece();
void RotPiece();
void TickPiece();
void LineCheck();
void ClearLine(int line);
bool GameOverCheck();

SDL_Window *window;
SDL_GLContext glContext;
SDL_Surface *gScreenSurface = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Rect pos;

int screenWidth = 500;
int screenHeight = 500;
double gridSize = screenHeight/20;
bool pieceInPlay = false;
int level = 5;
bool a = false;
bool d = false;
bool w = false;
bool s = false;
bool leftArrow = false;
bool rightArrow = false;

vector<vector<int>> palette;
vector<int> LevelSpeeds = {48, 43, 38, 33, 28, 23, 18, 13, 8, 6, 5, 5, 5, 4, 4, 4, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
vector<vector<vector<int>>> grid;
vector<vector<int>> piece;

bool Init()
{
    if (SDL_Init(SDL_INIT_NOPARACHUTE & SDL_INIT_EVERYTHING) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
        return false;
    }
    else
    {
        //Specify OpenGL Version (4.2)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_Log("SDL Initialised");
    }

    //Create Window Instance
    window = SDL_CreateWindow(
        "Game Engine",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        screenWidth,
        screenHeight,   
        SDL_WINDOW_OPENGL);

    //Check that the window was succesfully created
    if (window == NULL)
    {
        //Print error, if null
        printf("Could not create window: %s\n", SDL_GetError());
        return false;
    }
    else{
        gScreenSurface = SDL_GetWindowSurface(window);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        SDL_Log("Window Successful Generated");
    }
    //Map OpenGL Context to Window
    glContext = SDL_GL_CreateContext(window);

    return true;
}

int main()
{
    //Error Checking/Initialisation
    if (!Init())
    {
        printf("Failed to Initialize");
        return -1;
    }

    // Clear buffer with black background
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    //Swap Render Buffers
    SDL_GL_SwapWindow(window);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    Run();

    CleanUp();
    return 0;
}

void CleanUp()
{
    //Free up resources
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Run()
{
    bool gameLoop = true;
    srand(time(NULL));

    palette.push_back({227, 181, 5});
    palette.push_back({149, 25, 12});
    palette.push_back({97, 3, 69});
    palette.push_back({16, 126, 125});
    palette.push_back({4, 75, 127});
    palette.push_back({63, 102, 52});
    for(int x = 0; x < 10; x++){
        grid.push_back({{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}});
    }
    int time = clock();
    int frameCount = 0;
    SpawnPiece();
    pieceInPlay = true;
    while (gameLoop)
    {   
        int ctime = clock();
        
        TranslatePiece();
        RotatePiece();
        if(ctime - time > 1000/60){
            time = ctime;
            frameCount++;
            if(!pieceInPlay){
                LineCheck();
                SpawnPiece();
                pieceInPlay = true;
            }
            else if(pieceInPlay && frameCount >= LevelSpeeds[level]){
                frameCount = 0;
                TickPiece();
            }
            Draw();
            SDL_RenderPresent(renderer);
            pos.x = 0;
            pos.y = 0;
            pos.w = screenWidth;
            pos.h = screenHeight;
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderFillRect(renderer, &pos);
        }

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                gameLoop = false;
            }
            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym){
                    case SDLK_ESCAPE:
                        gameLoop = false;
                        break;
                    case SDLK_a:
                        a = true;
                        break;
                    case SDLK_d:
                        d = true;
                        break;
                    // case SDLK_w:
                    //     w = true;
                    //     break;
                    // case SDLK_s:
                    //     s = true;
                    //     break;
                    case SDLK_LEFT:
                        leftArrow = true;
                        break;
                    case SDLK_RIGHT:
                        rightArrow = true;
                        break;
                    default:
                        break;
                }
            }

            if (event.type == SDL_KEYUP)
            {
                switch (event.key.keysym.sym){
                    default:
                        break;
                }
            }
        }
    }
}

void SpawnPiece(){
    piece.clear();
    int num = rand() % 7 + 1;
    int color = rand() % 6;
    if(num == 1){
        vector<int> temp;
        for(int i = 3; i < 7; i++){
            temp.push_back(i);  //x
            temp.push_back(0);  //y
            temp.push_back(palette[color][0]); //r
            temp.push_back(palette[color][1]); //g
            temp.push_back(palette[color][2]); //b
            temp.push_back(num); //piece
            temp.push_back(0); //rotation
            temp.push_back(0); //prevrotation
            piece.push_back(temp);
            temp.clear();
        }
    }
    if(num == 2){
        vector<int> temp;
        for(int i = 3; i < 5; i++){
            temp.push_back(i);  //x
            temp.push_back(0);  //y
            temp.push_back(palette[color][0]); //r
            temp.push_back(palette[color][1]); //g
            temp.push_back(palette[color][2]); //b
            temp.push_back(num); //piece
            temp.push_back(0); //rotation
            temp.push_back(0); //prevrotation
            piece.push_back(temp);
            temp.clear();
        }
        for(int i = 4; i < 6; i++){
            temp.push_back(i);  //x
            temp.push_back(1);  //y
            temp.push_back(palette[color][0]); //r
            temp.push_back(palette[color][1]); //g
            temp.push_back(palette[color][2]); //b
            temp.push_back(num); //piece
            temp.push_back(0); //rotation
            temp.push_back(0); //prevrotation
            piece.push_back(temp);
            temp.clear();
        }
    }
    if(num == 3){
        vector<int> temp;
        for(int i = 5; i > 3; i--){
            temp.push_back(i);  //x
            temp.push_back(0);  //y
            temp.push_back(palette[color][0]); //r
            temp.push_back(palette[color][1]); //g
            temp.push_back(palette[color][2]); //b
            temp.push_back(num); //piece
            temp.push_back(0); //rotation
            temp.push_back(0); //prevrotation
            piece.push_back(temp);
            temp.clear();
        }
        for(int i = 4; i > 2; i--){
            temp.push_back(i);  //x
            temp.push_back(1);  //y
            temp.push_back(palette[color][0]); //r
            temp.push_back(palette[color][1]); //g
            temp.push_back(palette[color][2]); //b
            temp.push_back(num); //piece
            temp.push_back(0); //rotation
            temp.push_back(0); //prevrotation
            piece.push_back(temp);
            temp.clear();
        }
    }
    if(num == 4){
        vector<int> temp;
        temp.push_back(3);  //x
        temp.push_back(0);  //y
        temp.push_back(palette[color][0]); //r
        temp.push_back(palette[color][1]); //g
        temp.push_back(palette[color][2]); //b
        temp.push_back(num); //piece
        temp.push_back(0); //rotation
        temp.push_back(0); //prevrotation
        piece.push_back(temp);
        temp.clear();
        for(int i = 3; i < 6; i++){
            temp.push_back(i);  //x
            temp.push_back(1);  //y
            temp.push_back(palette[color][0]); //r
            temp.push_back(palette[color][1]); //g
            temp.push_back(palette[color][2]); //b
            temp.push_back(num); //piece
            temp.push_back(0); //rotation
            temp.push_back(0); //prevrotation
            piece.push_back(temp);
            temp.clear();
        }
    }
    if(num == 5){
        vector<int> temp;
        temp.push_back(5);  //x
        temp.push_back(0);  //y
        temp.push_back(palette[color][0]); //r
        temp.push_back(palette[color][1]); //g
        temp.push_back(palette[color][2]); //b
        temp.push_back(num); //piece
        temp.push_back(0); //rotation
        temp.push_back(0); //prevrotation
        piece.push_back(temp);
        temp.clear();
        for(int i = 5; i > 2; i--){
            temp.push_back(i);  //x
            temp.push_back(1);  //y
            temp.push_back(palette[color][0]); //r
            temp.push_back(palette[color][1]); //g
            temp.push_back(palette[color][2]); //b
            temp.push_back(num); //piece
            temp.push_back(0); //rotation
            temp.push_back(0); //prevrotation
            piece.push_back(temp);
            temp.clear();
        }
    }
    if(num == 6){
        vector<int> temp;
        temp.push_back(4);  //x
        temp.push_back(0);  //y
        temp.push_back(palette[color][0]); //r
        temp.push_back(palette[color][1]); //g
        temp.push_back(palette[color][2]); //b
        temp.push_back(num); //piece
        temp.push_back(0); //rotation
        temp.push_back(0); //prevrotation
        piece.push_back(temp);
        temp.clear();
        for(int i = 3; i < 6; i++){
            temp.push_back(i);  //x
            temp.push_back(1);  //y
            temp.push_back(palette[color][0]); //r
            temp.push_back(palette[color][1]); //g
            temp.push_back(palette[color][2]); //b
            temp.push_back(num); //piece
            temp.push_back(0); //rotation
            temp.push_back(0); //prevrotation
            piece.push_back(temp);
            temp.clear();
        }
    }
    if(num == 7){
        vector<int> temp;
        for(int i = 4; i < 6; i++){
            temp.push_back(i);  //x
            temp.push_back(0);  //y
            temp.push_back(palette[color][0]); //r
            temp.push_back(palette[color][1]); //g
            temp.push_back(palette[color][2]); //b
            temp.push_back(num); //piece
            temp.push_back(0); //rotation
            temp.push_back(0); //prevrotation
            piece.push_back(temp);
            temp.clear();
        }
        for(int i = 4; i < 6; i++){
            temp.push_back(i);  //x
            temp.push_back(1);  //y
            temp.push_back(palette[color][0]); //r
            temp.push_back(palette[color][1]); //g
            temp.push_back(palette[color][2]); //b
            temp.push_back(num); //piece
            temp.push_back(0); //rotation
            temp.push_back(0); //prevrotation
            piece.push_back(temp);
            temp.clear();
        }
    }

    bool gameover = GameOverCheck();
    if(!gameover){
        for(int i = 0; i < 4; i++){
            grid[piece[i][0]][piece[i][1]][0] = piece[i][2];
            grid[piece[i][0]][piece[i][1]][1] = piece[i][3];
            grid[piece[i][0]][piece[i][1]][2] = piece[i][4];
        }
    }
    else if(gameover){
        cout << "GameOver" << endl;
    }
}

void TranslatePiece(){
    vector<vector<int>> temp = piece;
    for(int i = 0; i < temp.size(); i++){
        if(a){
            temp[i][0]--;
        }
        else if(d){
            temp[i][0]++;
        }
        else if(w){
            temp[i][1]--;
        }
        else if(s){
            temp[i][1]++;
        }
    }
    for(int i = 0; i < temp.size(); i++){
        if(temp[i][0] < 0 || temp[i][0] > grid.size() - 1 || temp[i][1] < 0 || temp[i][1] > grid[i].size() - 1){
            temp = piece;
        }
    }
    if(temp != piece){
        for(int i = 0; i < piece.size(); i++){
            grid[piece[i][0]][piece[i][1]][0] = 0;
            grid[piece[i][0]][piece[i][1]][1] = 0;
            grid[piece[i][0]][piece[i][1]][2] = 0;
        }
        for(int i = 0; i < temp.size(); i++){
            if(grid[temp[i][0]][temp[i][1]][0] != 0 && grid[temp[i][0]][temp[i][1]][1] != 0 && grid[temp[i][0]][temp[i][1]][2] != 0){
                temp = piece;
            }
        }
    }
    for(int i = 0; i < temp.size(); i++){
        grid[temp[i][0]][temp[i][1]][0] = temp[i][2];
        grid[temp[i][0]][temp[i][1]][1] = temp[i][3];
        grid[temp[i][0]][temp[i][1]][2] = temp[i][4];
    }
    piece = temp;

    a = false;
    d = false;
    w = false;
    s = false;
}

void RotatePiece(){
    if(leftArrow){
        for(int i = 0; i < piece[i].size(); i++){
            piece[i][7] = piece[i][6];
            piece[i][6]--;
            if(piece[i][6] < 0)
                piece[i][6] = 3;
        }
        RotPiece();
        leftArrow = false;
    }
    else if(rightArrow){
        for(int i = 0; i < piece[i].size(); i++){
            piece[i][7] = piece[i][6];
            piece[i][6]++;
            if(piece[i][6] > 3)
                piece[i][6] = 0;
        }
        RotPiece();
        rightArrow = false;
    }
}

void RotPiece(){
    vector<vector<int>> temp = piece;
    if(temp[0][5] == 1){
        if(temp[0][6] == 0){
            if(temp[0][7] == 3){
                int x = piece[1][0] - 1;
                if(x < 0)
                    x = 0;
                else if(x > grid.size() - 4)
                    x = grid.size() - 4;
                temp[0][0] = x;
                temp[0][1] = piece[1][1];
                temp[1][0] = x + 1;
                temp[1][1] = piece[1][1];
                temp[2][0] = x + 2;
                temp[2][1] = piece[1][1];
                temp[3][0] = x + 3;
                temp[3][1] = piece[1][1];
            }
            if(temp[0][7] == 1){
                int x = piece[1][0] - 2;
                if(x < 0)
                    x = 0;
                else if(x > grid.size() - 4)
                    x = grid.size() - 4;
                temp[0][0] = x;
                temp[0][1] = piece[1][1];
                temp[1][0] = x + 1;
                temp[1][1] = piece[1][1];
                temp[2][0] = x + 2;
                temp[2][1] = piece[1][1];
                temp[3][0] = x + 3;
                temp[3][1] = piece[1][1];
            }
        }
        if(temp[0][6] == 1){
            if(temp[0][7] == 0){
                int y = piece[2][1] - 1;
                if(y < 0)
                    y = 0;
                else if(y > grid[0].size() - 4)
                    y = grid[0].size() - 4;
                temp[0][0] = piece[2][0];
                temp[0][1] = y;
                temp[1][0] = piece[2][0];
                temp[1][1] = y + 1;
                temp[2][0] = piece[2][0];
                temp[2][1] = y + 2;
                temp[3][0] = piece[2][0];
                temp[3][1] = y + 3;
            }
            else if(temp[0][7] == 2){
                int y = piece[2][1] - 2;
                if(y < 0)
                    y = 0;
                else if(y > grid[0].size() - 4)
                    y = grid[0].size() - 4;
                temp[0][0] = piece[2][0];
                temp[0][1] = y;
                temp[1][0] = piece[2][0];
                temp[1][1] = y + 1;
                temp[2][0] = piece[2][0];
                temp[2][1] = y + 2;
                temp[3][0] = piece[2][0];
                temp[3][1] = y + 3;
            }
        }
        else if(temp[0][6] == 2){
            if(temp[0][7] == 1){
                int x = piece[2][0] - 2;
                if(x < 0)
                    x = 0;
                else if(x > grid.size() - 4)
                    x = grid.size() - 4;
                temp[0][0] = x;
                temp[0][1] = piece[2][1];
                temp[1][0] = x + 1;
                temp[1][1] = piece[2][1];
                temp[2][0] = x + 2;
                temp[2][1] = piece[2][1];
                temp[3][0] = x + 3;
                temp[3][1] = piece[2][1];
            }
            if(temp[0][7] == 3){
                int x = piece[2][0] - 1;
                if(x < 0)
                    x = 0;
                else if(x > grid.size() - 4)
                    x = grid.size() - 4;
                temp[0][0] = x;
                temp[0][1] = piece[2][1];
                temp[1][0] = x + 1;
                temp[1][1] = piece[2][1];
                temp[2][0] = x + 2;
                temp[2][1] = piece[2][1];
                temp[3][0] = x + 3;
                temp[3][1] = piece[2][1];
            }
        }
        if(temp[0][6] == 3){
            if(temp[0][7] == 0){
                int y = piece[1][1] - 1;
                if(y < 0)
                    y = 0;
                else if(y > grid[0].size() - 4)
                    y = grid[0].size() - 4;
                temp[0][0] = piece[1][0];
                temp[0][1] = y;
                temp[1][0] = piece[1][0];
                temp[1][1] = y + 1;
                temp[2][0] = piece[1][0];
                temp[2][1] = y + 2;
                temp[3][0] = piece[1][0];
                temp[3][1] = y + 3;
            }
            else if(temp[0][7] == 2){
                int y = piece[1][1] - 2;
                if(y < 0)
                    y = 0;
                else if(y > grid[0].size() - 4)
                    y = grid[0].size() - 4;
                temp[0][0] = piece[1][0];
                temp[0][1] = y;
                temp[1][0] = piece[1][0];
                temp[1][1] = y + 1;
                temp[2][0] = piece[1][0];
                temp[2][1] = y + 2;
                temp[3][0] = piece[1][0];
                temp[3][1] = y + 3;
            }
        }
    }
    if(temp[0][5] == 2){
        if(temp[0][6] == 0){
            if(temp[0][7] == 3){
                int x = piece[0][0];
                if(x > grid.size() - 3)
                    x = grid.size() - 3;
                temp[0][0] = x;
                temp[0][1] = piece[3][1];
                temp[1][0] = x + 1;
                temp[1][1] = piece[3][1];
                temp[2][0] = x + 1;
                temp[2][1] = piece[2][1];
                temp[3][0] = x + 2;
                temp[3][1] = piece[2][1];
            }
            if(temp[0][7] == 1){
                int x = piece[2][0] - 1;
                if(x < 0)
                    x = 0;
                temp[0][0] = x;
                temp[0][1] = piece[0][1];
                temp[1][0] = x + 1;
                temp[1][1] = piece[0][1];
                temp[2][0] = x + 1;
                temp[2][1] = piece[1][1];
                temp[3][0] = x + 2;
                temp[3][1] = piece[1][1];
            }
        }
        if(temp[0][6] == 1){
            if(temp[0][7] == 0){
                int y = piece[0][1];
                if(y > grid[0].size() - 3)
                    y = grid[0].size() - 3;
                temp[0][0] = piece[3][0];
                temp[0][1] = y;
                temp[1][0] = piece[3][0];
                temp[1][1] = y + 1;
                temp[2][0] = piece[2][0];
                temp[2][1] = y + 1;
                temp[3][0] = piece[2][0];
                temp[3][1] = y + 2;
            }
            else if(temp[0][7] == 2){
                int y = piece[2][1] - 1;
                if(y < 0)
                    y = 0;
                temp[0][0] = piece[0][0];
                temp[0][1] = y;
                temp[1][0] = piece[0][0];
                temp[1][1] = y + 1;
                temp[2][0] = piece[1][0];
                temp[2][1] = y + 1;
                temp[3][0] = piece[1][0];
                temp[3][1] = y + 2;
            }
        }
        else if(temp[0][6] == 2){
            if(temp[0][7] == 1){
                int x = piece[0][0];
                if(x < 2)
                    x = 2;
                temp[0][0] = x;
                temp[0][1] = piece[3][1];
                temp[1][0] = x - 1;
                temp[1][1] = piece[3][1];
                temp[2][0] = x - 1;
                temp[2][1] = piece[2][1];
                temp[3][0] = x - 2;
                temp[3][1] = piece[2][1];
            }
            if(temp[0][7] == 3){
                int x = piece[2][0] + 1;
                if(x > grid.size() - 1)
                    x = grid.size() - 1;
                temp[0][0] = x;
                temp[0][1] = piece[0][1];
                temp[1][0] = x - 1;
                temp[1][1] = piece[0][1];
                temp[2][0] = x - 1;
                temp[2][1] = piece[1][1];
                temp[3][0] = x - 2;
                temp[3][1] = piece[1][1];
            }
        }
        if(temp[0][6] == 3){
            if(temp[0][7] == 0){
                int y = piece[2][1] + 1;
                if(y > grid[0].size() - 1)
                    y = grid[0].size() - 1;
                temp[0][0] = piece[0][0];
                temp[0][1] = y;
                temp[1][0] = piece[0][0];
                temp[1][1] = y - 1;
                temp[2][0] = piece[1][0];
                temp[2][1] = y - 1;
                temp[3][0] = piece[1][0];
                temp[3][1] = y - 2;
            }
            else if(temp[0][7] == 2){
                int y = piece[0][1];
                if(y < 2)
                    y = 2;
                temp[0][0] = piece[3][0];
                temp[0][1] = y;
                temp[1][0] = piece[3][0];
                temp[1][1] = y - 1;
                temp[2][0] = piece[2][0];
                temp[2][1] = y - 1;
                temp[3][0] = piece[2][0];
                temp[3][1] = y - 2;
            }
        }
    }
    if(temp[0][5] == 3){
        if(temp[0][6] == 0){
            if(temp[0][7] == 3){
                int x = piece[0][0] + 2;
                if(x > grid.size() - 1)
                    x = grid.size() - 1;
                temp[0][0] = x;
                temp[0][1] = piece[0][1];
                temp[1][0] = x - 1;
                temp[1][1] = piece[0][1];
                temp[2][0] = x - 1;
                temp[2][1] = piece[1][1];
                temp[3][0] = x - 2;
                temp[3][1] = piece[1][1];
            }
            if(temp[0][7] == 1){
                int x = piece[0][0];
                if(x < 2)
                    x = 2;
                temp[0][0] = x;
                temp[0][1] = piece[3][1];
                temp[1][0] = x - 1;
                temp[1][1] = piece[3][1];
                temp[2][0] = x - 1;
                temp[2][1] = piece[2][1];
                temp[3][0] = x - 2;
                temp[3][1] = piece[2][1];
            }
        }
        if(temp[0][6] == 1){
            if(temp[0][7] == 0){
                int y = piece[2][1] + 1;
                if(y > grid[0].size() - 1)
                    y = grid[0].size() - 1;
                temp[0][0] = piece[0][0];
                temp[0][1] = y;
                temp[1][0] = piece[0][0];
                temp[1][1] = y - 1;
                temp[2][0] = piece[1][0];
                temp[2][1] = y - 1;
                temp[3][0] = piece[1][0];
                temp[3][1] = y - 2;
            }
            else if(temp[0][7] == 2){
                int y = piece[0][1];
                if(y < 2)
                    y = 2;
                temp[0][0] = piece[3][0];
                temp[0][1] = y;
                temp[1][0] = piece[3][0];
                temp[1][1] = y - 1;
                temp[2][0] = piece[2][0];
                temp[2][1] = y - 1;
                temp[3][0] = piece[2][0];
                temp[3][1] = y - 2;
            }
        }
        else if(temp[0][6] == 2){
            if(temp[0][7] == 1){
                int x = piece[2][0] - 1;
                if(x < 0)
                    x = 0;
                temp[0][0] = x;
                temp[0][1] = piece[0][1];
                temp[1][0] = x + 1;
                temp[1][1] = piece[0][1];
                temp[2][0] = x + 1;
                temp[2][1] = piece[1][1];
                temp[3][0] = x + 2;
                temp[3][1] = piece[1][1];
            }
            if(temp[0][7] == 3){
                int x = piece[0][0];
                if(x > grid.size() - 3)
                    x = grid.size() - 3;
                temp[0][0] = x;
                temp[0][1] = piece[3][1];
                temp[1][0] = x + 1;
                temp[1][1] = piece[3][1];
                temp[2][0] = x + 1;
                temp[2][1] = piece[2][1];
                temp[3][0] = x + 2;
                temp[3][1] = piece[2][1];
            }
        }
        if(temp[0][6] == 3){
            if(temp[0][7] == 0){
                int y = piece[0][1];
                if(y > grid[0].size() - 3)
                    y = grid[0].size() - 3;
                temp[0][0] = piece[3][0];
                temp[0][1] = y;
                temp[1][0] = piece[3][0];
                temp[1][1] = y + 1;
                temp[2][0] = piece[2][0];
                temp[2][1] = y + 1;
                temp[3][0] = piece[2][0];
                temp[3][1] = y + 2;
            }
            else if(temp[0][7] == 2){
                int y = piece[2][1] - 1;
                if(y < 0)
                    y = 0;
                temp[0][0] = piece[0][0];
                temp[0][1] = y;
                temp[1][0] = piece[0][0];
                temp[1][1] = y + 1;
                temp[2][0] = piece[1][0];
                temp[2][1] = y + 1;
                temp[3][0] = piece[1][0];
                temp[3][1] = y + 2;
            }
        }
    }
    if(temp[0][5] == 4){
        if(temp[0][6] == 0){
            if(temp[0][7] == 3){
                int x = piece[0][0];
                if(x > grid.size() - 3)
                    x = grid.size() - 3;
                temp[0][0] = x;
                temp[0][1] = piece[3][1];
                temp[1][0] = x;
                temp[1][1] = piece[2][1];
                temp[2][0] = x + 1;
                temp[2][1] = piece[2][1];
                temp[3][0] = x + 2;
                temp[3][1] = piece[2][1];
            }
            if(temp[0][7] == 1){
                int x = piece[1][0] - 1;
                if(x < 0)
                    x = 0;
                temp[0][0] = x;
                temp[0][1] = piece[1][1];
                temp[1][0] = x;
                temp[1][1] = piece[2][1];
                temp[2][0] = x + 1;
                temp[2][1] = piece[2][1];
                temp[3][0] = x + 2;
                temp[3][1] = piece[2][1];
            }
        }
        if(temp[0][6] == 1){
            if(temp[0][7] == 0){
                int y = piece[0][1];
                if(y > grid[0].size() - 3)
                    y = grid[0].size() - 3;
                temp[0][0] = piece[3][0];
                temp[0][1] = y;
                temp[1][0] = piece[2][0];
                temp[1][1] = y;
                temp[2][0] = piece[2][0];
                temp[2][1] = y + 1;
                temp[3][0] = piece[2][0];
                temp[3][1] = y + 2;
            }
            else if(temp[0][7] == 2){
                int y = piece[1][1] - 1;
                if(y < 0)
                    y = 0;
                temp[0][0] = piece[1][0];
                temp[0][1] = y;
                temp[1][0] = piece[2][0];
                temp[1][1] = y;
                temp[2][0] = piece[2][0];
                temp[2][1] = y + 1;
                temp[3][0] = piece[2][0];
                temp[3][1] = y + 2;
            }
        }
        else if(temp[0][6] == 2){
            if(temp[0][7] == 1){
                int x = piece[0][0];
                if(x < 2)
                    x = 2;
                temp[0][0] = x;
                temp[0][1] = piece[0][1] + 2;
                temp[1][0] = x;
                temp[1][1] = piece[0][1] + 1;
                temp[2][0] = x - 1;
                temp[2][1] = piece[2][1];
                temp[3][0] = x - 2;
                temp[3][1] = piece[2][1];
            }
            if(temp[0][7] == 3){
                int x = piece[1][0] + 1;
                if(x > grid.size() - 1)
                    x = grid.size() - 1;
                temp[0][0] = x;
                temp[0][1] = piece[1][1];
                temp[1][0] = x;
                temp[1][1] = piece[2][1];
                temp[2][0] = x - 1;
                temp[2][1] = piece[2][1];
                temp[3][0] = x - 2;
                temp[3][1] = piece[2][1];
            }
        }
        if(temp[0][6] == 3){
            if(temp[0][7] == 2){
                int y = piece[0][1];
                if(y < 2)
                    y = 2;
                temp[0][0] = piece[3][0];
                temp[0][1] = y;
                temp[1][0] = piece[2][0];
                temp[1][1] = y;
                temp[2][0] = piece[2][0];
                temp[2][1] = y - 1;
                temp[3][0] = piece[2][0];
                temp[3][1] = y - 2;
            }
            else if(temp[0][7] == 0){
                int y = piece[1][1] + 1;
                if(y > grid[0].size() - 1)
                    y = grid[0].size() - 1;
                temp[0][0] = piece[1][0];
                temp[0][1] = y;
                temp[1][0] = piece[2][0];
                temp[1][1] = y;
                temp[2][0] = piece[2][0];
                temp[2][1] = y - 1;
                temp[3][0] = piece[2][0];
                temp[3][1] = y - 2;
            }
        }
    }
    if(temp[0][5] == 5){
        if(temp[0][6] == 0){
            if(temp[0][7] == 3){
                int x = piece[1][0] + 1;
                if(x > grid.size() - 1)
                    x = grid.size() - 1;
                temp[0][0] = x;
                temp[0][1] = piece[1][1];
                temp[1][0] = x;
                temp[1][1] = piece[2][1];
                temp[2][0] = x - 1;
                temp[2][1] = piece[2][1];
                temp[3][0] = x - 2;
                temp[3][1] = piece[2][1];
            }
            if(temp[0][7] == 1){
                int x = piece[0][0];
                if(x < 2)
                    x = 2;
                temp[0][0] = x;
                temp[0][1] = piece[3][1];
                temp[1][0] = x;
                temp[1][1] = piece[2][1];
                temp[2][0] = x - 1;
                temp[2][1] = piece[2][1];
                temp[3][0] = x - 2;
                temp[3][1] = piece[2][1];
            }
        }
        if(temp[0][6] == 1){
            if(temp[0][7] == 0){
                int y = piece[1][1] + 1;
                if(y > grid[0].size() - 1)
                    y = grid[0].size() - 1;
                temp[0][0] = piece[0][0];
                temp[0][1] = y;
                temp[1][0] = piece[2][0];
                temp[1][1] = y;
                temp[2][0] = piece[2][0];
                temp[2][1] = y - 1;
                temp[3][0] = piece[2][0];
                temp[3][1] = y - 2;
            }
            else if(temp[0][7] == 2){
                int y = piece[0][1];
                if(y < 2)
                    y = 2;
                temp[0][0] = piece[3][0];
                temp[0][1] = y;
                temp[1][0] = piece[2][0];
                temp[1][1] = y;
                temp[2][0] = piece[2][0];
                temp[2][1] = y - 1;
                temp[3][0] = piece[2][0];
                temp[3][1] = y - 2;
            }
        }
        else if(temp[0][6] == 2){
            if(temp[0][7] == 1){
                int x = piece[1][0] - 1;
                if(x < 0)
                    x = 0;
                temp[0][0] = x;
                temp[0][1] = piece[0][1];
                temp[1][0] = x;
                temp[1][1] = piece[2][1];
                temp[2][0] = x + 1;
                temp[2][1] = piece[2][1];
                temp[3][0] = x + 2;
                temp[3][1] = piece[2][1];
            }
            if(temp[0][7] == 3){
                int x = piece[0][0];
                if(x > grid.size() - 3)
                    x = grid.size() - 3;
                temp[0][0] = x;
                temp[0][1] = piece[3][1];
                temp[1][0] = x;
                temp[1][1] = piece[2][1];
                temp[2][0] = x + 1;
                temp[2][1] = piece[2][1];
                temp[3][0] = x + 2;
                temp[3][1] = piece[2][1];
            }
        }
        if(temp[0][6] == 3){
            if(temp[0][7] == 0){
                int y = piece[0][1];
                if(y > grid[0].size() - 3)
                    y = grid[0].size() - 3;
                temp[0][0] = piece[3][0];
                temp[0][1] = y;
                temp[1][0] = piece[2][0];
                temp[1][1] = y;
                temp[2][0] = piece[2][0];
                temp[2][1] = y + 1;
                temp[3][0] = piece[2][0];
                temp[3][1] = y + 2;
            }
            else if(temp[0][7] == 2){
                int y = piece[1][1] - 1;
                if(y < 0)
                    y = 0;
                temp[0][0] = piece[0][0];
                temp[0][1] = y;
                temp[1][0] = piece[2][0];
                temp[1][1] = y;
                temp[2][0] = piece[2][0];
                temp[2][1] = y + 1;
                temp[3][0] = piece[2][0];
                temp[3][1] = y + 2;
            }
        }
    }
    if(temp[0][5] == 6){
        if(temp[0][6] == 0){
            if(temp[0][7] == 3){
                int x = piece[0][0];
                if(x > grid.size() - 3)
                    x = grid.size() - 3;
                temp[0][0] = x + 1;
                temp[0][1] = piece[3][1];
                temp[1][0] = x;
                temp[1][1] = piece[0][1];
                temp[2][0] = x + 1;
                temp[2][1] = piece[0][1];
                temp[3][0] = x + 2;
                temp[3][1] = piece[0][1];
            }
            if(temp[0][7] == 1){
                int x = piece[1][0] - 1;
                if(x < 0)
                    x = 0;
                temp[0][0] = x + 1;
                temp[0][1] = piece[1][1];
                temp[1][0] = x;
                temp[1][1] = piece[2][1];
                temp[2][0] = x + 1;
                temp[2][1] = piece[2][1];
                temp[3][0] = x + 2;
                temp[3][1] = piece[2][1];
            }
        }
        if(temp[0][6] == 1){
            if(temp[0][7] == 0){
                int y = piece[0][1];
                if(y > grid[0].size() - 3)
                    y = grid[0].size() - 3;
                temp[0][0] = piece[3][0];
                temp[0][1] = y + 1;
                temp[1][0] = piece[2][0];
                temp[1][1] = y;
                temp[2][0] = piece[2][0];
                temp[2][1] = y + 1;
                temp[3][0] = piece[2][0];
                temp[3][1] = y + 2;
            }
            else if(temp[0][7] == 2){
                int y = piece[1][1] - 1;
                if(y < 0)
                    y = 0;
                temp[0][0] = piece[1][0];
                temp[0][1] = y + 1;
                temp[1][0] = piece[2][0];
                temp[1][1] = y;
                temp[2][0] = piece[2][0];
                temp[2][1] = y + 1;
                temp[3][0] = piece[2][0];
                temp[3][1] = y + 2;
            }
        }
        else if(temp[0][6] == 2){
            if(temp[0][7] == 1){
                int x = piece[0][0];
                if(x < 2)
                    x = 2;
                temp[0][0] = x - 1;
                temp[0][1] = piece[3][1];
                temp[1][0] = x;
                temp[1][1] = piece[2][1];
                temp[2][0] = x - 1;
                temp[2][1] = piece[2][1];
                temp[3][0] = x - 2;
                temp[3][1] = piece[2][1];
            }
            if(temp[0][7] == 3){
                int x = piece[1][0] + 1;
                if(x > grid.size() - 1)
                    x = grid.size() - 1;
                temp[0][0] = x - 1;
                temp[0][1] = piece[1][1];
                temp[1][0] = x;
                temp[1][1] = piece[2][1];
                temp[2][0] = x - 1;
                temp[2][1] = piece[2][1];
                temp[3][0] = x - 2;
                temp[3][1] = piece[2][1];
            }
        }
        if(temp[0][6] == 3){
            if(temp[0][7] == 0){
                int y = piece[1][1] + 1;
                if(y > grid[0].size() - 1)
                    y = grid[0].size() - 1;
                temp[0][0] = piece[1][0];
                temp[0][1] = y - 1;
                temp[1][0] = piece[2][0];
                temp[1][1] = y;
                temp[2][0] = piece[2][0];
                temp[2][1] = y - 1;
                temp[3][0] = piece[2][0];
                temp[3][1] = y - 2;
            }
            else if(temp[0][7] == 2){
                int y = piece[0][1];
                if(y < 2)
                    y = 2;
                temp[0][0] = piece[3][0];
                temp[0][1] = y - 1;
                temp[1][0] = piece[2][0];
                temp[1][1] = y;
                temp[2][0] = piece[2][0];
                temp[2][1] = y - 1;
                temp[3][0] = piece[2][0];
                temp[3][1] = y - 2;
            }
        }
    }

    for(int i = 0; i < piece.size(); i++){
        grid[piece[i][0]][piece[i][1]][0] = 0;
        grid[piece[i][0]][piece[i][1]][1] = 0;
        grid[piece[i][0]][piece[i][1]][2] = 0;
    }
    for(int i = 0; i < temp.size(); i++){
        if(grid[temp[i][0]][temp[i][1]][0] != 0 && grid[temp[i][0]][temp[i][1]][1] != 0 && grid[temp[i][0]][temp[i][1]][2] != 0){
            temp = piece;
            for(int i = 0; i < temp.size(); i++){
                temp[i][6] = temp[i][7];
            }
        }
    }
    for(int i = 0; i < temp.size(); i++){
        grid[temp[i][0]][temp[i][1]][0] = temp[i][2];
        grid[temp[i][0]][temp[i][1]][1] = temp[i][3];
        grid[temp[i][0]][temp[i][1]][2] = temp[i][4];
    }
    piece = temp;
}

void LineCheck(){
    for(int i = grid[0].size() - 1; i >= 0; i--){
        int fullRow = true;
        for(int j = 0; j < grid.size(); j++){
            if(grid[j][i][0] == 0 && grid[j][i][1] == 0 && grid[j][i][2] == 0){
                fullRow = false;
            }
        }
        if(fullRow){
            ClearLine(i);
            i++;
        }
    }

}

void ClearLine(int line){
    vector<vector<vector<int>>> temp = grid;
    for(int i = 0; i < grid.size(); i++){
        temp[i][line] = {0, 0, 0};
    }
    for(int i = line; i >= 0; i--){
        for(int j = 0; j < grid.size(); j++){
            if(i == 0)
                temp[j][i] = {0, 0, 0};
            else   
                temp[j][i] = temp[j][i-1];
        }
    }
    grid = temp;
}

void TickPiece(){
    vector<vector<int>> temp = piece;
    for(int i = 0; i < temp.size(); i++){
        temp[i][1]++;
    }
    for(int i = 0; i < temp.size(); i++){
        if(temp[i][1] > grid[0].size() - 1){
            temp = piece;
            pieceInPlay = false;
        }
    }
    if(temp != piece){
        for(int i = 0; i < piece.size(); i++){
            grid[piece[i][0]][piece[i][1]][0] = 0;
            grid[piece[i][0]][piece[i][1]][1] = 0;
            grid[piece[i][0]][piece[i][1]][2] = 0;
        }
        for(int i = 0; i < temp.size(); i++){
            if(grid[temp[i][0]][temp[i][1]][0] != 0 && grid[temp[i][0]][temp[i][1]][1] != 0 && grid[temp[i][0]][temp[i][1]][2] != 0){
                temp = piece;
                pieceInPlay = false;
            }
        }
    }
    for(int i = 0; i < temp.size(); i++){
        grid[temp[i][0]][temp[i][1]][0] = temp[i][2];
        grid[temp[i][0]][temp[i][1]][1] = temp[i][3];
        grid[temp[i][0]][temp[i][1]][2] = temp[i][4];
    }
    piece = temp;
}

void Draw(){
    for(int x = 0; x < grid.size(); x++){
        for(int y = 0; y < grid[x].size(); y++){
            pos.x = (x * gridSize) + (screenWidth / 2 - 5 * gridSize) + 1;
            pos.y = y * gridSize + 1;
            pos.w = gridSize - 2;
            pos.h = gridSize - 2;
            SDL_SetRenderDrawColor(renderer, grid[x][y][0], grid[x][y][1], grid[x][y][2], 255);
            SDL_RenderFillRect(renderer, &pos);

            if(grid[x][y][0] != 0 && grid[x][y][1] != 0 && grid[x][y][2] != 0){
                pos.x = (x * gridSize) + (screenWidth / 2 - 5 * gridSize) + 2;
                pos.y = y * gridSize + 2;
                pos.w = ceil(gridSize / 10);
                pos.h = ceil(gridSize / 10);
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderFillRect(renderer, &pos);
                pos.x = (x * gridSize) + (screenWidth / 2 - 5 * gridSize) + 2 + ceil(gridSize / 10);
                pos.y = y * gridSize + 2;
                pos.w = ceil(gridSize / 10);
                pos.h = ceil(gridSize / 10);
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderFillRect(renderer, &pos);
                pos.x = (x * gridSize) + (screenWidth / 2 - 5 * gridSize) + 2;
                pos.y = y * gridSize + 2 + ceil(gridSize / 10);
                pos.w = ceil(gridSize / 10);
                pos.h = ceil(gridSize / 10);
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderFillRect(renderer, &pos);
            }
        }
    }
}

bool GameOverCheck(){
    for(int i = 0; i < piece.size(); i++){
        if(grid[piece[i][0]][piece[i][1]][0] != 0 && grid[piece[i][0]][piece[i][1]][1] != 0 && grid[piece[i][0]][piece[i][1]][2] != 0){
            return true;
        }
    }
    return false;
}
