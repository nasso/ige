/*
** EPITECH PROJECT, 2021
** un
** File description:
** IStorage
*/

#ifndef B41B6C35_5EBE_4406_927A_68F6084106B9
#define B41B6C35_5EBE_4406_927A_68F6084106B9

#include "rtl/Option.hpp"

template <typename K, typename V>
class IStorage {
public:
    virtual ~IStorage() = default;

    virtual void set(K&&, V&&) = 0;
    virtual rtl::Option<const V&> get(const K&) const = 0;
    virtual rtl::Option<V&> get(const K&) = 0;
    virtual rtl::Option<V> remove(const K&) = 0;
};

#endif /* B41B6C35_5EBE_4406_927A_68F6084106B9 */
