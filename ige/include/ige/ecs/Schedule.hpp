#ifndef C36AD5E7_BDB7_4987_A83E_C6A2BB423A01
#define C36AD5E7_BDB7_4987_A83E_C6A2BB423A01

#include "System.hpp"
#include "World.hpp"
#include <concepts>
#include <vector>

namespace ige {
namespace ecs {

    class Schedule {
    private:
        std::vector<System> m_systems;

        Schedule(std::vector<System>);

    public:
        class Builder {
        private:
            std::vector<System> m_systems;

        public:
            template <typename F>
            requires std::constructible_from<System, F> Builder& add_system(
                F&& sys)
            {
                m_systems.emplace_back(std::forward<F>(sys));

                return *this;
            }

            Schedule build() const;
        };

        Schedule() = default;

        void operator()(World&);
    };

}
}

#endif /* C36AD5E7_BDB7_4987_A83E_C6A2BB423A01 */
