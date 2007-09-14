/***************************************************************************
 *                                                                         *
 *   (c) Art Tevs, MPI Informatik Saarbruecken                             *
 *       mailto: <tevs@mpi-sb.mpg.de>                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef __GLFW_PACKAGE_PLUGIN_H_
#define __GLFW_PACKAGE_PLUGIN_H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include <nrEngine/nrEngine.h>
#include "Event.h"

namespace nrPackage {

    namespace glfw {

        using namespace nrEngine;
        
        //! Forward declaration
        class Task;

        //! Main class of the glfw package
        /**
        * Binding is a main class of the glfw loadtime plugin (or package).
        * It is declared as a singleton, so it can be created only once.
        * This will initialize the subsysems like glfw framework and
        * event management. So you have to create a instance of this class
        * to provide the user the functionality of this package.
        **/
        class _NRPackageExport Package : public nrEngine::Package{

            public:

                /**
                * Default constructor used, to create instance and
                * to initialize all underlying systems.
                *
                * NOTE: You have to instantiate a class after initialization
                *       of the engine, otherwise exception will be thrown
                **/
                Package();

                /**
                * Release used memory. Close glfw subsystem and close
                * the task running in the kernel
                **/
                ~Package();

                /**
                 * @see nrEngine::Package::getName()
                 **/
                const std::string& getName();

                /**
                 * @see nrEngine::Package::getFullName()
                 **/
                const std::string& getFullName();

                /**
                 * Open a window where all OpenGL operations access on.
                 *
                 * @param width,height Size of the window
                 * @param fullscreen If true, create fullscreen
                 * @param bpp Colorbuffer bit depth (default 32)
                 * @param depth Depthbuffer bit depth (default 24)
                 * @param stencil Stencilbuffer bit depth (default 8)
                 * @return true if it succeeds, false otherwise. See log files
                 **/
                bool createWindow(nrEngine::int32 width, nrEngine::int32 height, bool fullscreen, nrEngine::int32 bpp = 32, nrEngine::int32 depth = 24, nrEngine::int32 stencil = 8);

                /**
                 * Close the window and kill all used OpenGL contexts
                 **/
                void closeWindow();

                /**
                 * Set window data, as title, position and size
                 **/
                void setWindow(const std::string& title, nrEngine::int32 width = 0, nrEngine::int32 height = 0);

                /**
                 * Get window title
                 **/
                const std::string& getWindowTitle() const { return mWindowTitle; }
                
                /**
                 * Get the name of the message channel where events are
                 * published
                 **/
                static const std::string& getChannelName();

                /**
                 * Get current mouse position
                 **/
                void getMousePos(nrEngine::int32& x, nrEngine::int32& y);

                /**
                 * Set mouse to new position
                 **/
                void setMousePos(nrEngine::int32 x, nrEngine::int32 y);

                /**
                 * Check if a certain key is down.
                 **/
                bool isKeyDown(nrEngine::keyIndex) const ;
                
                /**
                 * Get current window width
                **/
                nrEngine::int32 getWindowWidth() const { return mWindowWidth; }

                /**
                 * Get current window height
                **/
                nrEngine::int32 getWindowHeight() const { return mWindowHeight; }

                /**
                 * Access to the singleton object
                 **/
                static Package* instance();
    
                /**
                 * Release the singleton object
                 **/
                static void release();
                 
                 /**
                 * Check whenever the engine'S singleton was created before
                 */
                 static bool valid();

                 /**
                  * In normal keys the key event is only initiated if the state of the key is changed.
                  * This means, that you have always to poll for a key state if key is down (i.e. isKeyDown()).
                  * However you can specify keys to emit keypress events on every task update even if they are down on.
                  *
                  * @param keyIndex Index of a key to be monitored
                  **/
                 void addKeyDownMonitor(nrEngine::keyIndex);
                 
                 /**
                 * Remove a key from the key down monitor @see addKeyDownMonitor()
                 **/
                 void delKeyDownMonitor(nrEngine::keyIndex);
                 
            private:

                //! Singleton to hold the package
                static SharedPtr<Package> sSingleton;

                //! Task object of the package
                SharedPtr<Task> mTask;
                
                //! Name of the package
                std::string mName;

                //! Full name
                std::string mFullName;

                //! Is window already create
                std::string mWindowTitle;

                //! Current window size
                nrEngine::int32 mWindowWidth, mWindowHeight;

                //! Is already created
                bool bWindowCreated;
                
                //! Callback function to close the window
                static nrEngine::int32 closeWindowCallback();

                ScriptFunctionDef(resizeWindow);
                ScriptFunctionDef(openWindow);
                ScriptFunctionDef(setWindowTitle);

        };

    };
};

#endif

