#include <vector>

#include "Core/TravailleurContext.h"
#include "DummyApp1.h"
#include "DummyApp2.h"

int main()
{
    DummyApp1 dummyApp1;
    DummyApp2 dummyApp2;

    std::vector<TravailleurApp*> apps;
    apps.push_back(&dummyApp1);
    apps.push_back(&dummyApp2);

    TravailleurContext travailleurContext("DummyContext", apps);
    travailleurContext.startContext();
    return 0;
}