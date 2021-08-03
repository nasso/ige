#ifndef C4543C72_0155_415B_AFDE_A96665B96550
#define C4543C72_0155_415B_AFDE_A96665B96550

#include <concepts>

namespace ige::utility {

/**
 * @brief Object that calls a function when it is destroyed.
 *
 * @tparam F The type of the function to call.
 */
template <std::invocable<> F>
class CallOnExit {
public:
    CallOnExit(F&& f);
    ~CallOnExit();

private:
    F m_callback;
};

}

#endif /* C4543C72_0155_415B_AFDE_A96665B96550 */
