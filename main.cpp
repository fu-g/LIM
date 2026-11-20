#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include <fstream>

int LEVEL_WIDTH = 1250;
int LEVEL_HEIGHT = 650;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
 
const int SPRITE_BLOCK_WIDTH = 24;
const int SPRITE_BLOCK_HEIGHT = 24;
const int BLOCK_WIDTH = 50;
const int BLOCK_HEIGHT = 50;
int TOTAL_BLOCKS = 325;
int PREV_TOTAL_BLOCKS = 325;
const int TOTAL_BLOCK_SPRITE = 15;

const int SPRITE_SLIME_WIDTH = 36;
const int SPRITE_SLIME_HEIGHT = 36;
const int TOTAL_SLIME_SPRITE = 5;
const int TOTAL_PARTICLE = 435;

const int cAppearance = 30;
int frameTimer = 0;

const int SCREEN_FPS = 60;
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;
Uint32 prevTicks = SDL_GetTicks();
Uint32 transTime = SDL_GetTicks();
Uint32 arrowDelay = SDL_GetTicks();

const int TOTAL_TEXT = 10;
const int TOTAL_LAUNCHARROW = 10;

const int TOTAL_BUTTON_SPRITE = 2;

bool isJumping = true;
bool liquid = false;
bool out = false;
bool clear = false;
bool die = false;
bool stageChanging = false;
int deathCount = 0;
int currentStage = 0;

enum BlockType
{
    dark,
    sand,
    blockTop,
    blockLeft,
    blockRight,
    block,
    blockBottom,
    blockTR,
    blockTL,
    trap,
    cornerL,
    cornerR,
    blockBL,
    blockBR,
    air,
};

enum SlimeMotion
{
    BASIC0,
    BASIC1,
    BASIC2,
    JUMP,
    JUMPING,
};

enum ButtonStatus
{
    NORMAL,
    PUSH,
};

enum Stage
{
    OPENING,
    STAGE1,
    STAGE2,
    STAGE3,
    STAGE4,
    STAGE5,
    STAGE6,
    STAGE7,
    STAGE8,
    GAMECLEAR,
};

class LTexture
{
public:
    LTexture();
    ~LTexture();

    // 下記二つを呼び出す
    bool loadFromFile(std::string path);
    // サーフェスからピクセル
    bool loadPixelsFromFile(std::string path);
    // ピクセルからテクスチャ
    bool loadFromPixels();
    bool loadFromRenderedText( std::string textureText, SDL_Color textColor );
    bool createBlank(int width, int height, SDL_TextureAccess access);
    void free();
    void setColor(Uint8 red, Uint8 green, Uint8 blue);
    void setBlendMode(SDL_BlendMode blending);
    void setAlpha(Uint8 alpha);
    void render(int x, int y, SDL_Rect *clip = NULL, float angle = 0.0, SDL_Point *center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE, int sizeW = -1, int sizeH = -1);
    void setAsRenderTarget();
    int getWidth();
    int getHeight();
    Uint32 *getPixels32();
    Uint32 getPixel32(Uint32 x, Uint32 y);
    Uint32 getPitch32();
    void copyRawPixels32(void *pixels);
    bool lockTexture();
    bool unlockTexture();
    void sizeChange(int w, int h);

private:
    SDL_Texture *mTexture;
    SDL_Surface *mSurfacePixels;
    void *mRawPixels;
    int mRawPitch;
    int mWidth;
    int mHeight;
};

class Block
{
public:
    Block(int x, int y, int blockType);
    void render(SDL_Rect &camera);
    int getType();
    SDL_Rect getBox();
    void setNear(bool near);
    bool getNear();

private:
    SDL_Rect mBox;
    int mType;
    bool nearSlime;
};

class Slime
{
public:
    static const int SLIME_VEL = 2;
    const float G = 0.25;
    Slime();
    void handleEvent(SDL_Event &e);
    void initPos();
    void setPos(SDL_Rect posBox, Block *blocks[], int d);
    void move(Block *blocks[]);
    void setCamera(SDL_Rect &camera);
    void render(SDL_Rect &camera);
    void jump();
    void basicMotion(int& currentFrame, int& totalFrames);
    bool specialMotion();
    bool isOnGround(Block *blocks[]);
    SDL_Rect getBox();
    bool getDir();
    void setDir(bool right);
    void reset();
    void dead();

private:
    SDL_Rect mBox;
    float mVelX, mVelY;
    int mType;
    // jumpの開始時判定
    bool sJump;
    bool mRight;
    bool keyDownR;
    bool keyDownL;
     
};

class Liquid
{
    public:
        static const int LIQUID_VEL = 1;
        const float bloG = 1;
        Liquid(int x, int y, Uint8 red, Uint8 green, Uint8 blue, bool core, int i);
        void setPos(SDL_Rect posBox, SDL_Rect &camera, bool right);
        void handleEvent(SDL_Event &e, Block *blocks[]);
        void vecOpe();
        //futureでは粒子同士の衝突判定を行わない(壁にぶつかる粒子だけ判定)
        void futureMove(Block *blocks[]);
        void currentMove(Block *blocks[], Liquid *particles[]);
        void setCamera(SDL_Rect &camera);
        void render(SDL_Rect &camera);
        SDL_Rect getFutureBox();
        SDL_Rect getCurrentBox();
        bool getNextBox();
        int getDir();
        int getVel();
        bool getBorder();
        void setNextBox();
        void setStop(bool stop);
        void setBorder(bool bo);
        bool isOn(Block *blocks[], Liquid *particles[], int d);
        bool isOut(SDL_Rect box, Block *blocks[], int d);
        bool control(Block *blocks[], int d);
        void reset();

    private:
        SDL_Rect currentBox;
        SDL_Rect futureBox;
        int initPosX;
        int initPosY;
        float mVelR, mVelD;
        Uint8 mR;
        Uint8 mG;
        Uint8 mB;
        bool core;
        bool parJum;
        int index;
        bool keyDownU;
        bool keyDownR;
        bool keyDownD;
        bool keyDownL;
        bool nextBox;
        bool stop;
        int directionG;
        bool stick;
        bool stopping;
        bool border;
        enum dir
        {
            UP,
            RIGHT,
            DOWN,
            LEFT
        };
        int corner;
        enum co
        {
            topLeft,
            topRight,
            bottomRight,
            bottomLeft,
            na
        };
};

class Text
{
    public:
        Text();
        void render(SDL_Rect &camera, int i);
        void setXY(int x, int y);

    private:
        int mx;
        int my;
};

class DownWall
{
    public:
        DownWall();
        void render(SDL_Rect &camera);
        void setPos(int x, int y);
        SDL_Rect getBox();
        void down();
        void clearObje();
        void setUp(bool uping);
        void reset();

    private:
        SDL_Rect mBox;
        int countY;
        bool up;
        int initx;
        int inity;

};

class Button
{
    public:
        Button();
        void render(SDL_Rect &camera);
        void setPos(int x, int y);
        void setRender(bool render);
        bool getRender();
        SDL_Rect getBox();
        bool getPushing();
        void push(Slime slime);
        void clearObje();
        void reset();

    private:
        SDL_Rect mBox;
        SDL_Rect pushingBox;
        bool doDraw;
        bool pushing;
        int mType;
};

class Arrow
{
    public:
        Arrow();
        void render(SDL_Rect &camera);
        void setPos();
        void launch(Block *blocks[]);
        SDL_Rect getBox();
        void clearObje();
        int getAlpha();
        void setIndex(int i);
        bool getDisa();
        void setDelay(bool wait, int ms);
        void reset();
        void delay();
        bool getWaiting();

    private:
        SDL_Rect mBox;
        SDL_Rect colliBox;
        //アルファ値(マイナスの値も使うのでUint8にしない)
        int a;
        int index;
        int flightDistance;
        bool disappear;
        bool waiting;
        int waitms;
        bool initwait;
};

class Launcher
{
    public:
        Launcher();
        void render(SDL_Rect &camera);
        void setPos(int x, int y);
        void setRender(bool render);
        bool getRender();
        SDL_Rect getBox();
        void clearObje();
        void setDir(int d);
        int getDir();

    private:
        SDL_Rect mBox;
        bool doDraw;
        int dir;
};

class Curtain
{
    public:
        Curtain();
        void render();
        void closeCurtain(Slime &slime, Block *blocks[]);
        void reset();
        bool getChangePoint();

    private:
        SDL_Rect mBox;
        bool changePoint;
};

bool init();
bool loadMedia();
void close(Block *blocks[]);
bool opening(Block *blocks[], SDL_Event &e);
bool checkCollision(SDL_Rect a, SDL_Rect b, int type = -1);
bool touchesWall(SDL_Rect box, Block *blocks[], bool arrowcolli = false, bool slimeclli = false, bool outCheck = false);
bool touchesParticle(SDL_Rect box, Liquid *particles[], int index, bool nextBox = false);
bool touchesArrorw(SDL_Rect pbox);
bool setBlocks(Block *blocks[], int stage);
void setParticles(Liquid *particles[]);
void sortParticles(Liquid *particles[]);
void clipSlime();
void clipButton();
bool allNextBox(Liquid *particles[], int d);
void isBorder(Liquid *particles[]);
void debug(Liquid *particles[]);
bool timeStop();
void nextStage(Block *blocks[], Slime &slime, Curtain &curtain, int *textX, int *textY);
void checkNearbyBlocks(SDL_Rect sbox, Block *blocks[], int index);

SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;

LTexture gSlimeTexture;
LTexture gBlockTexture;
LTexture gBgTexture;
LTexture gTextTexture[TOTAL_TEXT];
LTexture gDownWallTexture;
LTexture gButtonTexture;
LTexture gArrowTexture;
LTexture gLauncherTexture;
LTexture gCurtainTexture;

DownWall downwall[2];
Button button[2];
Arrow arrow[TOTAL_LAUNCHARROW];
Launcher launcher[TOTAL_LAUNCHARROW];

SDL_Rect gSlimeClips[TOTAL_SLIME_SPRITE];
SDL_Rect gBlockClips[TOTAL_BLOCK_SPRITE];
SDL_Rect gButtonClips[TOTAL_BUTTON_SPRITE];

TTF_Font *titleFont = NULL;
TTF_Font *instFont = NULL;

LTexture::LTexture()
{
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;

    mSurfacePixels = NULL;
    mRawPixels = NULL;
    mRawPitch = 0;
}

LTexture::~LTexture()
{
    free();
}

