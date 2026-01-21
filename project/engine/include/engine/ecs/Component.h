#pragma once

#include "ecs.h"
#include "engine/Utilities.h"

namespace EisEngine{
    class Game;

    namespace ecs {
        /// \n A generic class for Components in Eis-Engine's Entity-Component-System
        /// \n Represents any form of data attributed to an entity.
        /// \n Components all derive directly or indirectly from this class.
        class Component {
            friend class ComponentManager;
            friend class Entity;
        public:
            /// \n Constructs the component and assigns it to the owner.
            explicit Component(Game &engine, guid_t owner = invalidID);
            /// \n Flags the component for deletion.
            virtual ~Component() = default;

            /// \n Returns the unique ID of the entity attached to this component.
            [[nodiscard]] guid_t GetOwner() const { return owner;}
            /// \n Determines whether a component has been flagged for deletion.
            [[nodiscard]] bool isDeleted() const { return deleted; }
            /// \n A pointer to the entity owning this component.
            Entity *entity() const;
        protected:
            /// \n A function made to encompass what needs to be done when a component is intentionally deleted.
            virtual void Invalidate() { deleted = true;}

            /// \n The unique ID of the entity this component is assigned to.
            guid_t owner;
            /// \n Determines whether the component is marked for deletion.
            bool deleted = false;
            /// \n A reference to the engine instance to access the entity manager.
            Game &engine;
        };
    }
}
