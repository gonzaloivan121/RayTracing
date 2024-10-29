#pragma once

#include "Walnut/UI/UI.h"

class Modal {
public:
	Modal() = default;
	~Modal() = default;

	virtual void OnUIRender() = 0;
};