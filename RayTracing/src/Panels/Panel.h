#pragma once

#include <functional>

class Panel {
public:
	Panel() = default;
	~Panel() = default;

	virtual bool OnUIRender() = 0;
};