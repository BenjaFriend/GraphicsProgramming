#pragma once

#include "stdafx.h"
#include "Utils/Dispatcher.hpp"
#include "IInput_Impl.h"

#include <stdio.h>
#include <unordered_map>
#include <vector>

namespace Input
{

    /// <summary>
    /// Input manager in charge of checking when input is happening
    /// </summary>
    class InputManager
    {
        typedef void( *input_action_func )( );

    public:

        template <typename T>
        static InputManager* Initalize()
        {
            assert( Instance == nullptr );

            static_assert(
                std::is_base_of<IInput_Impl, T>::value,
                "T is not derived from IInput_Impl. InputManager::Initalize<T>"
                );

            IInput_Impl* impl = new T();

            Instance = new InputManager( impl );

            return Instance;
        }

        /// <summary>
        /// Getter for the InputManager instance
        /// </summary>
        /// <returns>Singleton instance of the input manager</returns>
        static InputManager* GetInstance();

        /// <summary>
        ///  Release the instance of the input manager
        /// </summary>
        static void ReleaseInstance();

        /// <summary>
        /// Bind an action to an input type. A bound action will be called when 
        /// this type of input is detected by the input manager
        /// </summary>
        /// <param name="parentObj">Object to call the action on</param>
        /// <param name="inputListenerFunc">Function to call</param>
        /// <param name="inputType">Type of input to listen for</param>
        template<class T>
        void BindAction( T* parentObj, void ( T::*inputListenerFunc )( ), InputType inputType )
        {
            actionListeners[ inputType ].BindListener( parentObj, inputListenerFunc );
        }

        /// <summary>
        /// Check for input in the input manager and signal dispatchers as necessary
        /// </summary>
        /// <param name="dt">delta time of the frame</param>
        void Update( float dt );

        /* The reason why there are two different 'IsKeyDown' functions */
        /* Sol and Lua don't properly cast (int <--> char) values */
        /* 'IsCKeyDown' only works with capital chars */
        /// <summary>
        /// Test if a key is down
        /// </summary>
        /// <param name="vKey">Int value of the key</param>
        /// <returns>True if the key is down</returns>
        bool IsKeyDown( int vKey );

        /// <summary>
        /// Test if a key is down
        /// </summary>
        /// <param name="vKey">Char value of the key</param>
        /// <returns>True if the key is down</returns>
        bool IsCKeyDown( char vKey );

        bool IsInputTypeDown( InputType inputType );

        /// <summary>
        /// Check if a certain key is UP
        /// </summary>
        /// <param name="vKey">Key to check</param>
        /// <returns>True if key is up</returns>
        bool IsKeyUp( int vKey );

        void OnLookDown( WPARAM buttonState, int x, int y );
        void OnLookUp( WPARAM buttonState, int x, int y );

        // Windows specific input callbacks
#if defined( _WIN32 ) || defined( _WIN64 )

        void OnMouseDown( WPARAM buttonState, int x, int y );
        void OnMouseUp( WPARAM buttonState, int x, int y );
        void OnMouseMove( WPARAM buttonState, int x, int y );

        glm::vec2 GetMousePosition() const;

#endif  // _WIN32 || _WIN64

    protected:

        /** Vector of input bindings to keys/buttons */
        std::vector<InputBinding> InputBinds;

        std::vector< bool > PreviousInputState;

    private:

        /// <summary>
        /// Private constructor so that no objects can be created
        /// </summary>
        InputManager( IInput_Impl* impl );

        /// <summary>
        /// Destructor for the input manager
        /// </summary>
        ~InputManager();

        glm::vec2 curMousePos = glm::vec2( 0, 0 );

        /** The instance of the InputManager */
        static InputManager* Instance;

        /** The implementation of any methods to that depend on platform */
        IInput_Impl* Implementation = nullptr;

        /** A map of active listeners */
        std::unordered_map<InputType, Dispatcher> actionListeners;

        /// <summary>
        /// Signal to any listeners of this input type that it should happen
        /// </summary>
        /// <param name="type">The input type</param>
        void SignalInput( InputType type );



    };  // Class InputManager

}   // namespace Input