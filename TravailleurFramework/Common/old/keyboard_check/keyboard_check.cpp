#include "keyboard_check.h"

bool isKeyPressed_CTRL()
{
    if(GetAsyncKeyState(VK_CONTROL) & 0x8000)
    {
        return true;
    }
    return false;
}

bool isKeyPressed_LEFT()
{
    if(GetAsyncKeyState(VK_LEFT) & 0x8000)
    {
        return true;
    }
    return false;
}

bool isKeyPressed_RIGHT()
{
    if(GetAsyncKeyState(VK_RIGHT) & 0x8000)
    {
        return true;
    }
    return false;
}

bool isKeyPressed_UP()
{
    if(GetAsyncKeyState(VK_UP) & 0x8000)
    {
        return true;
    }
    return false;
}

bool isKeyPressed_DOWN()
{
    if(GetAsyncKeyState(VK_DOWN) & 0x8000)
    {
        return true;
    }
    return false;
}

bool isKeyPressed_SHIFT()
{
    if(GetAsyncKeyState(VK_SHIFT) & 0x8000)
    {
        return true;
    }
    return false;
}

bool isKeyPressed_Q()
{
    if(GetAsyncKeyState(81) & 0x8000)
    {
        return true;
    }
    return false;
}

bool isKeyPressed_W()
{
    if(GetAsyncKeyState(87) & 0x8000)
    {
        return true;
    }
    return false;
}

bool isKeyPressed_E()
{
    if(GetAsyncKeyState(69) & 0x8000)
    {
        return true;
    }
    return false;
}

bool isKeyPressed_A()
{
    if(GetAsyncKeyState(65) & 0x8000)
    {
        return true;
    }
    return false;
}

bool isKeyPressed_S()
{
    if(GetAsyncKeyState(83) & 0x8000)
    {
        return true;
    }
    return false;
}

bool isKeyPressed_D()
{
    if(GetAsyncKeyState(68) & 0x8000)
    {
        return true;
    }
    return false;
}

bool isKeyPressed_Z()
{
    if(GetAsyncKeyState(90) & 0x8000)
    {
        return true;
    }
    return false;
}

bool isKeyPressed_X()
{
    if(GetAsyncKeyState(88) & 0x8000)
    {
        return true;
    }
    return false;
}

bool isKeyPressed_C()
{
    if(GetAsyncKeyState(67) & 0x8000)
    {
        return true;
    }
    return false;
}

bool isKeyPressed_ENTER()
{
    if(GetAsyncKeyState(VK_RETURN) & 0x8000)
    {
        return true;
    }
    return false;
}

bool isKeyPressed_SPACE()
{
    if(GetAsyncKeyState(VK_SPACE) & 0x8000)
    {
        return true;
    }
    return false;
}

bool isKeyPressed_END()
{
    if(GetAsyncKeyState(VK_END) & 0x8000)
    {
        return true;
    }
    return false;
}

bool isKeyPressed_DELETE()
{
    if(GetAsyncKeyState(VK_DELETE) & 0x8000)
    {
        return true;
    }
    return false;
}