bool LTexture::loadFromFile(std::string path)
{
    // ピクセル作成
    if (!loadPixelsFromFile(path))
    {
        printf("Failed to load pixels for %s!\n", path.c_str());
    }
    else
    {
        // テクスチャ作成
        if (!loadFromPixels())
        {
            printf("Failed to texture from pixels from %s!\n", path.c_str());
        }
    }
    return mTexture != NULL;
}

bool LTexture::loadPixelsFromFile(std::string path)
{
    free();
    SDL_Surface *loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL)
    {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
    }
    else
    {
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0xFF, 0xFF, 0xFF));
        mSurfacePixels = SDL_ConvertSurfaceFormat(loadedSurface, SDL_GetWindowPixelFormat(gWindow), 0);
        if (mSurfacePixels == NULL)
        {
            printf("Unable to convert loaded surface to display format! SDL Error: %s\n", SDL_GetError());
        }
        else
        {
            mWidth = mSurfacePixels->w;
            mHeight = mSurfacePixels->h;
        }
        // surfaceからピクセルを読み込んだので消去
        SDL_FreeSurface(loadedSurface);
    }
    return mSurfacePixels != NULL;
}

bool LTexture::loadFromPixels()
{
    if (mSurfacePixels == NULL)
    {
        printf("No pixels loaded!");
    }
    else
    {
        mTexture = SDL_CreateTextureFromSurface(gRenderer, mSurfacePixels);
        if (mTexture == NULL)
        {
            printf("Unable to create texture from loaded pixels! SDL Error: %s\n", SDL_GetError());
        }
        else
        {
            mWidth = mSurfacePixels->w;
            mHeight = mSurfacePixels->h;
        }
    }
    return mTexture != NULL;
}

bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor)
{
    free();
    SDL_Surface* textSurface;
    if(currentStage == OPENING)
    {
        textSurface = TTF_RenderText_Solid(titleFont, textureText.c_str(), textColor);
    }
    else
    {
        textSurface = TTF_RenderText_Solid(instFont, textureText.c_str(), textColor);
    }
    
    if(textSurface == NULL)
    {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
    }
    else
    {
        mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
        if(mTexture == NULL)
        {
            printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        }
        else
        {
            mWidth = textSurface->w;
            mHeight = textSurface->h;
        }

        SDL_FreeSurface(textSurface);
    }
    
    return mTexture != NULL;
}

