#ifndef DummyApp2_H
#define DummyApp2_H

#include "Core/TravailleurContext.h"

class DummyApp2 : public TravailleurApp
{
public:
	std::string getName() const override;
	void setup() override;
	void processFrame() override;
	void showPreRunGui() override;
	void showRunningGui() override;
};

#endif