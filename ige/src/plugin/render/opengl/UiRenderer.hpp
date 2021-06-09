#ifndef FFE76428_D275_401F_91CB_CCA396785D99
#define FFE76428_D275_401F_91CB_CCA396785D99

#include "ige/core/App.hpp"

class UiRenderer : public ige::core::App::Plugin {
public:
    void plug(ige::core::App::Builder&) const override;
};

#endif /* FFE76428_D275_401F_91CB_CCA396785D99 */
