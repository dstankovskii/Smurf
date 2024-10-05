#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <windows.h>

using namespace sf;

const int H = 26;
const int W = 160;
char TileMap[H][W];
bool amnyam = false;

float mapX = 0, mapY = 0; // Map offset coordinates

class GAMER
{
public:
    float dx, dy, Kdr, Ldr;
    FloatRect rect;
    bool Air;
    Sprite spr;
    GAMER(Texture &image)
    {
        spr.setTexture(image);
        spr.setTextureRect(IntRect(12, 1, 39, 62)); // Setting sprite texture
        rect = FloatRect(40, 350, 39, 62); // Player's position on the map

        dx = 0, dy = 0, Kdr = 0, Ldr = 0;
    }

    void up(float t)
    {
        // Checking for map boundaries
        if (rect.left > 1 && rect.left < 5050) { rect.left += dx*t; }
        else
        {
            if (rect.left >= 5050) { rect.left -= 0.1; }
            if (rect.left <= 1) { rect.left += 0.1; }
        }
        side(0);

        if (Air) { dy = dy + 0.01*(t/20); } // Movement in the air
        rect.top += dy*t;
        Air = true;
        side(1);

        Kdr += 0.01*t;
        if (Kdr > 4) { Kdr -= 4; Ldr += 1; } // Step animation
        if (Ldr > 3) { Ldr -= 4; }

        if (dx > 0) { spr.setTextureRect(IntRect(64*int(Kdr) + 12, 64*int(Ldr), 39, 62)); }
        if (dx < 0) { spr.setTextureRect(IntRect(64*int(Kdr) + 52, 64*int(Ldr), -39, 62)); }

        spr.setPosition(rect.left - mapX, rect.top - mapY);
        dx = 0;
    }

    void side(int Run)
    {
        for (int i = rect.top/32; i < (rect.top + rect.height)/32; i++)
            for (int j = rect.left/32; j < (rect.left + rect.width)/32; j++)
            { 
                // Handling collisions with map objects
                if (TileMap[i][j] == 'B' || TileMap[i][j] == 'A' || TileMap[i][j] == '0') 
                { 
                    if ((dx > 0) && (Run == 0)) { rect.left = j*32 - rect.width; } 
                    if ((dx < 0) && (Run == 0)) { rect.left = (j+1)*32; }
                    if ((dy > 0) && (Run == 1)) { rect.top = i*32 - rect.height; dy = 0; Air = false; }
                    if ((dy < 0) && (Run == 1)) { rect.top = (i+1)*32; dy = 0; }
                }

                // Handling bonus collection
                if (TileMap[i][j] == 'P') { TileMap[i][j] = ' '; amnyam = true; }
            }
    }
};

class BOT
{
public:
    float dx, dy, Kdr;
    int x, y;
    FloatRect rect;
    Sprite spr;

    void set(Texture &image, int x, int y)
    {   
        spr.setTexture(image);
        spr.setTextureRect(IntRect(96, 192, 32, 32));
        rect = FloatRect(x*32, y*32, 32, 32);

        dx = 0.05;
        Kdr = 0;
    }

    void up(float t)
    {
        rect.left += dx*t;
        side();
        Kdr += t * 0.005;
        if (Kdr > 3) Kdr -= 3;

        // Bot movement animation
        if (dx > 0) { spr.setTextureRect(IntRect(96 + 32*int(Kdr), 192, 32, 32)); }
        if (dx < 0) { spr.setTextureRect(IntRect(288 + 32*int(Kdr), 192, 32, 32)); }
        spr.setPosition(rect.left - mapX, rect.top - mapY);
    }

    void side()
    {
        for (int i = rect.top/32; i < (rect.top + rect.height)/32; i++)
            for (int j = rect.left/32; j < (rect.left + rect.width)/32; j++)
                if ((TileMap[i][j] == 'B') || (TileMap[i][j] == '0') || (TileMap[i][j] == 'Z'))
                { 
                    // Bot direction change logic upon collisions with obstacles
                    if (dx > 0) { rect.left = j*32 - rect.width; dx *= -1; }
                    else if (dx < 0) { rect.left = j*32 + 32; dx *= -1; }
                }
    }
};