void LTexture::free()
{
    if (mTexture != NULL)
    {
        SDL_DestroyTexture(mTexture);
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
    }

    if (mSurfacePixels != NULL)
    {
        SDL_FreeSurface(mSurfacePixels);
        mSurfacePixels = NULL;
    }
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
    SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha)
{
    SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(int x, int y, SDL_Rect *clip, float angle, SDL_Point *center, SDL_RendererFlip flip, int sizeW, int sizeH)
{
    SDL_Rect renderQuad = {x, y, mWidth, mHeight};
    if (clip != NULL)
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }
    if((sizeW != -1) && (sizeH != -1))
    {
        renderQuad.w = sizeW;
        renderQuad.h = sizeH;
    }


    SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth()
{
    return mWidth;
}

int LTexture::getHeight()
{
    return mHeight;
}

Uint32 LTexture::getPixel32( Uint32 x, Uint32 y )
{
	Uint32* pixels = static_cast<Uint32*>( mSurfacePixels->pixels );
    
	//Get the pixel requested
	return pixels[ ( y * getPitch32() ) + x ];
}

Uint32 LTexture::getPitch32()
{
	Uint32 pitch = 0;

	if( mSurfacePixels != NULL )
	{
		pitch = mSurfacePixels->pitch / 4;
	}

	return pitch;
}


void LTexture::sizeChange(int w, int h)
{
    mWidth = w;
    mHeight = h;
}

Block::Block(int x, int y, int blockType)
{
    mBox.x = x;
    mBox.y = y;
    mBox.w = BLOCK_WIDTH;
    mBox.h = BLOCK_HEIGHT;
    mType = blockType;
    nearSlime = false;
}

void Block::render(SDL_Rect &camera)
{
    if(checkCollision(camera, mBox))
    {
        if(mType == dark)
        {
            gBlockTexture.setAlpha(220);
        }
        else
        {
            gBlockTexture.setAlpha(255);
        }
        gBlockTexture.render(mBox.x - camera.x, mBox.y - camera.y, &gBlockClips[mType], 0.0, NULL, SDL_FLIP_NONE, BLOCK_WIDTH, BLOCK_HEIGHT);
    }
}

int Block::getType()
{
    return mType;
}

SDL_Rect Block::getBox()
{
    return mBox;
}

void Block::setNear(bool near)
{
    nearSlime = near;
}

bool Block::getNear()
{
    return nearSlime;
}


Slime::Slime()
{

    mBox.x = 0;
    mBox.y = 0 ;
    mBox.w = 30;
    mBox.h = 36 - 18;

    mVelX = 0;
    mVelY = 0;
    mRight = true;
    mType = 0;
    sJump = false;

    keyDownR = false;
    keyDownL = false;
}

void Slime::handleEvent(SDL_Event &e)
{
    if(!stageChanging)
    {
        if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
        {
            switch (e.key.keysym.sym)
            {
                case SDLK_UP:
                    if (!isJumping && !sJump)
                    {
                        frameTimer = 0;
                        sJump = true;
                        mType = 3;
                    }
                    break;

                case SDLK_LEFT:
                    if(!keyDownL)
                    {
                        keyDownL = true;
                        mVelX -= SLIME_VEL;
                    }
                    break;

                case SDLK_RIGHT:
                    if(!keyDownR)
                    {
                        keyDownR = true;
                        mVelX += SLIME_VEL;
                    }
                    break;

                case SDLK_z:
                    if(!isJumping)
                    {
                        liquid = true;
                        transTime = SDL_GetTicks();
                    }
                    break;
            }
        }
        else if (e.type == SDL_KEYUP && e.key.repeat == 0)
        {
            switch (e.key.keysym.sym)
            {
                case SDLK_LEFT:
                    if(keyDownL)
                    {
                        keyDownL = false;
                        mVelX += SLIME_VEL;
                    }
                    break;
                case SDLK_RIGHT:
                    if(keyDownR)
                    {
                        keyDownR = false;
                        mVelX -= SLIME_VEL;
                    }
                    break;
            }
        }
    }
}

void Slime::initPos()
{
    switch (currentStage)
    {
    case STAGE1:
        mBox.x = 10;
        mBox.y = 400;
        break;
    
    case STAGE2:
        mBox.x = 10;
        mBox.y = 250;
        break;

    case STAGE3:
        mBox.x = 10;
        mBox.y = 1000;
        break;

    case STAGE4:
        mBox.x = 10;
        mBox.y = 800;
        break;

    case STAGE5:
        mBox.x = 10;
        mBox.y = 400;
        break;

    case STAGE6:
        mBox.x = 10;
        mBox.y = 200;
        break;

    case STAGE7:
        mBox.x = 10;
        mBox.y = 450;
        break;

    case STAGE8:
        mBox.x = 10;
        mBox.y = 300;

    case GAMECLEAR:
        mBox.x = 10;
        mBox.y = 250;
        break;
    }
}

void Slime::setPos(SDL_Rect posBox, Block *blocks[], int d)
{
    enum dir
    {
        UP,
        RIGHT,
        DOWN,
        LEFT
    };

    switch (d)
    {
    case UP:
        if(mRight)
        {
            mBox.x = posBox.x;
        }
        else
        {
            mBox.x = posBox.x - 18;
        }
        mBox.y = posBox.y + 8;
        if(touchesWall(mBox, blocks, false, true))
        {
            mBox.x += 15;
        }

        if(touchesWall(mBox, blocks, false, true))
        {
            mBox.x -= 30;
        }
        break;

    case RIGHT:
        if(mRight)
        {
            mBox.x = posBox.x - 15;
        }
        else
        {
            mBox.x = posBox.x - 30;
        }
        mBox.y = posBox.y;

        if(touchesWall(mBox, blocks, false, true))
        {
            mBox.y += 5;
        }

        if(touchesWall(mBox, blocks, false, true))
        {
            mBox.y -= 10;
        }
        break;

    case DOWN:
        if(mRight)
        {
            mBox.x = posBox.x - 7;
        }
        else
        {
            mBox.x = posBox.x - 23;
        }
        mBox.y = posBox.y - 3;
        if(touchesWall(mBox, blocks, false, true))
        {
            mBox.x += 10;
        }

        if(touchesWall(mBox, blocks, false, true))
        {
            mBox.x -= 20;
        }
        break;

    case LEFT:
        if(mRight)
        {
            mBox.x = posBox.x + 1;
        }
        else
        {
            mBox.x = posBox.x - 13;
        }
        mBox.y = posBox.y;
        if(touchesWall(mBox, blocks, false, true))
        {
            mBox.y += 5;
        }

        if(touchesWall(mBox, blocks, false, true))
        {
            mBox.y -= 10;
        }
        break;

    }
}

void Slime::move(Block *blocks[])
{
    //x
    if(!sJump)
    {
        mBox.x += mVelX;
    }
    
    if ((mBox.x < 0) || (mBox.x + mBox.w > LEVEL_WIDTH) || touchesWall(mBox, blocks, false, true))
    {
        mBox.x -= mVelX;
    }


    //y

    //空中にいるかどうか
    isJumping = isOnGround(blocks) ? false : true;

    if(isJumping)
    {
        mVelY += G;
    }

    if(!sJump)
    {
        mBox.y += mVelY;
    }

    //衝突判定    
    if ((mBox.y < 0) || touchesWall(mBox, blocks, false, true))
    {
        mBox.y -= mVelY;
        mVelY = 0;
    }

    SDL_Rect characterTop = { mBox.x, mBox.y, mBox.w, 1 };
    if(touchesWall(characterTop, blocks, false, true))
    {
        mBox.y += 1;
    }

    //落下死
    if((mBox.y + mBox.h / 2 > LEVEL_HEIGHT))
    {
        initPos();
    }

    //落下速度上限
    if (mVelY > 7)
    {
        mVelY = 7;
    }
    
    //向きの反転
    if (mVelX > 0)
    {
        mRight = true;
    }
    else if (mVelX < 0)
    {
        mRight = false;
    }
    
}

void Slime::setCamera(SDL_Rect &camera)
{
    camera.x = (mBox.x + 18 + 30 / 2) - SCREEN_WIDTH / 2;
	camera.y = (mBox.y + 18 / 2) - SCREEN_HEIGHT / 2;

	//Keep the camera in bounds
	if( camera.x < 0 )
	{ 
		camera.x = 0;
	}
	if( camera.y < 0 )
	{
		camera.y = 0;
	}
	if( camera.x > LEVEL_WIDTH - camera.w )
	{
		camera.x = LEVEL_WIDTH - camera.w;
	}
	if( camera.y > LEVEL_HEIGHT - camera.h )
	{
		camera.y = LEVEL_HEIGHT - camera.h;
	}
}

void Slime::render(SDL_Rect &camera)
{
    //左右反転
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (!mRight)
    {
        flip = SDL_FLIP_HORIZONTAL;
    }

    gSlimeTexture.render(mBox.x - 3 - camera.x, mBox.y - 18 - camera.y, &   gSlimeClips[mType], 0.0, NULL, flip);
    

}

void Slime::jump()
{
    mVelY = -5;
    isJumping = true;
}

void Slime::basicMotion(int& currentFrame, int& totalFrames)
{
    if (currentFrame == 2)
    {
        mType = 0;
    }
    else if (currentFrame == 3)
    {
        mType = 2;
    }
    else
    {
        mType = currentFrame;
    }

    if (frameTimer >= cAppearance)
    {
        currentFrame++;
        if (currentFrame >= totalFrames)
        {
            currentFrame = 0;
        }
        if (currentFrame == 2)
        {
            mType = 0;
        }
        else if (currentFrame == 3)
        {
            mType = 2;
        }
        else
        {
            mType = currentFrame;
        }
        frameTimer = 0;
    }

}

bool Slime::specialMotion()
{
    bool action = false;
    if(sJump)
    {
        mType = 3;
        action = true;
        if(frameTimer >= cAppearance)
        {
            sJump = false;
            jump();
            frameTimer = 0;
        }
    }

    if(isJumping && !sJump)
    {
        mType = 4;
        action = true;
        if(frameTimer >= cAppearance)
        {
            frameTimer = 0;
        }
    }

    return action;
}

bool Slime::isOnGround(Block* blocks[])
{
    SDL_Rect characterBottom = { mBox.x, mBox.y + mBox.h, mBox.w, 1 }; // キャラクターの底辺を表す矩形
    if(touchesWall(characterBottom, blocks, false, true))
    {
        return true;
    }
    return false;
}

SDL_Rect Slime::getBox()
{
    return mBox;
}

void Slime::reset()
{
    mVelX = 0;
    keyDownL = false;
    keyDownR = false;
}

bool Slime::getDir()
{
    return mRight;
}

void Slime::setDir(bool right)
{
    mRight = right;
}

void Slime::dead()
{
    initPos();
    ++deathCount;
}


Liquid::Liquid(int x, int y, Uint8 red, Uint8 green, Uint8 blue, bool eye, int i)
{
    currentBox = {x, y, 1, 1};
    futureBox = currentBox;
    initPosX = x;
    initPosY = y;
    mVelR = 0;
    mVelD = 0;
    mR = red;
    mG = green;
    mB = blue;
    core = eye;
    parJum = true;
    index = i;
    keyDownU = false;
    keyDownR = false;
    keyDownD = false;
    keyDownL = false;
    nextBox = false;
    stop = false;
    directionG = DOWN;
    corner = na;
    stick = false;
    border = false;
}

void Liquid::setPos(SDL_Rect slimeBox, SDL_Rect &camera, bool right)
{
    currentBox.x = initPosX;
    currentBox.y = initPosY;
    if(!right)
    {
        currentBox.x = 36 - currentBox.x;
    }
    currentBox.x += slimeBox.x - 3;
    currentBox.y += slimeBox.y - 18;
    futureBox = currentBox;
    stopping = true;
}

void Liquid::handleEvent(SDL_Event &e, Block *blocks[])
{
    //変身中は少し止める
    if(stopping)
    {
        stopping = timeStop();
    }
    
    if(!stopping)
    {
        if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
        {
            switch (e.key.keysym.sym)
            {
                case SDLK_UP:
                    if(!keyDownR && !keyDownL)
                    {
                        if(directionG == LEFT || corner == bottomLeft)
                        {
                            corner = na;
                            directionG = LEFT;
                            keyDownU = true;
                            mVelR -= LIQUID_VEL;
                        }
                        else if(directionG == RIGHT || corner == bottomRight)
                        {
                            corner = na;
                            directionG = RIGHT;
                            keyDownU = true;
                            mVelR += LIQUID_VEL;
                        }
                    }

                    break;
                case SDLK_DOWN:

                    if(!keyDownR && !keyDownL)
                    {
                        if(directionG == LEFT || corner == topLeft)
                        {
                            corner = na;
                            directionG = LEFT;
                            keyDownD = true;
                            mVelR += LIQUID_VEL;
                        }
                        else if(directionG == RIGHT || corner == topRight)
                        {
                            corner = na;
                            directionG = RIGHT;
                            keyDownD = true;
                            mVelR -= LIQUID_VEL;
                        }
                    }

                    break;
                case SDLK_LEFT:
                    if(!keyDownD && !keyDownU)
                    {
                        corner = na;
                        keyDownL = true;
                        mVelR -= LIQUID_VEL;
                        if(corner == topRight) directionG = UP;
                    }

                    break;
                case SDLK_RIGHT:
                    if(!keyDownD && !keyDownU)
                    {
                        corner = na;
                        keyDownR = true;
                        mVelR += LIQUID_VEL;
                        if(corner == topLeft) directionG = UP;
                    }

                    break;
                case SDLK_z:
                    liquid = false;
                    break;
            }
        }
        else if (e.type == SDL_KEYUP && e.key.repeat == 0)
        {
            switch (e.key.keysym.sym)
            {
                case SDLK_UP:
                    if(keyDownU)
                    {
                        if(mVelR < 0)
                        {
                            keyDownU = false;
                            mVelR += LIQUID_VEL;
                        }
                        else if(mVelR > 0)
                        {
                            keyDownU = false;
                            mVelR -= LIQUID_VEL;
                        }
                    }


                    break;
                case SDLK_DOWN:
                    if(keyDownD)
                    {
                        if(mVelR < 0)
                        {
                            keyDownD = false;
                            mVelR += LIQUID_VEL;
                        }
                        else if(mVelR > 0)
                        {
                            keyDownD = false;
                            mVelR -= LIQUID_VEL;
                        }

                    }


                    break;
                case SDLK_LEFT:
                    if(keyDownL)
                    {
                        if(mVelR != 0)
                        {

                            mVelR += LIQUID_VEL;   
                        }
                        keyDownL = false;
                    }

                    break;
                case SDLK_RIGHT:
                    if(keyDownR)
                    {
                        if(mVelR != 0)
                        {

                            mVelR -= LIQUID_VEL;
                        }
                        keyDownR = false;
                    }

                    break;
            }
        }
    }
    
}

void Liquid::vecOpe()
{
    if(keyDownD && directionG == DOWN)
    {
        if(mVelR > 0)
        {
            corner = bottomLeft;
            mVelR -= LIQUID_VEL;
        }
        else if(mVelR < 0)
        {
            corner = bottomRight;
            mVelR += LIQUID_VEL;
        }
        keyDownD = false;
    }
    else if(keyDownU && directionG == UP)
    {
        if(mVelR > 0)
        {
            corner = topRight;
            mVelR -= LIQUID_VEL;
        }
        else if(mVelR < 0)
        {
            corner = topLeft;
            mVelR += LIQUID_VEL;
        }
        keyDownU = false;
    }
    
}

void Liquid::futureMove(Block *blocks[])
{
    vecOpe();

    //先に進行方向に動かしておき、壁との衝突判定だけ行う
    if(border)
    {
        if(directionG == DOWN || directionG == UP)
        {
                futureBox.x += mVelR;
                if(mVelR != 0)
                {
                    if(touchesWall(futureBox, blocks, false, true))
                    {
                        nextBox = true;
                        futureBox.x -= mVelR;
                    }
                    else
                    {
                        nextBox = false;
                    }
                }

                if(isOut(futureBox, blocks, directionG))
                {
                    out = true;
                }

        }
        else if(directionG == RIGHT)
        {
            futureBox.y -= mVelR;
            if(mVelR != 0)
            {   
                if(touchesWall(futureBox, blocks, false, true))
                {
                    nextBox = true;
                    futureBox.y += mVelR;
                }
                else
                {
                    nextBox = false;
                }
            }

            if(isOut(futureBox, blocks, directionG))
            {
                out = true;
            }
        }
        else if(directionG == LEFT)
        {
            futureBox.y += mVelR;
            if(mVelR != 0)
            {
                if(touchesWall(futureBox, blocks, false, true))
                {
                    nextBox = true;
                    futureBox.y -= mVelR;
                }
                else
                {
                    nextBox = false;
                }
            }

            if(isOut(futureBox, blocks, directionG))
            {
                out = true;
            }
        }
    }

}

void Liquid::currentMove(Block *blocks[], Liquid *particles[])
{
    
    //g
    parJum = isOn(blocks, particles, directionG) ? false : true;
    if(parJum)
    {
        mVelD += bloG;
        
    }

    if (mVelD > 1)
    {
        mVelD = 1;
    }

    //right, left

    
    /*ブロックの外側を直角に曲がる
    if(directionG == DOWN || directionG == UP)
    {
        if(isOut(blocks, directionG))
        {
            cliff = true;
            control(blocks, directionG);
            if(stick)
            {
                directionG = RIGHT;
                printf("a");
            }
        }
    }
    */
   
    if(directionG == DOWN || directionG == UP)
    {
        currentBox.x += mVelR;
        if(mVelR != 0)
        {
            if(nextBox || out)
            {
                currentBox.x -= mVelR;
                if(allNextBox(particles, directionG))
                {
                    if(mVelR < 0 && control(blocks, directionG) && corner != topLeft)
                    {
                        if(directionG == DOWN) corner = bottomLeft;
                        if(directionG == UP) corner = topLeft; 
                        directionG = LEFT;
                    }
                    else if(mVelR > 0 && control(blocks, directionG) && corner != topRight)
                    {
                        if(directionG == DOWN) corner = bottomRight;
                        if(directionG == UP) corner = topRight;
                        directionG = RIGHT;
                    }
                }
            }
            
        }
        else
        {
            touchesArrorw(currentBox);   
        }
    }
    else if(directionG == RIGHT)
    {
        currentBox.y -= mVelR;
        if(mVelR != 0)
        {
            if(nextBox || out)
            {
                currentBox.y += mVelR;
                if(allNextBox(particles, directionG))
                {
                    if(mVelR < 0 && control(blocks, directionG))
                    {
                        corner = bottomRight;
                        directionG = DOWN;
                    }
                    else if(mVelR > 0 && control(blocks, directionG))
                    {
                        corner = topRight;
                        directionG = UP;
                    }
                }
            }
        }
        else
        {
            touchesArrorw(currentBox);   
        }
    }
    else if(directionG == LEFT)
    {
        currentBox.y += mVelR;
        if(mVelR != 0)
        {
            if(nextBox || out)
            {
                currentBox.y -= mVelR;
                if(allNextBox(particles, directionG))
                {
                    if(mVelR < 0 && control(blocks, directionG))
                    {
                        corner = topLeft;
                        directionG = UP;
                    }
                    else if(mVelR > 0 && control(blocks, directionG))
                    {
                        corner = bottomLeft;
                        directionG = DOWN;
                    }
                }
            }
        }
        else
        {
            touchesArrorw(currentBox);   
        }
    }

    

    //up, down
    if(parJum)
    {
        if(directionG == UP)
        {
            currentBox.y -= mVelD;
            if(touchesWall(currentBox, blocks, false, true) || touchesParticle  (currentBox, particles, index, nextBox) || out)
            {
                currentBox.y += mVelD;
                mVelD = 0;
                parJum = false;
            }
        }
        else if(directionG == RIGHT)
        {
            currentBox.x += mVelD;
            if(touchesWall(currentBox, blocks, false, true) || touchesParticle  (currentBox, particles, index, nextBox) || out)
            {
                currentBox.x -= mVelD;
                mVelD = 0;
                parJum = false;
            }
        }
        else if(directionG == DOWN)
        {
            currentBox.y += mVelD;
            if(touchesWall(currentBox, blocks, false, true) || touchesParticle  (currentBox,  particles, index, nextBox) || out)
            {
                currentBox.y -= mVelD;
                mVelD = 0;
                parJum = false;
            }

        }
        else if(directionG == LEFT)
        {
            currentBox.x -= mVelD;
            if(touchesWall(currentBox, blocks, false, true) || touchesParticle  (currentBox, particles, index, nextBox) || out)
            {
                currentBox.x += mVelD;
                mVelD = 0;
                parJum = false;
            }
        }
    }
    
    futureBox = currentBox;
    nextBox = false;
    stop = false;
    border = false;
}

void Liquid::setCamera(SDL_Rect &camera)
{
    camera.x = (currentBox.x + 3 + 30 / 2) - SCREEN_WIDTH / 2;
	camera.y = (currentBox.y + 18 / 2) - SCREEN_HEIGHT / 2;

	//Keep the camera in bounds
	if( camera.x < 0 )
	{
		camera.x = 0;
	}
	if( camera.y < 0 )
	{
		camera.y = 0;
	}
	if( camera.x > LEVEL_WIDTH - camera.w )
	{
		camera.x = LEVEL_WIDTH - camera.w;
	}
	if( camera.y > LEVEL_HEIGHT - camera.h )
	{
		camera.y = LEVEL_HEIGHT - camera.h;
	}
}

void Liquid::render(SDL_Rect &camera)
{
    SDL_Rect rect = {currentBox.x - camera.x, currentBox.y - camera.y, 1, 1};
    SDL_SetRenderDrawColor(gRenderer, mR, mG, mB, 255);
    SDL_RenderFillRect(gRenderer, &rect);
}

SDL_Rect Liquid::getFutureBox()
{
    return futureBox;
}

SDL_Rect Liquid::getCurrentBox()
{
    return currentBox;
}

bool Liquid::getNextBox()
{
    return nextBox;
}

int Liquid::getDir()
{
    return directionG;
}

int Liquid::getVel()
{
    return mVelR;
}

bool Liquid::getBorder()
{
    return border;
}

void Liquid::setNextBox()
{
    nextBox = stop;
}

void Liquid::setStop(bool nomove)
{
    stop = nomove;
}

void Liquid::setBorder(bool bo)
{
    border = bo;
}

bool Liquid::isOn(Block *blocks[], Liquid *particles[], int d)
{
    SDL_Rect futurePos = { currentBox.x, currentBox.y, 1, 1 };
    if(d == UP)
    {
        futurePos.y -= 1;
    }
    else if(d == RIGHT)
    {
        futurePos.x += 1;
    }
    else if(d == DOWN)
    {
        futurePos.y += 1;
    }
    else if(d == LEFT)
    {
        futurePos.x -= 1;
    }
    
    if(touchesParticle(futurePos, particles, index, nextBox) || touchesWall(futurePos, blocks, false, true))
    {
        return true;
    }
    return false;
}

bool Liquid::isOut(SDL_Rect box, Block *blocks[], int d)
{
    SDL_Rect longRect = {box.x, box.y, 1, 1};
    if(d == UP)
    {
        longRect.y -= BLOCK_HEIGHT;
        longRect.h = BLOCK_HEIGHT;
    }
    else if(d == RIGHT)
    {
        longRect.w = BLOCK_WIDTH;
    }
    else if(d == DOWN)
    {
        longRect.h = BLOCK_HEIGHT;
    }
    else if(d == LEFT)
    {
        longRect.x -= BLOCK_WIDTH;
        longRect.w = BLOCK_WIDTH;
    }

    if(touchesWall(longRect, blocks, false, true, true))
    {
        return false;
    }
    else
    {
        return true;    
    }
    
}

bool Liquid::control(Block *blocks[], int d)
{
    SDL_Rect temp;
    SDL_Rect downtemp;
    int bx, by;
    int dx, dy;
    bool nearWall = false;
    for(int i = 0; i < TOTAL_BLOCKS; ++i)
    {
        if(blocks[i]->getType() != dark && blocks[i]->getType() != air)
        {
            temp = blocks[i]->getBox();
            bx = temp.x;
            by = temp.y;
            if(d == UP || d == DOWN)
            {
                //粒子がブロックの右側に近い場合
                if(((currentBox.x >= bx + BLOCK_WIDTH) && (currentBox.x <= bx + BLOCK_WIDTH + 50)) && ((currentBox.y <= by + BLOCK_HEIGHT) && (currentBox.y >= by)))
                {
                    if(parJum) stick = true;
                    if(mVelR < 0) nearWall = true;
                }
                //粒子がブロックの左側に近い場合
                if(((currentBox.x <= bx) && (currentBox.x >= bx - 50)) && ((currentBox.y <= by + BLOCK_HEIGHT) && (currentBox.y >= by)))
                {
                    if(parJum) stick = true;
                    if(mVelR > 0) nearWall = true;
                }
            }
            else if(d == RIGHT)
            {
                //粒子がブロックの下側に近い場合(天井)
                if((mVelR > 0) && ((currentBox.y >= by + BLOCK_HEIGHT) && (currentBox.y <= by + BLOCK_HEIGHT + 50)) && ((currentBox.x <= bx + BLOCK_WIDTH) && (currentBox.x >= bx)))
                {
                    nearWall = true;
                }
                //粒子がブロックの上側に近い場合（地面）
                if((mVelR < 0) && ((currentBox.y <= by) && (currentBox.y >= by - 50)) && ((currentBox.x <= bx + BLOCK_WIDTH) && (currentBox.x >= bx)))
                {
                    nearWall = true;
                }
            }
            else if(d == LEFT)
            {
                //粒子がブロックの下側に近い場合(天井)
                if((mVelR < 0) && ((currentBox.y >= by + BLOCK_HEIGHT) && (currentBox.y <= by + BLOCK_HEIGHT + 50)) && ((currentBox.x <= bx + BLOCK_WIDTH) && (currentBox.x >= bx)))
                {
                    nearWall = true;
                }
                //粒子がブロックの上側に近い場合（地面）
                if((mVelR > 0) && ((currentBox.y <= by) && (currentBox.y >= by - 50)) && ((currentBox.x <= bx + BLOCK_WIDTH) && (currentBox.x >= bx)))
                {
                    nearWall = true;
                }
            }
        }
    }

    for(int i = 0; i < 2; ++i)
    {
        downtemp = downwall[i].getBox();
        dx = downtemp.x;
        dy = downtemp.y;
        if(d == UP || d == DOWN)
        {
            //粒子がブロックの右側に近い場合
            if(((currentBox.x >= dx + 10) && (currentBox.x <= dx + 10 + 50)) && ((currentBox.y <= dy + BLOCK_HEIGHT) && (currentBox.y >= dy)))
            {
                if(parJum) stick = true;
                if(mVelR < 0) nearWall = true;
            }
            //粒子がブロックの左側に近い場合
            if((((currentBox.x <= dx) && (currentBox.x >= dx - 50)) && ((currentBox.y <= dy + BLOCK_HEIGHT) && (currentBox.y >= dy))))
            {
                if(parJum) stick = true;
                if(mVelR > 0) nearWall = true;
            }
        }
    }
    return nearWall;
}

void Liquid::reset()
{
    mVelR = 0;
    mVelD = 0;
    directionG = DOWN;
    keyDownU = false;
    keyDownD = false;
    keyDownR = false;
    keyDownL = false;
}

Text::Text()
{
    mx = 0;
    my = 0;
}

void Text::render(SDL_Rect &camera, int i)
{
    if(gTextTexture != NULL)
    {
        gTextTexture[i].render(mx - camera.x, my - camera.y);
    }
}

void Text::setXY(int x, int y)
{
    mx = x;
    my = y;
}


DownWall::DownWall()
{
    mBox = {0, 0, 10, 50};
    countY = 0;
    up = false;
}

void DownWall::render(SDL_Rect &camera)
{
    gDownWallTexture.render(mBox.x - camera.x, mBox.y - camera.y, NULL, 0.0, NULL, SDL_FLIP_NONE, 10, 50);
}

void DownWall::setPos(int x, int y)
{
    mBox.x = x;
    mBox.y = y;
    initx = x;
    inity = y;
}

void DownWall::down()
{
    if(countY < 100)
    {
        if(countY % 2 == 0)
        {
            if(up)
            {
                --mBox.y;
            }
            else
            {
                ++mBox.y;
            }
            
        }
        ++countY;
    }
}

SDL_Rect DownWall::getBox()
{
    return mBox;
}

void DownWall::clearObje()
{
    countY = 0;
    mBox.x = -50;
    mBox.y = -50;
}

void DownWall::setUp(bool uping)
{
    up = uping;
}

void DownWall::reset()
{
    countY = 0;
    mBox.x = initx;
    mBox.y = inity;
}


Button::Button()
{
    doDraw = false;
    pushing = false;
    mBox = {0, 0, 40, 18};
    pushingBox = {0, 9, 40, 9};
    mType = 0;
}

void Button::render(SDL_Rect &camera)
{
    gButtonTexture.render(mBox.x - camera.x, mBox.y - camera.y, &gButtonClips[mType], 0.0, NULL, SDL_FLIP_NONE, mBox.w, mBox.h);
}

void Button::setPos(int x, int y)
{
    mBox.x = x;
    mBox.y = y;
    pushingBox.x = x;
    pushingBox.y += y;
}

void Button::setRender(bool render)
{
    doDraw = render;
}

bool Button::getRender()
{
    return doDraw;
}

SDL_Rect Button::getBox()
{
    if(mType == PUSH)
    {
        return pushingBox;
    }
    return mBox;
}

bool Button::getPushing()
{
    return pushing;
}

void Button::push(Slime slime)
{
    SDL_Rect topButton = {mBox.x, mBox.y - 1, mBox.w, 1};
    if(checkCollision(slime.getBox(), topButton) && !pushing)
    {
        pushing = true;
        mType = PUSH;
    }
}

void Button::clearObje()
{
    doDraw = false;
    pushing = false;
    mType = 0;
    mBox = {0, 0, 40, 18};
    pushingBox = {0, 9, 40, 9};
}

void Button::reset()
{
    pushing = false;
    mType = 0;
}


Arrow::Arrow()
{
    mBox = {0, 0, 40, 10};
    colliBox = {0, 0, 0, 0};
    a = 255;
    index = 0;
    flightDistance = 0;
    disappear = false;
    waiting = false;
    waitms = 0;
    initwait = false;
}

void Arrow::render(SDL_Rect &camera)
{
    double angle;
    gArrowTexture.setAlpha(a);
    //上右下左
    if(launcher[index].getDir() == 0) angle = 90.0;
    if(launcher[index].getDir() == 1) angle = 180.0;
    if(launcher[index].getDir() == 2) angle = 270.0;
    if(launcher[index].getDir() == 3) angle = 0.0;
    gArrowTexture.render(mBox.x - camera.x, mBox.y - camera.y, NULL, angle, NULL, SDL_FLIP_NONE, mBox.w, mBox.h);
}

void Arrow::setPos()
{
    SDL_Rect tempRect = launcher[index].getBox();
    //上右下左
    if(launcher[index].getDir() == 0)
    {
        mBox.x = tempRect.x - 5;
        mBox.y = tempRect.y;
        colliBox.x = mBox.x + 15;
        colliBox.y = mBox.y - 15;
        colliBox.w = 10;
        colliBox.h = 40;
    }
    if(launcher[index].getDir() == 1)
    {
        mBox.x = tempRect.x - 5;
        mBox.y = tempRect.y + 10;
        colliBox.x = mBox.x;
        colliBox.y = mBox.y;
        colliBox.w = 40;
        colliBox.h = 10;
    }
    if(launcher[index].getDir() == 2)
    {
        mBox.x = tempRect.x - 5;
        mBox.y = tempRect.y + 35;
        colliBox.x = mBox.x + 15;
        colliBox.y = mBox.y - 15;
        colliBox.w = 10;
        colliBox.h = 40;
    }
    if(launcher[index].getDir() == 3)
    {
        mBox.x = tempRect.x;
        mBox.y = tempRect.y + 10;
        colliBox.x = mBox.x;
        colliBox.y = mBox.y;
        colliBox.w = 40;
        colliBox.h = 10;
    }
}

SDL_Rect Arrow::getBox()
{
    return colliBox;
}

void Arrow::launch(Block *blocks[])
{
    if(waiting)
    {
        delay();
    }
    
    if(!waiting)
    {
        if((touchesWall(colliBox, blocks, true, false) || mBox.x + mBox.w <= 0) && flightDistance > 100)
        {
            disappear = true;
            a -= 3;
            if(a <= 0)
            {
                a = 0;
                reset();
            }
        }
        else
        {
            if(launcher[index].getDir() == 0)
            {
                mBox.y -= 2;
                colliBox.y -= 2;
            }
            else if(launcher[index].getDir() == 1)
            {
                mBox.x += 2;
                colliBox.x += 2;
            }
            else if(launcher[index].getDir() == 2)
            {
                mBox.y += 2;
                colliBox.y += 2;
            }
            else if(launcher[index].getDir() == 3)
            {
                mBox.x -= 2;
                colliBox.x -= 2;
            }
            flightDistance += 2;
        }
    }
    
}

void Arrow::clearObje()
{
    a = 255;
    flightDistance = 0;
    disappear = false;
    mBox.x = -50;
    mBox.y = -50;
    colliBox.x = -50;
    colliBox.x = -50;
    waiting = false;
    waitms = 0;
    initwait = false;
}

int Arrow::getAlpha()
{
    return a;
}

void Arrow::setIndex(int i)
{
    index = i;
}

bool Arrow::getDisa()
{
    return disappear;
}

void Arrow::reset()
{
    a = 255;
    flightDistance = 0;
    disappear = false;
    if(die) arrowDelay = SDL_GetTicks();
    waiting = initwait;
    setPos();
}

void Arrow::setDelay(bool wait, int ms)
{
    waiting = wait;
    waitms = ms;
    initwait = wait;
}

void Arrow::delay()
{
    Uint32 currenttime = SDL_GetTicks();
    if(currenttime - arrowDelay >= waitms)
    {
        waiting = false;
    }
}

bool Arrow::getWaiting()
{
    return waiting;
}


Launcher::Launcher()
{
    mBox = {0, 0, 30, 30};
    doDraw = false;
    dir = 0;
}

void Launcher::render(SDL_Rect &camera)
{
    double angle;
    if(dir == 0) angle = 90.0;
    if(dir == 1) angle = 180.0;
    if(dir == 2) angle = 270.0;
    if(dir == 3) angle = 0.0;
    gLauncherTexture.render(mBox.x - camera.x, mBox.y - camera.y, NULL, angle, NULL, SDL_FLIP_NONE, mBox.w, mBox.h);
}

void Launcher::setRender(bool render)
{
    doDraw = render;
}

void Launcher::setPos(int x, int y)
{
    mBox.x = x;
    mBox.y = y;
}

SDL_Rect Launcher::getBox()
{
    return mBox;
}

bool Launcher::getRender()
{
    return doDraw;
}

void Launcher::clearObje()
{
    doDraw = false;
    mBox.x = -50;
    mBox.y = -50;
}

void Launcher::setDir(int d)
{
    dir = d;
}

int Launcher::getDir()
{
    return dir;
}


Curtain::Curtain()
{
    mBox = {0 - SCREEN_WIDTH - 400, 0, SCREEN_WIDTH + 400, SCREEN_HEIGHT};
    changePoint = false;
}

void Curtain::render()
{
    gCurtainTexture.render(mBox.x, mBox.y, NULL, 0.0, NULL, SDL_FLIP_NONE, mBox.w, mBox.h);
}

void Curtain::closeCurtain(Slime &slime, Block *blocks[])
{
    stageChanging = true;
    if(stageChanging) mBox.x += 6;
    if (mBox.x >= SCREEN_WIDTH)
    {
        stageChanging = false;
        reset();
    }
    if((mBox.x >= 0 - 400) && !changePoint)
    {
        changePoint = true;
    }
}

void Curtain::reset()
{
    changePoint = false;
    mBox= {0 - SCREEN_WIDTH - 400, 0, SCREEN_WIDTH + 400, SCREEN_HEIGHT};
}

bool Curtain::getChangePoint()
{
    return changePoint;
}


bool init()
{
    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        success = false;
    }
    else
    {
        gWindow = SDL_CreateWindow("LIM", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (gWindow == NULL)
        {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            success = false;
        }
        else
        {
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            if (gRenderer == NULL)
            {
                printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
                success = false;
            }
            else
            {
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
                int imgFlags = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlags) & imgFlags))
                {
                    printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
                    success = false;
                }

                if( TTF_Init() == -1 )
                {
                    printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
                    success = false;
                }
            }
        }
    }

    return success;
}

