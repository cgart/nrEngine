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

#include "glew.h"
#include "Package.h"
#include "Task.h"
#include "glfw.h"

namespace nrPackage {
    namespace glfw{

        using namespace nrEngine;

        SharedPtr<Package> Package::sSingleton;

        //------------------------------------------------------------------------
        ScriptFunctionDec(resizeWindow, Package)
        {
            // check if the parameter count is valid
            if (args.size() <= 2){
                return ScriptResult(std::string("resizeWindow(w,h) - wrong parameter count"));
            }

            // convert the given values
            int32 width = 0;
            int32 height = 0;
            try{
                width = boost::lexical_cast<int32>(args[1]);
                height = boost::lexical_cast<int32>(args[2]);
            }catch(...){
                return ScriptResult(std::string("Given values are not valid!"));
            }
            
            // resize window
            Package::instance()->setWindow(Package::instance()->getWindowTitle(), width, height);
            return ScriptResult();
        }

        //------------------------------------------------------------------------
        ScriptFunctionDec(setWindowTitle, Package)
        {
            // check if the parameter count is valid
            if (args.size() <= 1){
                return ScriptResult(std::string("setWindowTitle(title) - wrong parameter count"));
            }

            // combine the values
            std::string title;
            for (uint32 i=1; i < args.size(); i++)
                title += (args[i] + " ");
            
            // set title
            Package::instance()->setWindow(title);
            return ScriptResult();
        }
        
        //------------------------------------------------------------------------
        ScriptFunctionDec(openWindow, Package)
        {

            // check if the parameter count is valid
            if (args.size() <= 3){
                return ScriptResult(std::string("openWindow(w,h,full[,bpp,depth,stencil]) - wrong parameter count"));
            }

            // convert given values
            int32 w,h;
            int32 bpp = 32;
            int32 depth = 16;
            int32 stencil = 8;
            bool full = false;

            try{

                w = boost::lexical_cast<int32>(args[1]);
                h = boost::lexical_cast<int32>(args[2]);
                full = boost::lexical_cast<bool>(args[3]);
                if (args.size() > 4) bpp = boost::lexical_cast<int32>(args[4]);
                if (args.size() > 5) depth = boost::lexical_cast<int32>(args[5]);
                if (args.size() > 6) stencil = boost::lexical_cast<int32>(args[6]);
                
            }catch(...){
                return ScriptResult(std::string("Wrong parameters used!"));
            }

            Package::instance()->createWindow(w,h,full,bpp,depth,stencil);
            return ScriptResult();
        }


        //------------------------------------------------------------------------
        Package* Package::instance()
        {
            if (!valid()){
                sSingleton.reset(new Package());
            }
            return sSingleton.get();
        }

        //------------------------------------------------------------------------
        void Package::release()
        {
            if (valid())
            {
                sSingleton.reset();
            }
        }

        //--------------------------------------------------------------------------
        bool Package::valid()
        {
            return sSingleton != NULL;
        }

        //----------------------------------------------------------------------
        Package::Package()
        {
            // initialize default variables
            mName = "glfwPackage";
            mFullName = "OpenGL Framework Library v0.2 for nrEngine";
            int32 major, minor, rev;
            glfwGetVersion(&major, &minor, &rev);
            NR_Log(Log::LOG_PLUGIN, "%s: %s (glfw v%d.%d.%d)",mName.c_str(), mFullName.c_str(), major, minor, rev);
            bWindowCreated = false;
            mWindowWidth = mWindowHeight = 0;

            // check whenever engine is valid
            if (!Engine::valid())
            {
                NR_Log(Log::LOG_PLUGIN, Log::LL_ERROR, "%s: Engine must be initialized before!", mName.c_str());
                NR_EXCEPT(ENGINE_ERROR, "Engine must be initialized before", "glfw::Package::Package()");
            }

            // initialize glfw task
            NR_Log(Log::LOG_PLUGIN, "glfwPackage: Initialize the glfw subsystem (OpenGL Framework)");
            try{
                Engine* root = Engine::instance();
                mTask.reset(new Task(root));
            }catch(int i){
                if (i == GLFW_CANNOT_INITIALIZE){
                    NR_Log(Log::LOG_PLUGIN, Log::LL_ERROR, "%s: Initialization of glfw fails! glfwinit() returns GL_FALSE", mName.c_str());
                }else if (i == GLFW_TASK_ALREADY_RUNNING) {
                    NR_Log(Log::LOG_PLUGIN, Log::LL_ERROR, "%s: You are trying to start the same plugin twice.", mName.c_str());
                }
            }

            // ok task is created now add it to the kernel
            if (mTask)
            {
                if (Engine::sKernel()->AddTask(mTask, ORDER_LAST) == 0){
                    NR_Log(Log::LOG_PLUGIN, Log::LL_ERROR, "%s: glfwPackage_Task could not been added to the kernel", mName.c_str());
                }
            }

            // now register some new function by the scripting engine
            Engine::sScriptEngine()->add("openWindow", openWindow);
            Engine::sScriptEngine()->add("resizeWindow", resizeWindow);
            Engine::sScriptEngine()->add("setWindowTitle", setWindowTitle);
            
        }

        //----------------------------------------------------------------------
        Package::~Package()
        {
            // remove the registered functions
            Engine::sScriptEngine()->del("openWindow");
            Engine::sScriptEngine()->del("resizeWindow");
            Engine::sScriptEngine()->del("setWindowTitle");
            
            NR_Log(Log::LOG_PLUGIN, "glfwPackage: Terminate the glfw subsystem");

            // first close the rendering window
            closeWindow();

            // remove the task from the system
            if (Engine::valid())
                Engine::sKernel()->RemoveTask(mTask->getTaskID());

            // close the task
            mTask.reset();
        }

