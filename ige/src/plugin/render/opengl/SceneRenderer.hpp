#ifndef F5506B1A_F18B_46CC_AFDF_6E5FE3A63B75
#define F5506B1A_F18B_46CC_AFDF_6E5FE3A63B75

#include "ige/core/App.hpp"

class SceneRenderer : public ige::core::App::Plugin {
public:
    void plug(ige::core::App::Builder&) const override;
};

#endif /* F5506B1A_F18B_46CC_AFDF_6E5FE3A63B75 */