bool loadMedia()
{
    bool success = true;

    //画像
    if (!gSlimeTexture.loadFromFile("img/spriteSlime36.png"))
    {
        printf("Failed to load slime texture!\n");
        success = false;
    }

    if (!gBlockTexture.loadFromFile("img/spriteBlock.png"))
    {
        printf("Failed to load block texture!\n");
        success = false;
    }
    gBlockTexture.setBlendMode(SDL_BLENDMODE_BLEND);

    if (!gBgTexture.loadFromFile("img/background.png"))
    {
        printf("Failed to load background texture!\n");
        success = false;
    }

    if(!gDownWallTexture.loadFromFile("img/wallDown.png"))
    {
        printf("Failed to load wallDown texture!");
        success = false;
    }

    if(!gButtonTexture.loadFromFile("img/spriteButton.png"))
    {
        printf("Failed to load button texture!");
        success = false;
    }

    if(!gArrowTexture.loadFromFile("img/arrow.png"))
    {
        printf("Failed to load arrow texture");
        success = false;
    }

    if(!gLauncherTexture.loadFromFile("img/launcher.png"))
    {
        printf("Failed to load launcher texture");
        success = false;
    }

    if(!gCurtainTexture.loadFromFile("img/curtain.png"))
    {
        printf("Failed to load curtain texture");
        success = false;
    }

    //font
    titleFont = TTF_OpenFont("Best10-Font/BestTen-CRT.otf", 28);
    instFont = TTF_OpenFont("Best10-Font/BestTen-CRT.otf", 18);
    if(titleFont == NULL || instFont == NULL)
    {
        printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        success = false;
    }

    // spritesheetの分割
    clipSlime();
    clipButton();
    return success;
}

