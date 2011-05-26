/*
 *  This file is part of Vertigo.
 *
 *  Vertigo - a cross-platform arcade game powered by Ogre3D.
 *  Copyright (C) 2011  Ahmad Amireh <ahmad@amireh.net>
 * 
 *  Vertigo is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Vertigo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Vertigo.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef H_Listener_H
#define H_Listener_H

namespace Pixy
{
	/*! \class Listener
	 *	Listeners receive events and process them using
	 *	their event handlers.
	 */
    class Event;
    class Listener
		{
        public:
            inline virtual ~Listener(){ };
            virtual bool process(Event*)=0;
			
		protected:
			virtual bool dispatch(Event*)=0;
			
		};
}
#endif // H_Listener_H
