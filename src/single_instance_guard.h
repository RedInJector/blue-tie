//
// Created by Max on 6/2/2025.
//

#ifndef SINGLEINSTANCEGUARD_H
#define SINGLEINSTANCEGUARD_H
#include <memory>

class SingleInstanceGuard {
public:
    virtual ~SingleInstanceGuard() = default;

    [[nodiscard]] virtual bool IsPrimaryInstance() const = 0;

    static std::unique_ptr<SingleInstanceGuard> Create();
};
#endif //SINGLEINSTANCEGUARD_H
