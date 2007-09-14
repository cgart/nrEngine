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


#ifndef _NR_GLFW_PACKAGE_TASK_H_
#define _NR_GLFW_PACKAGE_TASK_H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include <nrEngine/nrEngine.h>

#define GLFW_CANNOT_INITIALIZE ( 1 << 0 )
#define GLFW_TASK_ALREADY_RUNNING ( 1 << 1)

namespace nrPackage {

    namespace glfw {

        /**
        * glfwTask is a task provided by this plugin running in the engine's kernel.
        * The work of a task will be to poll the events from the glfw subsystem
        * and to send events through the engine's communication channel
        * to message listeners.
        *
        * The class also defines some callback functions which will be called
        * from the glfw subsystem and emit some events to nrEngine's message
        * listeners.
        **/
        class Task : public nrEngine::ITask{
            public:

                /**
                * Initialize glfw subsystem and check if such task is
                * already in the kernel. If it is a case, so throw an error.
                * The error must be catched by the calling functions, so
                * we can tell the application which loads the plugin, that
                * there is already a plugin running such a task.
                **/
                Task(nrEngine::Engine* root);

                /**
                * Terminate glfw subsystem and release used memory
                **/
                ~Task();

                /**
                * update the task
                **/
                nrEngine::Result updateTask();

                /**
                 * Window was opened, so do the stuff needed for this
                 **/
                void noticeWindowCreated();

                /**
                 * Get current mouse position
                 **/
                void getMousePos(int& x, int& y);

                /**
                 * Set mouse to new position
                 **/
                void setMousePos(int x, int y);

                /**
                 * Check if a certain key is down
                 **/
                bool isKeyDown(nrEngine::keyIndex) const;

                /**
                 * Get the channel name where events are commited
                 **/
                static const std::string& getChannelName() ;

                //! Channel name
                static std::string ChannelName;

                void addKeyDownMonitor(nrEngine::keyIndex);
                void delKeyDownMonitor(nrEngine::keyIndex);

            private:

                //! Here we store all the keys which sould be monitored for key down 
                std::vector<nrEngine::keyIndex> mMonitoredKeys;                

                //! Engine's root pointer
                nrEngine::Engine* mEngine;

                //! Call this callback if a key press event was emited from the glfw
                static void keyCallback(int key, int action);

                //! Call this callback if a character key press event was emited from the glfw
                static void keyCharCallback(int character, int action);

                //! Convert the glfw key index into engine's one
                static nrEngine::keyIndex convert(int key);

                //! Convert the glfw key index into engine's one
                static int inv_convert(nrEngine::keyIndex);

                //! Callback for any mouse movements
                static void mousePosCallback (int x, int y);

                //! Store current mouse position
                static nrEngine::int32 mMouseX, mMouseY;

        };
    };
};

#endif  //_NR...
