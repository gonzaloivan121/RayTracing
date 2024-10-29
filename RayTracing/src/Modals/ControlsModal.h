#pragma once

#include "Modal.h"

class ControlsModal : public Modal {
public:
	ControlsModal(bool& isOpen);

	virtual void OnUIRender() override;
private:
	bool& m_IsOpen;
};