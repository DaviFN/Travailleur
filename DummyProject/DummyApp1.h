#ifndef DummyApp1_H
#define DummyApp1_H

#include "Core/TravailleurContext.h"

class DummyApp1 : public TravailleurApp
{
public:
	std::string getName() const override;
	void setup() override;
	void processFrame() override;
	void showPreRunGui() override;
	void showRunningGui() override;

private:
	void showDummyFrame1Gui();
};

#endif