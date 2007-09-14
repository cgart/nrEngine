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


#include "Task.h"
#include "glfw.h"
#include "Event.h"

#include <boost/function.hpp>
#define TASK_NAME "glfwPackage"

namespace nrPackage {

    namespace glfw{

        std::string Task::ChannelName(TASK_NAME);
        nrEngine::int32 Task::mMouseX = 0;
        nrEngine::int32 Task::mMouseY = 0;

        using namespace nrEngine;

        //------------------------------------------------------------
        Task::Task(nrEngine::Engine* root) : mEngine(root)
        {
            setTaskName(TASK_NAME);

            // check for engine
            if (mEngine == NULL) throw(-1);

            // check if such a task already running
            if (mEngine->sKernel()->getTaskByName(TASK_NAME)) throw GLFW_TASK_ALREADY_RUNNING;

            // ok let's initialize the glfw subsystem
            if (glfwInit() == GL_FALSE) throw GLFW_CANNOT_INITIALIZE;

            // create a communication channel
            mEngine->sEventManager()->createChannel(Task::ChannelName);
        }

        //------------------------------------------------------------
        Task::~Task()
        {
            glfwTerminate();
        }

        //------------------------------------------------------------
        void Task::noticeWindowCreated()
        {
            // propaget the key callback function
            glfwSetKeyCallback(keyCallback);
            glfwSetCharCallback(keyCharCallback);
            glfwSetMousePosCallback(mousePosCallback);
        }

        //------------------------------------------------------------
        const std::string& Task::getChannelName(){
            return Task::ChannelName;
        }


        //------------------------------------------------------------
        Result Task::updateTask()
        {
            // swap buffers and thus poll for new events
            glfwSwapBuffers();
            
            // now check for monitored keys
            std::vector<nrEngine::keyIndex>::iterator it = mMonitoredKeys.begin();
            for (; it != mMonitoredKeys.end(); it++)
            {
                // if key is down, then send event
                if (isKeyDown(*it))
                {
                    SharedPtr<Event> msg(new OnKeyboardDownEvent(*it));
                    Engine::sEventManager()->emit(Task::getChannelName(), msg);
                }
            }
            
                        
            // all things all right
            return OK;
        }

        //------------------------------------------------------------
        void Task::keyCallback(int key, int action)
        {
            keyIndex nrkey = convert(key);
            SharedPtr<Event> msg;
            
            // the key was pressed
            if (action == GLFW_PRESS){

                // send a message that the key was pressed
                msg.reset(new OnKeyboardPressEvent(nrkey));

            // key is released
            }else if (action == GLFW_RELEASE){

                // send a message that key was released
                msg.reset(new OnKeyboardReleaseEvent(nrkey));

            }

            // send to the input chanel
            Engine::sEventManager()->emit(Task::getChannelName(), msg);
        }


        //------------------------------------------------------------
        void Task::keyCharCallback(int character, int action)
        {

        }

        //------------------------------------------------------------
        void Task::mousePosCallback (int x, int y)
        {
            SharedPtr<Event> msg (new OnMouseMoveEvent(x,y, mMouseX, mMouseY));
            Engine::sEventManager()->emit(Task::getChannelName(), msg);
            mMouseX = x;
            mMouseY = y;
        }

        //----------------------------------------------------------------------
        void Task::getMousePos(int& x, int& y)
        {
            x = mMouseX;
            y = mMouseY;
        }

        //----------------------------------------------------------------------
        void Task::setMousePos(int x, int y)
        {
            mMouseX = x;
            mMouseY = y;
            glfwSetMousePos(x,y);
        }

        //------------------------------------------------------------
        bool Task::isKeyDown(nrEngine::keyIndex key) const
        {
            return glfwGetKey(inv_convert(key)) == GLFW_PRESS;
        }
        
        //----------------------------------------------------------------------
        void Task::addKeyDownMonitor(nrEngine::keyIndex key)
        {
            // check if such a key index already exists in the database
            for (unsigned int i = 0; i < mMonitoredKeys.size(); i++)
                if (mMonitoredKeys[i] == key) return;
                
            // add the key to the list
            mMonitoredKeys.push_back(key);        
        }
        
