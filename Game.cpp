#include "Game.h"
#include <exception>
#include "TestScene.h"

const float Game::FrameRate = 60.0f;
const float Game::MaxDeltaTime = 0.05f;

Game::Game()
    : m_hwnd(nullptr)
    , m_width(0)
    , m_height(0)
    , m_startTime{}
    , m_endTime{}
    , m_freqTime{}
{
}

Game::~Game()
{
    //シーンの開放
    if (m_scene.get() != nullptr)
    {
        m_scene.reset();
    }
    
}

void Game::initialize(HWND hwnd, int width, int height)
{
    if (width <= 0 || height <= 0) throw std::exception();

    m_hwnd  = hwnd;
    m_width = width;
    m_height = height;

    //レンダラーの初期化
    m_rederer = std::make_unique<Renderer>(this, 0.0f, 0.0f, 0.0f);
    if (!m_rederer->initialize()) throw std::exception();

    //時間計測の初期化
    QueryPerformanceFrequency(&m_freqTime);
    QueryPerformanceCounter(&m_startTime);

    //キーボード初期化
    m_keyboard.initialize();

    //シーンの初期化
    m_scene = std::make_unique<TestScene>(this);

    //ゲーム状態の初期化
}

bool Game::loop()
{
    float deltaTime = 0.0f;
    if (tick(deltaTime))
    {
        input();
        update(deltaTime);
        if (m_scene->isRunning() == false) return false;
        draw();

        //int dt = (int)std::round(deltaTime * 1000.0f);
        //printNum(L"delta time = %d[mesc]\n", dt);
    }
    return m_scene->isRunning();
}

void Game::input()
{
    //キーボードやゲームパッドなどの入力を受け取る処理を記述
    m_keyboard.input();

    //if (m_keyboard.isDown(VK_LEFT)) OutputDebugStringW(L"左矢印キーが押し状態です\n");
    //if (m_keyboard.isPressed('A')) OutputDebugStringW(L"Aキーが押されました\n");
    //if (m_keyboard.isUp(VK_RIGHT)) OutputDebugStringW(L"右矢印キーが離し状態です\n");
    //if (m_keyborad.isReleased('B')) OutputDebugStringW(L"Bキーが押されました\n");
}

void Game::update(float deltaTime) 
{
    m_scene->update(deltaTime);
}

void Game::draw()
{
    m_rederer->begin();
    //ここにゲームの描画処理を記述
    m_scene->draw();

    m_rederer->end();
}

bool Game::tick(float& deltaTime)
{
    QueryPerformanceCounter(&m_endTime);
    if (m_endTime.QuadPart - m_startTime.QuadPart == 0) return false;

    deltaTime = (float)(m_endTime.QuadPart - m_startTime.QuadPart)
        / (float)m_freqTime.QuadPart;
    if (deltaTime < (1.0f / (FrameRate + 1.0f))) return false;

    m_startTime = m_endTime;
    deltaTime = (deltaTime > MaxDeltaTime) ? MaxDeltaTime : deltaTime;
    return true;
}