void close(Block *blocks[])
{
    for (int i = 0; i < TOTAL_BLOCKS; ++i)
    {
        if (blocks[i] != NULL)
        {
            delete blocks[i];
            blocks[i] = NULL;
        }
    }

    gSlimeTexture.free();
    gBlockTexture.free();
    gBgTexture.free();
    gDownWallTexture.free();
    gButtonTexture.free();
    gArrowTexture.free();
    gLauncherTexture.free();
    gCurtainTexture.free();
    for(int i = 0; i < TOTAL_TEXT; ++i)
    {
        gTextTexture[i].free();
    }

    TTF_CloseFont(titleFont);
    titleFont = NULL;
    TTF_CloseFont(instFont);
    instFont = NULL;

    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

bool opening(Block *blocks[], SDL_Event &e)
{
    SDL_Rect rect = {LEVEL_WIDTH / 2 - SCREEN_WIDTH / 2, 50, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_Color textColor = {255, 255, 255};
    gTextTexture[0].loadFromRenderedText("Press Any Key", textColor);
    int currentFrame = 0;
    int totalFrames = 80;
    bool appear = true;
    if(!setBlocks(blocks, OPENING))
    {
        printf("Failled set blocks");
    }
    else
    {
        while(1)
        {
            ++currentFrame;
            while (SDL_PollEvent(&e) != 0)
            {
                if (e.type == SDL_QUIT)
                {
                    return true;
                }
                if(e.type == SDL_KEYDOWN)
                {
                    return false;
                }
				
            }
            
            SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(gRenderer);

            gBgTexture.render(0, 0, NULL, 0.0, NULL, SDL_FLIP_NONE, 800, 600);

            for (int i = 0; i < TOTAL_BLOCKS; ++i)
            {
                blocks[i]->render(rect);
            }

            gSlimeTexture.render(380, 365, &gSlimeClips[BASIC0]);

            if(currentFrame >= totalFrames)
            {
                currentFrame = 0;
                appear = (appear == true) ? false : true;
            }

            if(appear)
            {
                gTextTexture[0].render((SCREEN_WIDTH - gTextTexture[0].getWidth()) / 2, SCREEN_HEIGHT - 100);
            }

            SDL_RenderPresent(gRenderer);

        	Uint32 currentTicks = SDL_GetTicks();
    		Uint32 frameTicks = currentTicks - prevTicks;
    		if (frameTicks < SCREEN_TICKS_PER_FRAME)
    		{
    		    SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
    		}
    		prevTicks = currentTicks;
        }
    }
    return false;
}

bool checkCollision(SDL_Rect a, SDL_Rect b, int type)
{
    bool collision = true;

    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    leftA = a.x;
    rightA = a.x + a.w;
    topA = a.y;
    bottomA = a.y + a.h;

    leftB = b.x;
    rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;
    if(type == dark)
    {
        leftB = b.x + 40;
    }

    if(type == trap)
    {
        topB = b.y + 20;
    }

    if (bottomA <= topB)
    {
        collision = false;
    }

    if (topA >= bottomB)
    {
        collision = false;
    }

    if (rightA <= leftB)
    {
        collision = false;
    }

    if (leftA >= rightB)
    {
        collision = false;
    }

    return collision;
}

bool touchesWall(SDL_Rect box, Block *blocks[], bool arrowcolli, bool slimecolli, bool outCheck)
{
    //block
    for (int i = 0; i < TOTAL_BLOCKS; ++i)
    {
        if(blocks[i]->getNear() || !slimecolli)
        {
            if (blocks[i]->getType() != air)
            {
                // If the collision box touches the wall tile
                if (checkCollision(box, blocks[i]->getBox(), blocks[i]->getType()))
                {
                    if(blocks[i]->getType() == dark)
                    {
                        if(slimecolli) clear = true;
                    }
                    if(blocks[i]->getType() == trap)
                    {
                        if(outCheck) return false;
                        if(slimecolli) die = true;
                    }
                    return true;
                }
            }
        }
    }
    

    //button,downwall
    for(int i = 0; i < 2; ++i)
    {
        if(checkCollision(box, button[i].getBox()))
        {
            out = true;
            return true;
        }
        if(checkCollision(box, downwall[i].getBox()))
        {
            return true;
        }
    }

    //arrow,launcher
    for(int i = 0; i < TOTAL_LAUNCHARROW; ++i)
    {
        if(!arrowcolli)
        {
            if(checkCollision(box, launcher[i].getBox()))
            {
                out = true;
                return true;
            }
            if(checkCollision(box, arrow[i].getBox()))
            {
                if(!arrow[i].getDisa())
                {
                    die = true;
                    return true;
                }
            }
        }
    }
    return false;
}

bool touchesParticle(SDL_Rect box, Liquid *particles[], int index, bool nextBox)
{
    for(int i = 0; i < TOTAL_PARTICLE; ++i)
    {
        if(i != index && checkCollision(box, particles[i]->getFutureBox()))
        {
            return true;
        }
        if(i != index && checkCollision(box, particles[i]->getCurrentBox()) && nextBox)
        {
            return true;
        }
    }
    return false;
}

bool touchesArrorw(SDL_Rect pbox)
{
    for(int i = 0; i < TOTAL_LAUNCHARROW; ++i)
    {
        if(checkCollision(pbox, arrow[i].getBox()) && !arrow[i].getDisa())
        {
            die = true;
            return true;
        }
    }
    return false;
}


bool setBlocks(Block *blocks[], int stage)
{
    bool blocksLoaded = true;
    int x = 0, y = 0;
    std::ifstream map;
    if(stage == OPENING) map.open("map/opening.map");
    if(stage == STAGE1) map.open("map/stage1.map");
    if(stage == STAGE2) map.open("map/stage2.map");
    if(stage == STAGE3) map.open("map/stage3.map");
    if(stage == STAGE4) map.open("map/stage4.map");
    if(stage == STAGE5) map.open("map/stage5.map");
    if(stage == STAGE6) map.open("map/stage6.map");
    if(stage == STAGE7) map.open("map/stage7.map");
    if(stage == STAGE8) map.open("map/stage8.map");
    if(stage == GAMECLEAR) map.open("map/gameclear.map");
    if (map.fail())
    {
        printf("Unable to load map file!\n");
        blocksLoaded = false;
    }
    else
    {
        for (int i = 0; i < TOTAL_BLOCKS; ++i)
        {
            int blockType = -1;
            map >> blockType;
            if (map.fail())
            {
                // Stop loading map
                printf("Error loading map: Unexpected end of file!\n");
                blocksLoaded = false;
                break;
            }

            if ((blockType >= 0) && (blockType < TOTAL_BLOCK_SPRITE))
            {
                blocks[i] = new Block(x, y, blockType);
            }
            else
            {
                // Stop loading map
                printf("Error loading map: Invalid tile type at %d!\n", i);
                blocksLoaded = false;
                break;
            }

            x += BLOCK_WIDTH;
            if (x >= LEVEL_WIDTH)
            {
                // Move back
                x = 0;

                // Move to the next row
                y += BLOCK_HEIGHT;
            }
        }

        if(blocksLoaded)
        {
            gBlockClips[dark].x = 0;
			gBlockClips[dark].y = 0;
			gBlockClips[dark].w = SPRITE_BLOCK_WIDTH;
			gBlockClips[dark].h = SPRITE_BLOCK_HEIGHT;

			gBlockClips[sand].x = 24;
			gBlockClips[sand].y = 0;
			gBlockClips[sand].w = SPRITE_BLOCK_WIDTH;
			gBlockClips[sand].h = SPRITE_BLOCK_HEIGHT;

            gBlockClips[blockTop].x = 48;
			gBlockClips[blockTop].y = 0;
			gBlockClips[blockTop].w = SPRITE_BLOCK_WIDTH;
			gBlockClips[blockTop].h = SPRITE_BLOCK_HEIGHT;

            gBlockClips[blockLeft].x = 0;
			gBlockClips[blockLeft].y = 24;
			gBlockClips[blockLeft].w = SPRITE_BLOCK_WIDTH;
			gBlockClips[blockLeft].h = SPRITE_BLOCK_HEIGHT;

            gBlockClips[blockRight].x = 24;
			gBlockClips[blockRight].y = 24;
			gBlockClips[blockRight].w = SPRITE_BLOCK_WIDTH;
			gBlockClips[blockRight].h = SPRITE_BLOCK_HEIGHT;

            gBlockClips[block].x = 48;
			gBlockClips[block].y = 24;
			gBlockClips[block].w = SPRITE_BLOCK_WIDTH;
			gBlockClips[block].h = SPRITE_BLOCK_HEIGHT;

            gBlockClips[blockBottom].x = 0;
			gBlockClips[blockBottom].y = 48;
			gBlockClips[blockBottom].w = SPRITE_BLOCK_WIDTH;
			gBlockClips[blockBottom].h = SPRITE_BLOCK_HEIGHT;

            gBlockClips[blockTR].x = 24;
			gBlockClips[blockTR].y = 48;
			gBlockClips[blockTR].w = SPRITE_BLOCK_WIDTH;
			gBlockClips[blockTR].h = SPRITE_BLOCK_HEIGHT;

            gBlockClips[blockTL].x = 48;
			gBlockClips[blockTL].y = 48;
			gBlockClips[blockTL].w = SPRITE_BLOCK_WIDTH;
			gBlockClips[blockTL].h = SPRITE_BLOCK_HEIGHT;

            gBlockClips[trap].x = 0;
			gBlockClips[trap].y = 72;
			gBlockClips[trap].w = SPRITE_BLOCK_WIDTH;
			gBlockClips[trap].h = SPRITE_BLOCK_HEIGHT;

            gBlockClips[cornerL].x = 24;
			gBlockClips[cornerL].y = 72;
			gBlockClips[cornerL].w = SPRITE_BLOCK_WIDTH;
			gBlockClips[cornerL].h = SPRITE_BLOCK_HEIGHT;

            gBlockClips[cornerR].x = 48;
			gBlockClips[cornerR].y = 72;
			gBlockClips[cornerR].w = SPRITE_BLOCK_WIDTH;
			gBlockClips[cornerR].h = SPRITE_BLOCK_HEIGHT;

            gBlockClips[blockBL].x = 0;
			gBlockClips[blockBL].y = 96;
			gBlockClips[blockBL].w = SPRITE_BLOCK_WIDTH;
			gBlockClips[blockBL].h = SPRITE_BLOCK_HEIGHT;

            gBlockClips[blockBR].x = 24;
			gBlockClips[blockBR].y = 96;
			gBlockClips[blockBR].w = SPRITE_BLOCK_WIDTH;
			gBlockClips[blockBR].h = SPRITE_BLOCK_HEIGHT;

            gBlockClips[air].x = 150;
			gBlockClips[air].y = 150;
			gBlockClips[air].w = SPRITE_BLOCK_WIDTH;
			gBlockClips[air].h = SPRITE_BLOCK_HEIGHT;
        }
    }

    map.close();
    return blocksLoaded;
}

void clipSlime()
{
    gSlimeClips[BASIC0].x = 0;
    gSlimeClips[BASIC0].y = 0;
    gSlimeClips[BASIC0].w = SPRITE_SLIME_WIDTH;
    gSlimeClips[BASIC0].h = SPRITE_SLIME_HEIGHT;

    gSlimeClips[BASIC1].x = SPRITE_SLIME_WIDTH;
    gSlimeClips[BASIC1].y = 0;
    gSlimeClips[BASIC1].w = SPRITE_SLIME_WIDTH;
    gSlimeClips[BASIC1].h = SPRITE_SLIME_HEIGHT;

    gSlimeClips[BASIC2].x = 0;
    gSlimeClips[BASIC2].y = SPRITE_SLIME_HEIGHT;
    gSlimeClips[BASIC2].w = SPRITE_SLIME_WIDTH;
    gSlimeClips[BASIC2].h = SPRITE_SLIME_HEIGHT;

    gSlimeClips[JUMP].x = SPRITE_SLIME_WIDTH;
    gSlimeClips[JUMP].y = SPRITE_SLIME_HEIGHT;
    gSlimeClips[JUMP].w = SPRITE_SLIME_WIDTH;
    gSlimeClips[JUMP].h = SPRITE_SLIME_HEIGHT;

    gSlimeClips[JUMPING].x = SPRITE_SLIME_WIDTH * 2;
    gSlimeClips[JUMPING].y = 0;
    gSlimeClips[JUMPING].w = SPRITE_SLIME_WIDTH;
    gSlimeClips[JUMPING].h = SPRITE_SLIME_HEIGHT;

}

void clipButton()
{
    gButtonClips[NORMAL].x = 0;
    gButtonClips[NORMAL].y = 0;
    gButtonClips[NORMAL].w = 16;
    gButtonClips[NORMAL].h = 7;

    gButtonClips[PUSH].x = 16;
    gButtonClips[PUSH].y = 0;
    gButtonClips[PUSH].w = 16;
    gButtonClips[PUSH].h = 7;
}

void setParticles(Liquid *particles[])
{
    int i = 0;
    Uint8 red, green, blue;
    bool eye = false;
    for (int y = 0; y < SPRITE_SLIME_HEIGHT; ++y)
    {
        for (int x = 0; x < SPRITE_SLIME_WIDTH; ++x)
        {
            
            Uint32 pixelColor = gSlimeTexture.getPixel32(x, y);
            red = (pixelColor & 0x00FF0000) >> 16;
            green = (pixelColor & 0x0000FF00) >> 8;
            blue = (pixelColor & 0x000000FF);
            if (pixelColor != SDL_MapRGBA(SDL_GetWindowSurface(gWindow)->format, 255, 255, 255, 255))
            {
                //目玉の判定
                if((pixelColor == SDL_MapRGBA(SDL_GetWindowSurface(gWindow)->format, 0, 0, 0, 255)) || (pixelColor == SDL_MapRGBA(SDL_GetWindowSurface(gWindow)->format, 193, 227, 247, 255)))
                {
                    eye = true;
                }
                else
                {
                    eye = false;
                }
                particles[i] = new Liquid(x, y, red, green, blue, eye, i);
                ++i;
            }
        }
    }
}

void sortParticles(Liquid *particles[])
{
    SDL_Rect box[TOTAL_PARTICLE];
    int direction[TOTAL_PARTICLE];
    int count = 0;
    for (int i = 0; i < TOTAL_PARTICLE; ++i)
    {
        box[i] = particles[i]->getCurrentBox();
        direction[i] = particles[i]->getDir();
    }

    //隣が壁ならその列（行）も壁判定にする
    for (int i = 0; i < TOTAL_PARTICLE; ++i)
    {
        if (!particles[i]->getNextBox())
        {
            continue;
        }

        if(direction[i] == 0 || direction[i] == 2)
        {
            //重力が下向きまたは上向きの場合
            int y = box[i].y;
            for(int j = 0; j < TOTAL_PARTICLE; ++j)
            {
                if(box[j].y == y)
                {
                    particles[j]->setStop(true);
                    ++count;
                }
            }
        }
        else if(direction[i] == 1 || direction[i] == 3)
        {
            //重力が右向きまたは左向きの場合
            int x = box[i].x;
            for(int j = 0; j < TOTAL_PARTICLE; ++j)
            {
                if(box[j].x == x)
                {
                    particles[j]->setStop(true);
                    ++count;
                }
            }
        }
    }
    
    for(int i = 0; i < TOTAL_PARTICLE; ++i)
    {
        particles[i]->setNextBox();
    }
}

bool allNextBox(Liquid *particles[], int d)
{
    for(int i = 0; i < TOTAL_PARTICLE; ++i)
    {
        if(!(particles[i]->getNextBox()) && (d == particles[i]->getDir()))
        {
            return false;
        }
    }
    return true;
}

void isBorder(Liquid *particles[])
{
    SDL_Rect box;
    int directionG;
    int vel;
    for(int i = 0; i < TOTAL_PARTICLE; ++i)
    {
        directionG = particles[i]->getDir();
        box = particles[i]->getCurrentBox();
        vel = particles[i]->getVel();
        //U R D L
        if(vel != 0)
        {
            if(directionG == 0)
            {
                if(vel > 0)
                {
                    ++box.x;
                }
                else if(vel < 0)
                {
                    --box.x;
                }
            }
            else if(directionG == 1)
            {
                if(vel > 0)
                {
                    --box.y;
                }
                else if(vel < 0)
                {
                    ++box.y;
                }
            }
            else if(directionG == 2)
            {
                if(vel > 0)
                {
                    ++box.x;
                }
                else if(vel < 0)
                {
                    --box.x;
                }
            }
            else if(directionG == 3)
            {
                if(vel > 0)
                {
                    ++box.y;
                }
                else if(vel < 0)
                {
                    --box.y;
                }
            }
            if(!touchesParticle(box, particles, i))
            {
                particles[i]->setBorder(true);
            }
        }
        
    }
}

void debug(Liquid *particles[])
{
    SDL_Rect a, b;
    for(int i = 0; i < TOTAL_PARTICLE; ++i)
    {
        a = particles[i]->getCurrentBox();
        for(int j = 0; j < TOTAL_PARTICLE; ++j)
        {
            b = particles[j]->getCurrentBox();
            if(a.x == b.x && a.y == b.y && !(i == j))
            {
                printf("1 ");
            }
        }
    }
}

bool timeStop()
{
    Uint32 currentTime = SDL_GetTicks();
    if(currentTime - transTime >= 300)
    {
        return false;
    }
    return true;
}

void nextStage(Block *blocks[], Slime &slime, Curtain &curtain, int *textX, int *textY)
{
    enum dir
    {
        UP,
        RIGHT,
        DOWN,
        LEFT
    };
    for(int i = 0; i < TOTAL_TEXT; ++i)
    {
        gTextTexture[i].free();
        textX[i] = -1;
        textY[i] = -1;
    }
    for(int i = 0; i < 2; ++i)
    {
        button[i].clearObje();
        downwall[i].clearObje();
    }
    for(int i = 0; i < TOTAL_LAUNCHARROW; ++i)
    {
        arrow[i].clearObje();
        launcher[i].clearObje();
    }
    liquid = false;
    clear = false;
    ++currentStage;
    SDL_Color textColor = {170, 170, 170};

    //特定の場所に文字表示
    switch (currentStage)
    {
    case STAGE1:

        LEVEL_WIDTH = 1250;
        LEVEL_HEIGHT = 700;

        gTextTexture[0].loadFromRenderedText("Press the up key", textColor);
        textX[0] = 150;
        textY[0] = 350;

        break;
    
    case STAGE2:

        LEVEL_WIDTH = 1250;
        LEVEL_HEIGHT = 650;

        break;

    case STAGE3:

        LEVEL_WIDTH = 1250;
        LEVEL_HEIGHT = 50 * 26;

        gTextTexture[0].loadFromRenderedText("Press the Z key", textColor);
        gTextTexture[1].loadFromRenderedText("Press the ", textColor);
        gTextTexture[2].loadFromRenderedText("z key again", textColor);
        textX[0] = 250;
        textY[0] = 870;
        textX[1] = 150;
        textY[1] = 670;
        textX[2] = 150;
        textY[2] = 700;
        
        break;

    case STAGE4:

        LEVEL_WIDTH = 1250;
        LEVEL_HEIGHT = 50 * 23;

        button[0].setPos(1150, 785);
        downwall[0].setPos(440, 650);
        button[0].setRender(true);
        
        arrow[0].setIndex(0);
        launcher[0].setPos(50, 50 * 14 + 20);
        launcher[0].setDir(RIGHT);
        arrow[0].setPos();
        launcher[0].setRender(true);
        break;

    case STAGE5:

        LEVEL_WIDTH = 1250;
        LEVEL_HEIGHT = 700;

        //button,downwall
        button[0].setPos(1157, 385);
        downwall[0].setPos(1040, 300);
        button[0].setRender(true);

        //arrow
        arrowDelay = SDL_GetTicks();
        launcher[0].setPos(50 * 5 + 10, 100);
        launcher[0].setDir(DOWN);
        arrow[0].setDelay(true, 850);

        launcher[1].setPos(50 * 8 + 10, 100);
        launcher[1].setDir(DOWN);

        launcher[2].setPos(50 * 13 + 10, 100);
        launcher[2].setDir(DOWN);
        arrow[2].setDelay(true, 1700);

        launcher[3].setPos(50 * 19 + 20, 300);
        launcher[3].setDir(LEFT);
        arrow[3].setDelay(true, 850);

        for(int i = 0; i < 4; ++i)
        {
            arrow[i].setIndex(i);
            arrow[i].setPos();
            launcher[i].setRender(true);
        }

        break;

    case STAGE6:
        LEVEL_WIDTH = 50 * 29;
        LEVEL_HEIGHT = 50 * 35;

        button[0].setPos(50 * 27 + 5, 50 * 14 + 35);
        downwall[0].setPos(50 * 16, 50 * 26);
        downwall[0].setUp(true);
        button[0].setRender(true);

        arrow[0].setIndex(0);
        launcher[0].setPos(50 * 21 + 20, 200 + 20);
        launcher[0].setDir(LEFT);
        arrow[0].setPos();
        launcher[0].setRender(true);
        break;

    case STAGE7:
        LEVEL_WIDTH = 50 * 31;
        LEVEL_HEIGHT = 50 * 16;

        //button, downwall
        button[0].setPos(50 * 12 + 5, 50 * 9 + 35);
        downwall[0].setPos(50 * 4 - 10, 50 * 8);
        downwall[0].setUp(false);
        button[0].setRender(true);

        //arrow
        arrowDelay = SDL_GetTicks();
        launcher[0].setPos(50 * 14 + 20, 400 + 20);
        launcher[0].setDir(LEFT);
        arrow[0].setDelay(true, 850);

        launcher[1].setPos(50 * 14 + 20, 400 - 10);
        launcher[1].setDir(LEFT);

        launcher[2].setPos(50 * 14 + 20, 400 - 40);
        launcher[2].setDir(LEFT);
        arrow[2].setDelay(true, 1700);

        launcher[3].setPos(50 * 21 + 10, 200);
        launcher[3].setDir(DOWN);
        arrow[3].setDelay(true, 850);

        launcher[4].setPos(50 * 23 + 10, 200);
        launcher[4].setDir(DOWN);

        launcher[5].setPos(50 * 25 + 10, 200);
        launcher[5].setDir(DOWN);
        arrow[5].setDelay(true, 1350);

        launcher[6].setPos(50 * 27 + 10, 200);
        launcher[6].setDir(DOWN);
        arrow[6].setDelay(true, 850);

        for(int i = 0; i < 8; ++i)
        {
            arrow[i].setIndex(i);
            arrow[i].setPos();
            launcher[i].setRender(true);
        }


        break;

    case STAGE8:
        LEVEL_WIDTH = 50 * 18;
        LEVEL_HEIGHT = 50 * 15;

        //button, downwall
        button[0].setPos(50 * 12 + 5, 50 * 9 + 35);
        downwall[0].setPos(50 * 14, 50 * 3);
        downwall[0].setUp(false);
        button[0].setRender(true);

        button[1].setPos(50 * 2 + 5, 50 * 3 + 35);
        downwall[1].setPos(50 * 14, 50 * 8);
        downwall[1].setUp(true);
        button[1].setRender(true);

        //arrow
        arrowDelay = SDL_GetTicks();
        launcher[0].setPos(50 * 4 + 10, 450 + 20);
        launcher[0].setDir(UP);
        arrow[0].setDelay(true, 850);

        launcher[1].setPos(50 * 6 + 10, 450 + 20);
        launcher[1].setDir(UP);

        launcher[2].setPos(50 * 8 + 10, 450 + 20);
        launcher[2].setDir(UP);
        arrow[2].setDelay(true, 1700);

        launcher[3].setPos(50 * 10 + 10, 450 + 20);
        launcher[3].setDir(UP);
        arrow[3].setDelay(true, 850);

        launcher[4].setPos(50 * 2, 400 + 20);
        launcher[4].setDir(RIGHT);
        for(int i = 0; i < 5; ++i)
        {
            arrow[i].setIndex(i);
            arrow[i].setPos();
            launcher[i].setRender(true);
        }

        break;

    case GAMECLEAR:
        LEVEL_WIDTH = 50 * 29;
        LEVEL_HEIGHT = 50 * 14;

        gTextTexture[0].loadFromRenderedText("Thank you for playing!!", textColor);
        textX[0] = 550;
        textY[0] = 500;
        break;
    }
    
    TOTAL_BLOCKS = (LEVEL_WIDTH / 50) * (LEVEL_HEIGHT / 50);
    
    setBlocks(blocks, currentStage);
    slime.setDir(true);
    slime.initPos();
}

void checkNearbyBlocks(SDL_Rect sbox, Block *blocks[], int index)
{
    SDL_Rect bbox = blocks[index]->getBox();
    if(std::abs(sbox.x - bbox.x) <= 100 && std::abs(sbox.y - bbox.y) <= 100)
    {
        blocks[index]->setNear(true);
    }
    else
    {
        blocks[index]->setNear(false);
    }
}

int main(int argc, char* args[])
{
    if (!init())
    {
        printf("Failed to initialize!\n");
    }
    else
    {
		Block *blockset[1500];
        if (!loadMedia())
        {
            printf("Failed to load media!\n");
        }
        else
        {
            SDL_Event e;
            Slime slime;
            Curtain curtain;
            Liquid *slimeLiquid[TOTAL_PARTICLE];
            Text text;
            bool quit = false;
            bool liquidFlag = false;
            bool slimeFlag = false;
            bool skip = false;
            int textX[TOTAL_TEXT];
            int textY[TOTAL_TEXT];
            int currentFrame = 0;
            int totalFrames = 4;
            frameTimer = 0;
            setParticles(slimeLiquid);
            SDL_Rect camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

            //タイトル画面
            quit = opening(blockset, e);
            nextStage(blockset, slime, curtain, textX, textY);
            
            while (!quit)
            {
                frameTimer++;

                if (!slime.specialMotion())
                {
                    slime.basicMotion(currentFrame, totalFrames);
                }

                while (SDL_PollEvent(&e) != 0)
                {
                    if (e.type == SDL_QUIT)
                    {
                        quit = true;
                    }
                    
                    /*
                    if(e.type == SDL_KEYDOWN && e.key.repeat == 0)
                    {
                        if(e.key.keysym.sym == SDLK_s)
                        {
                            skip = true;
                        }
                    }
                    */

                    if(!liquid)
                    {
                        slime.handleEvent(e);
                        if(liquid)
                        {
                            liquidFlag = true;
                        }
                        else
                        {
                            out = false;
                        }
                    }
                    else
                    {
                        for(int i = 0; i < TOTAL_PARTICLE; ++i)
                        {
                            slimeLiquid[i]->handleEvent(e, blockset);
                        }
                        if(!liquid) slimeFlag = true;
                    }
                }



                if(!liquid || liquidFlag)
                {
                    for(int i = 0; i < TOTAL_BLOCKS; ++i)
                    {
                        checkNearbyBlocks(slime.getBox(), blockset, i);
                    }
                }
                else
                {
                    for(int i = 0; i < TOTAL_BLOCKS; ++i)
                    {
                        checkNearbyBlocks(slimeLiquid[8]->getCurrentBox(), blockset, i);
                    }
                    
                }

                //スライムの動き
                if(!liquid)
                {
                    //slimeの位置の初期化
                    if(slimeFlag)
                    {
                        slime.reset();
                        slime.setPos(slimeLiquid[0]->getCurrentBox(), blockset, slimeLiquid[0]->getDir());
                        slimeFlag = false;
                    }
                    slime.move(blockset);
                    slime.setCamera(camera);
                }
                
                if(liquid)
                {
                    for(int i = 0; i < TOTAL_PARTICLE; ++i)
                    {
                        //これはslimeLiquidの関数の中で最初に来るようにする
                        if(liquidFlag)
                        {
                            //それぞれの粒子の位置の初期化
                            slimeLiquid[i]->setPos(slime.getBox(), camera, slime.getDir());
                            slimeLiquid[i]->reset();
                        }
                    }
                    isBorder(slimeLiquid);
                    for(int i = 0; i < TOTAL_PARTICLE; ++i)
                    {
                        
                        if(liquidFlag)
                        {
                            //ブロック上になかったらliquidにならない
                            if(out)
                            {
                                liquid = false;
                                break;
                            }
                            if(slimeLiquid[i]->isOut(slimeLiquid[i]->getCurrentBox(), blockset, slimeLiquid[i]->getDir()))
                            {
                                liquid = false;
                            }
                        }
                    }
                    out = false;
                    for(int i = 0; i < TOTAL_PARTICLE; ++i)
                    {
                        slimeLiquid[i]->futureMove(blockset);
                    }
                    liquidFlag = false;

                    //liquidの動き
                    sortParticles(slimeLiquid);
                    for(int i = 0; i < TOTAL_PARTICLE; ++i)
                    {
                        slimeLiquid[i]->currentMove(blockset, slimeLiquid);
                    }
                    
                    slimeLiquid[8]->setCamera(camera);
                }

                //button, downwall
                for(int i = 0; i < 2; ++i)
                {
                    if(button[i].getRender())
                    {
                        button[i].push(slime);
                    }

                    if(button[i].getPushing())
                    {
                        downwall[i].down();
                    }
                }


                //arrow,launcher
                for(int i = 0; i < TOTAL_LAUNCHARROW; ++i)
                {
                    if(launcher[i].getRender())
                    {
                        arrow[i].launch(blockset);
                    }
                }
                                

                //stage
                if(clear || skip)
                {
                    curtain.closeCurtain(slime, blockset);
                    if(curtain.getChangePoint())
                    {
                        nextStage(blockset, slime, curtain, textX, textY);
                        skip = false;
                    }
                }

                if(die)
                {
                    liquid = false;
                    slime.dead();
                    for(int i = 0; i < TOTAL_LAUNCHARROW; ++i)
                    {
                        arrow[i].reset();
                    }
                    for(int i = 0; i < 2; ++i)
                    {
                        downwall[i].reset();
                        button[i].reset();
                    }
                    die = false;
                }


                //render
                
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(gRenderer);
                //bg->downwall->arrow->launcher->block->button->slime->text->curtain
                gBgTexture.render(0, 0, NULL, 0.0, NULL, SDL_FLIP_NONE, 800, 600);

                for(int i = 0; i < 2; ++i)
                {
                    if(button[i].getRender())
                    {
                        downwall[i].render(camera);
                    }
                }

                for(int i = 0; i < TOTAL_LAUNCHARROW; ++i)
                {
                    if(launcher[i].getRender())
                    {
                        if(!arrow[i].getWaiting())
                        {
                            arrow[i].render(camera);
                        }
                        launcher[i].render(camera);
                        
                    }
                }
               
                for (int i = 0; i < TOTAL_BLOCKS; ++i)
                {
                    blockset[i]->render(camera);
                }

                for(int i = 0; i < 2; ++i)
                {
                    if(button[i].getRender())
                    {
                        button[i].render(camera);
                    }
                }
                
                if(!liquid)
                {
                    slime.render(camera);
                }
                else
                {
                    
                    for(int i = 0; i < TOTAL_PARTICLE; ++i)
                    {
                        slimeLiquid[i]->render(camera);
                    }
                    //debug(slimeLiquid);
                }
                
                for(int i = 0; i < TOTAL_TEXT; ++i)
                {
                    if(textX[i] != -1 || textY[i] != -1)
                    {
                        text.setXY(textX[i], textY[i]);
                        text.render(camera, i);
                    }
                }

                if(stageChanging)
                {
                    curtain.closeCurtain(slime, blockset);
                    curtain.render();
                    slime.reset();
                }

                

                SDL_RenderPresent(gRenderer);

				Uint32 currentTicks = SDL_GetTicks();
    			Uint32 frameTicks = currentTicks - prevTicks;
    			if (frameTicks < SCREEN_TICKS_PER_FRAME)
    			{
    			    SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
    			}
    			prevTicks = currentTicks;
            }
        }
		close(blockset);
    }

    return 0;
}