        //----------------------------------------------------------------------
        void Task::delKeyDownMonitor(nrEngine::keyIndex key)
        {
            // go through the database and check for the key
            std::vector<nrEngine::keyIndex>::iterator it = mMonitoredKeys.begin();
            for (; it != mMonitoredKeys.end(); it++)
            {
                if (*it == key)
                {   
                    it = mMonitoredKeys.erase(it);
                    return;
                }
             }
        }

        //------------------------------------------------------------
        keyIndex Task::convert(int key)
        {
            #define MAP(glfw, nrkey) if (key == glfw) return nrkey;

            MAP(GLFW_KEY_SPACE, KEY_SPACE)
            MAP(GLFW_KEY_ESC, KEY_ESCAPE)

            MAP(GLFW_KEY_F1, KEY_F1)
            MAP(GLFW_KEY_F2, KEY_F2)
            MAP(GLFW_KEY_F3, KEY_F3)
            MAP(GLFW_KEY_F4, KEY_F4)
            MAP(GLFW_KEY_F5, KEY_F5)
            MAP(GLFW_KEY_F6, KEY_F6)
            MAP(GLFW_KEY_F7, KEY_F7)
            MAP(GLFW_KEY_F8, KEY_F8)
            MAP(GLFW_KEY_F9, KEY_F9)
            MAP(GLFW_KEY_F10, KEY_F10)
            MAP(GLFW_KEY_F11, KEY_F11)
            MAP(GLFW_KEY_F12, KEY_F12)
            MAP(GLFW_KEY_F13, KEY_F13)
            MAP(GLFW_KEY_F14, KEY_F14)
            MAP(GLFW_KEY_F15, KEY_F15)

            MAP(GLFW_KEY_UP, KEY_UP)
            MAP(GLFW_KEY_DOWN, KEY_DOWN)
            MAP(GLFW_KEY_LEFT, KEY_LEFT)
            MAP(GLFW_KEY_RIGHT, KEY_RIGHT)

            MAP(GLFW_KEY_LSHIFT, KEY_LSHIFT)
            MAP(GLFW_KEY_RSHIFT, KEY_RSHIFT)
            MAP(GLFW_KEY_LALT, KEY_LALT)
            MAP(GLFW_KEY_RALT, KEY_RALT)
            MAP(GLFW_KEY_LCTRL, KEY_LCTRL)
            MAP(GLFW_KEY_LCTRL, KEY_LCTRL)

            MAP(GLFW_KEY_TAB, KEY_TAB)
            MAP(GLFW_KEY_ENTER, KEY_RETURN)
            MAP(GLFW_KEY_BACKSPACE, KEY_BACKSPACE)
            MAP(GLFW_KEY_INSERT, KEY_INSERT)
            MAP(GLFW_KEY_DEL, KEY_DELETE)
            MAP(GLFW_KEY_PAGEUP, KEY_PAGEUP)
            MAP(GLFW_KEY_PAGEDOWN, KEY_PAGEDOWN)
            MAP(GLFW_KEY_HOME, KEY_HOME)
            MAP(GLFW_KEY_END, KEY_END)

            MAP(GLFW_KEY_KP_0, KEY_KP0)
            MAP(GLFW_KEY_KP_1, KEY_KP1)
            MAP(GLFW_KEY_KP_2, KEY_KP2)
            MAP(GLFW_KEY_KP_3, KEY_KP3)
            MAP(GLFW_KEY_KP_4, KEY_KP4)
            MAP(GLFW_KEY_KP_5, KEY_KP5)
            MAP(GLFW_KEY_KP_6, KEY_KP6)
            MAP(GLFW_KEY_KP_7, KEY_KP7)
            MAP(GLFW_KEY_KP_8, KEY_KP8)
            MAP(GLFW_KEY_KP_9, KEY_KP9)

            MAP(GLFW_KEY_KP_DIVIDE, KEY_KP_DIVIDE)
            MAP(GLFW_KEY_KP_MULTIPLY, KEY_KP_MULTIPLY)
            MAP(GLFW_KEY_KP_SUBTRACT, KEY_KP_MINUS)
            MAP(GLFW_KEY_KP_ADD, KEY_KP_PLUS)
            MAP(GLFW_KEY_KP_DECIMAL, KEY_KP_PERIOD)
            MAP(GLFW_KEY_KP_EQUAL, KEY_KP_EQUALS)
            MAP(GLFW_KEY_KP_ENTER, KEY_KP_ENTER)

            #undef MAP
            return KEY_UNKNOWN;
        }

