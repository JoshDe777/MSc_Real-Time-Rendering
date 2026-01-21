#pragma once

namespace EisEngine {
    class Game;

    namespace ecs{
        /// \n Template class for all types of systems to be used in games made with Eis-Engine.
        class System {
        public:
            /// \n Creates a system instance.
            /// @param engine - a reference to the instance of the game.
            explicit System(Game &engine) : engine(engine) {}
            System(System &&) = delete;
            System(const System &) = delete;
            virtual ~System() = default;
        protected:
            /// \n A reference to the engine instance to access components and entities.
            Game &engine;
        };
    }
}