int main()
{
    setlocale(LC_ALL, "Russian");
    printf ("\n RIGHT - move right\n");
    printf (" LEFT - move left\n");
    printf (" UP - jump\n");
    printf (" DOWN - crouch\n");
    printf ("\n Good luck!\n");
    printf (" Press ENTER to exit the game\n");
    getchar();
    
    // Loading the map from file
    FILE *myfile = fopen("map.txt", "r");
    for (int i = 0; i < H; i++)
        fgets(TileMap[i], 162, myfile);
    fclose(myfile);

    RenderWindow window(VideoMode(1000, 512), "smurf_master");
    
    // Loading music
    Music music1, music2;
    music1.openFromFile("ac_dc.ogg");
    music2.openFromFile("get_back.ogg");
    music1.play();

    // Loading sounds
    SoundBuffer sound1, sound2, sound3, sound4;
    sound1.loadFromFile("foot.ogg");
    sound2.loadFromFile("down.ogg");
    sound3.loadFromFile("chavk.ogg");
    sound4.loadFromFile("cry.ogg");

    Sound foot(sound1);
    Sound down(sound2);
    Sound chavk(sound3);
    Sound cry(sound4);

    // Loading textures
    Texture tex1, tex2, tex3, tex4, tex5, tex6;
    tex1.loadFromFile("image.png");
    tex2.loadFromFile("background.jpg");
    tex3.loadFromFile("texture.png");
    tex4.loadFromFile("bonus.png");
    tex5.loadFromFile("monsters.png");
    tex6.loadFromFile("pikap.jpg");

    Sprite textur, background, monsters, bonus, theEnd;
    theEnd.setTexture(tex6);
    background.setTexture(tex2);
    background.setPosition(-5, 0);
    textur.setTexture(tex3);
    bonus.setTexture(tex4);
    monsters.setTexture(tex5);

    GAMER smurf(tex1);

    // Bot initialization
    BOT bot[9];
    int e = 0;
    for (int i = 0; i < H; i++)
        for (int j = 0; j < W; j++)
            if (TileMap[i][j] == 'M') { TileMap[i][j] = ' '; bot[e].set(tex5, j, i); e++; }

    bool fullbonus;
    // Sound control (cries, falls)
    bool Rcry = false, Rdown = true;

    Clock time;

    RectangleShape tile(Vector2f(32, 32));

    while (window.isOpen())
    {
        float t = time.getElapsedTime().asMicroseconds();
        time.restart();
        t = t/600;

        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }

        // Player control logic
        if (!((Keyboard::isKeyPressed(Keyboard::Left)) && (Keyboard::isKeyPressed(Keyboard::Right))))
        {
            if (Keyboard::isKeyPressed(Keyboard::Down))
            {
                if (Keyboard::isKeyPressed(Keyboard::Right)) { smurf.dx = 0.35; }
                if (Keyboard::isKeyPressed(Keyboard::Left)) { smurf.dx = -0.35; }
            }
            else
            {
                if (Keyboard::isKeyPressed(Keyboard::Left)) { smurf.dx = -0.1; }
                if (Keyboard::isKeyPressed(Keyboard::Right)) { smurf.dx = 0.1; }
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Up) && !(Keyboard::isKeyPressed(Keyboard::Down)))
        {
            if (!smurf.Air) { smurf.dy = -0.42; smurf.Air = true; foot.play(); } // Jump
        }

        smurf.up(t);
        for (e = 0; e < 9; e++) { bot[e].up(t); }

        if (amnyam == true) { chavk.play(); amnyam = false; }

        if (smurf.dy != 0) { Rdown = true; }
        if (!smurf.Air && smurf.dy == 0 && Rdown == true) { down.play(); Rdown = false; }

        // Checking for bot collisions
        for (e = 0; e < 9; e++)
            if  ((bot[e].rect.intersects(smurf.rect)) && Rcry == false) 
            { 
                cry.play(); 
                Rcry = true; 
                smurf.rect.left = 40; 
                mapX = 1; 
                smurf.rect.top = 450; 
            }

        // Checking for falling
        if ((smurf.rect.top) > 763 && Rcry == false) 
        { 
            cry.play(); 
            Rcry = false; 
            smurf.rect.top = 450; 
            smurf.rect.left = 40; 
            mapX = 1; 
        }
        Rcry = false;

        // Map offset control
        if ((smurf.rect.left > 1000/3) && (smurf.rect.left < 4430)) { mapX = smurf.rect.left - 1000/3; }
        mapY = 224 + 3*32;

        // Checking for collecting all bonuses
        fullbonus = true;
        for (int i = 0; i < H; i++)
            for (int j = 0; j < W; j++)
                if (TileMap[i][j] == 'P')
                    fullbonus = false;

        // End the game if conditions are met
        if (((smurf.rect.left) > 5049) && (smurf.rect.top > 700) && (fullbonus == true)) 
        { 
            music1.stop(); 
            music2.play();
            Sleep(1000);
            window.draw(theEnd);
            window.display();
            Sleep(7000); 
            break; 
        }

        // Drawing game objects
        window.clear(Color::Black);
        window.draw(background);

        for (int i = 0; i < H; i++)
            for (int j = 0; j < W; j++)
            {   
                if (TileMap[i][j] == 'B') { textur.setTextureRect(IntRect(175, 143, 32, 32)); textur.setPosition(j*32 - mapX, i*32 - mapY); window.draw(textur); }
                if (TileMap[i][j] == 'A') { textur.setTextureRect(IntRect(736, 160, 96, 96)); textur.setPosition(j*32 - mapX, i*32 - mapY); window.draw(textur); }
                if (TileMap[i][j] == 'P') { bonus.setTextureRect(IntRect(15, 13, 38, 38)); bonus.setPosition(j*32 - mapX, i*32 - mapY); window.draw(bonus); }
                if ((TileMap[i][j] == ' ') || (TileMap[i][j] == '0') || (TileMap[i][j] == 'Z')) { continue; }
            }

        window.draw(smurf.spr);
        for (e = 0; e < 9; e++) 
            window.draw(bot[e].spr);
        window.display();
    }

    return 0;
}
