#pragma once

#include <functional>

namespace EisEngine::events {
    /// \n Represents an event that listeners can subscribe to.
    /// Events have an owner and an arbitrary number of arguments that are passed to the listener when invoked.
    /// Only the owner is allowed to invoke the event.
    template<typename Owner, typename ...Args>
    class Event {
        friend Owner;
    public:
        using listener_t = typename std::function<void(Args...)>;
        using container_t = typename std::list<listener_t>;
        using handle_t = typename container_t::iterator;

        /// \n Adds an event listener. All registered event listeners are called when the event is invoked.
        /// @param listener - an event listener. Its function signature is defined by the @a listener_t
        /// @return @a handle_t - a handle you can pass to the @a removeListener function use to remove the listener
        handle_t addListener(listener_t listener){
            listeners.push_back(listener);
            return --listeners.end();
        }

        /// \n Removes an event listener. Removed event listeners are no longer called when the event is invoked.
        /// @param handle - the handle returned by the @a addListener function.
        void removeListener(handle_t handle) {
            listeners.erase(handle);
        }

        /// \n Resets the event, removing all listeners.
        void reset(){
            listeners.clear();
        }

    private:
        /// \n Executes all callbacks saved in the event.
        void invoke(Args ...args) {
            container_t unmodifiedCallbacks(listeners);
            if(listeners.empty())
                return;
            for(auto &callback: unmodifiedCallbacks) {
                callback(std::forward<Args>(args) ...);
            }
        }

        /// \n A map of listeners to the current event.
        container_t listeners;
    };
}