        //------------------------------------------------------------
        int Task::inv_convert(nrEngine::keyIndex key)
        {
            if (key >= KEY_a && key <= KEY_z) {
                return int('A') + int(key) - int(KEY_a);
            }
            
            #define MAP(glfw, nrkey) if (key == nrkey) return glfw;

            MAP(GLFW_KEY_SPACE, KEY_SPACE)
            MAP(GLFW_KEY_ESC, KEY_ESCAPE)

            MAP(GLFW_KEY_F1, KEY_F1)
            MAP(GLFW_KEY_F2, KEY_F2)
            MAP(GLFW_KEY_F3, KEY_F3)
            MAP(GLFW_KEY_F4, KEY_F4)
            MAP(GLFW_KEY_F5, KEY_F5)
            MAP(GLFW_KEY_F6, KEY_F6)
            MAP(GLFW_KEY_F7, KEY_F7)
            MAP(GLFW_KEY_F8, KEY_F8)
            MAP(GLFW_KEY_F9, KEY_F9)
            MAP(GLFW_KEY_F10, KEY_F10)
            MAP(GLFW_KEY_F11, KEY_F11)
            MAP(GLFW_KEY_F12, KEY_F12)
            MAP(GLFW_KEY_F13, KEY_F13)
            MAP(GLFW_KEY_F14, KEY_F14)
            MAP(GLFW_KEY_F15, KEY_F15)

            MAP(GLFW_KEY_UP, KEY_UP)
            MAP(GLFW_KEY_DOWN, KEY_DOWN)
            MAP(GLFW_KEY_LEFT, KEY_LEFT)
            MAP(GLFW_KEY_RIGHT, KEY_RIGHT)

            MAP(GLFW_KEY_LSHIFT, KEY_LSHIFT)
            MAP(GLFW_KEY_RSHIFT, KEY_RSHIFT)
            MAP(GLFW_KEY_LALT, KEY_LALT)
            MAP(GLFW_KEY_RALT, KEY_RALT)
            MAP(GLFW_KEY_LCTRL, KEY_LCTRL)
            MAP(GLFW_KEY_LCTRL, KEY_LCTRL)

            MAP(GLFW_KEY_TAB, KEY_TAB)
            MAP(GLFW_KEY_ENTER, KEY_RETURN)
            MAP(GLFW_KEY_BACKSPACE, KEY_BACKSPACE)
            MAP(GLFW_KEY_INSERT, KEY_INSERT)
            MAP(GLFW_KEY_DEL, KEY_DELETE)
            MAP(GLFW_KEY_PAGEUP, KEY_PAGEUP)
            MAP(GLFW_KEY_PAGEDOWN, KEY_PAGEDOWN)
            MAP(GLFW_KEY_HOME, KEY_HOME)
            MAP(GLFW_KEY_END, KEY_END)

            MAP(GLFW_KEY_KP_0, KEY_KP0)
            MAP(GLFW_KEY_KP_1, KEY_KP1)
            MAP(GLFW_KEY_KP_2, KEY_KP2)
            MAP(GLFW_KEY_KP_3, KEY_KP3)
            MAP(GLFW_KEY_KP_4, KEY_KP4)
            MAP(GLFW_KEY_KP_5, KEY_KP5)
            MAP(GLFW_KEY_KP_6, KEY_KP6)
            MAP(GLFW_KEY_KP_7, KEY_KP7)
            MAP(GLFW_KEY_KP_8, KEY_KP8)
            MAP(GLFW_KEY_KP_9, KEY_KP9)

            MAP(GLFW_KEY_KP_DIVIDE, KEY_KP_DIVIDE)
            MAP(GLFW_KEY_KP_MULTIPLY, KEY_KP_MULTIPLY)
            MAP(GLFW_KEY_KP_SUBTRACT, KEY_KP_MINUS)
            MAP(GLFW_KEY_KP_ADD, KEY_KP_PLUS)
            MAP(GLFW_KEY_KP_DECIMAL, KEY_KP_PERIOD)
            MAP(GLFW_KEY_KP_EQUAL, KEY_KP_EQUALS)
            MAP(GLFW_KEY_KP_ENTER, KEY_KP_ENTER)

            #undef MAP
            return -1;
        }

    };
};

