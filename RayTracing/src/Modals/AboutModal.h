#pragma once

#include "Modal.h"

class AboutModal : public Modal {
public:
	AboutModal(bool& isOpen);

	virtual void OnUIRender() override;
private:
	bool& m_IsOpen;
};