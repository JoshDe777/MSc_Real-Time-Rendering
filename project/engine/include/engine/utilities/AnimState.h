#pragma once

#include <functional>
#include <utility>

namespace EisEngine {
    class AnimState {
    public:
        explicit AnimState(
                std::function<void(const float&)> onRun,
                std::function<void()> onEntering,
                std::function<void()> onExiting,
                std::function<bool()> exitCondition) :
                onRun(std::move(onRun)), onEntering(std::move(onEntering)),
                onExiting(std::move(onExiting)), exitCondition(std::move(exitCondition)) {}
        bool run(const float& elapsedTime);
        void onEnter() { onEntering();}
    private:
        std::function<void(const float&)> onRun;
        std::function<void()> onEntering;
        std::function<void()> onExiting;
        std::function<bool()> exitCondition;
    };
}
