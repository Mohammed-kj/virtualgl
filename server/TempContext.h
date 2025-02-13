// Copyright (C)2004 Landmark Graphics Corporation
// Copyright (C)2005, 2006 Sun Microsystems, Inc.
// Copyright (C)2011, 2014-2015, 2018-2021 D. R. Commander
//
// This library is free software and may be redistributed and/or modified under
// the terms of the wxWindows Library License, Version 3.1 or (at your option)
// any later version.  The full license is in the LICENSE.txt file included
// with this distribution.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// wxWindows Library License for more details.

// This is a container class that allows us to temporarily swap in a new
// drawable and then "pop the stack" after we're done with it.  It does nothing
// unless there is already a valid context established

#ifndef __TEMPCONTEXT_H__
#define __TEMPCONTEXT_H__

#include "faker-sym.h"
#include "ContextHash.h"


namespace faker
{
	class TempContext
	{
		public:

			TempContext(Display *dpy_, GLXDrawable draw, GLXDrawable read,
				GLXContext ctx) : dpy(dpy_), oldctx(backend::getCurrentContext()),
				oldread(backend::getCurrentReadDrawable()),
				olddraw(backend::getCurrentDrawable()), ctxChanged(false)
			{
				if(!ctx) THROW("Invalid argument");
				if((read || draw)
					&& (oldread != read  || olddraw != draw || oldctx != ctx))
				{
					if(!backend::makeCurrent(dpy, draw, read, ctx))
						THROW("Could not bind OpenGL context to window (window may have disappeared)");
					// If oldctx has already been destroyed, then we don't want to
					// restore it.  This can happen if the application is rendering to
					// the front buffer and glXDestroyContext() is called to destroy the
					// active context before glXMake*Current*() is called to switch it.
					if((oldctx && ctxhash.findConfig(oldctx))
						|| (!oldread && !olddraw && !oldctx))
						ctxChanged = true;
				}
			}

			~TempContext(void)
			{
				if(ctxChanged)
				{
					backend::makeCurrent(dpy, olddraw, oldread, oldctx);
					ctxChanged = false;
				}
			}

		private:

			Display *dpy;
			GLXContext oldctx;
			GLXDrawable oldread, olddraw;
			bool ctxChanged;
	};
}

#endif  // __TEMPCONTEXT_H__