        //----------------------------------------------------------------------
        const std::string& Package::getName()
        {
            return mName;
        }

        //----------------------------------------------------------------------
        const std::string& Package::getFullName()
        {
            return mFullName;
        }

        //----------------------------------------------------------------------
        const std::string& Package::getChannelName()
        {
            // cast the task to the glfwTask
            return Task::getChannelName();
        }

        //----------------------------------------------------------------------
        bool Package::createWindow(int32 width, int32 height, bool fullscreen, int32 bpp, int32 depth, int32 stencil)
        {
            if (bWindowCreated) {
                setWindow(mWindowTitle, width, height);
                return true;
            }
            
            // convert given bpp to bits
            int32 r,g,b,a;
            if (bpp == 8){
                r = 3; g = 2; b = 3; a = 0;
            }else if (bpp == 16){
                r = 5; g = 6; b = 5; a = 0;
            }else if (bpp == 24){
                r = 8; g = 8; b = 8; a = 0;
            }else if (bpp == 32){
                r = 8; g = 8; b = 8; a = 8;
            }else if (bpp == 0){
                r = 0; g = 0; b = 0; a = 0;
            }else{
                NR_Log(Log::LOG_PLUGIN, Log::LL_ERROR, "%s: Could not match given colorbit depth", mName.c_str());
                return false;
            }

            // create a rendering window as requested
            int32 res = glfwOpenWindow(width, height, r,g,b,a, depth, stencil, fullscreen ? GLFW_FULLSCREEN : GLFW_WINDOW);
            if (res == GL_FALSE){
                NR_Log(Log::LOG_PLUGIN, Log::LL_ERROR, "%s: Could not open a rendering window", mName.c_str());
                return false;
            }
            mWindowWidth = width;
            mWindowHeight = height;

            // initialize glew, so we get all extensions initialized
            NR_Log(Log::LOG_PLUGIN, "glfwPackage: Initialize the glew subsystem (OpenGL extension wrapper)");
            GLenum err = glewInit();
            if (GLEW_OK != err){
                NR_Log(Log::LOG_PLUGIN, Log::LL_ERROR, "%s: Initialization of glew failed! glewinit() returns error: %s", mName.c_str(), glewGetErrorString(err));
                return false;
            }else{
                NR_Log(Log::LOG_PLUGIN, "glfwPackage: Using GLEW %s\n", glewGetString(GLEW_VERSION));
            }

            // now create a callback function, so we get noticed if user close the window
            glfwSetWindowCloseCallback(Package::closeWindowCallback);

            // now emit an event on the channel, that we got opened a window
            SharedPtr<Event> msg(new OnCreateWindowEvent(width, height, fullscreen, bpp, depth, stencil));
            Engine::sEventManager()->emit(Package::getChannelName(), msg);

            // now notice the task, that we have created a window
            boost::dynamic_pointer_cast<Task, ITask>(mTask)->noticeWindowCreated();

            // now get current OpenGL version
            const char* renderer = (const char*)glGetString(GL_RENDERER);
            const char* vendor = (const char*)glGetString(GL_VENDOR);

            NR_Log(Log::LOG_PLUGIN, "%s: OpenGL-Driver %s %s", mName.c_str(), vendor, renderer);
            bWindowCreated = true;
            
            return true;
        }

        //----------------------------------------------------------------------
        void Package::closeWindow()
        {
            // now emit an event on the channel, that we got to close the window
            SharedPtr<Event> msg(new OnCloseWindowEvent());
            Engine::sEventManager()->emit(Package::getChannelName(), msg);

            glfwCloseWindow();
        }

        //----------------------------------------------------------------------
        int32 Package::closeWindowCallback()
        {
            // now emit an event on the channel, that we got to close the window
            SharedPtr<Event> msg(new OnCloseWindowEvent());
            Engine::sEventManager()->emit(Package::getChannelName(), msg);

            return GL_TRUE;
        }

        //----------------------------------------------------------------------
        void Package::setWindow(const std::string& title, nrEngine::int32 width, nrEngine::int32 height)
        {
            mWindowTitle = title;
            if (width || height ) {
                glfwSetWindowSize(width, height);
                mWindowWidth = width;
                mWindowHeight = height;

                SharedPtr<Event> msg(new OnResizeWindowEvent(width, height));
                Engine::sEventManager()->emit(Package::getChannelName(), msg);
            }
            
            glfwSetWindowTitle(title.c_str());
        }

        //----------------------------------------------------------------------
        void Package::getMousePos(int& x, int& y)
        {
            mTask->getMousePos(x,y);
        }

        //----------------------------------------------------------------------
        void Package::setMousePos(int x, int y)
        {
            mTask->setMousePos(x,y);
        }

        //----------------------------------------------------------------------
        bool Package::isKeyDown(nrEngine::keyIndex key) const
        {
            return mTask->isKeyDown(key);
        }
        
        //----------------------------------------------------------------------
        void Package::addKeyDownMonitor(nrEngine::keyIndex key)
        {
            mTask->addKeyDownMonitor(key); 
        }
        
        //----------------------------------------------------------------------
        void Package::delKeyDownMonitor(nrEngine::keyIndex key)
        {
            mTask->delKeyDownMonitor(key);
        }
        
        
    };
};
