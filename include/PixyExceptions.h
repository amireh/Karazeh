/*
 *  Copyright (c) 2011 Ahmad Amireh <ahmad@amireh.net>
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the 
 *  Software is furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in 
 *  all copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE. 
 *
 */
 
#ifndef H_PixyExceptions_H
#define H_PixyExceptions_H

namespace Pixy {

  /*
   * raised when our app version could not be located in the remote patch list
   */
	class BadVersion : public std::runtime_error {
	public:
		inline BadVersion(const std::string& s)
		: std::runtime_error(s)
		{ }
	};

  /*
   * raised when the downloader is unable to fetch the patch list from the patch
   * server
   */
	class BadPatchURL : public std::runtime_error {
	public:
		inline BadPatchURL(const std::string& s)
		: std::runtime_error(s)
		{ }
	};
	
	/*
	 * raised when the received patch list from the server is malformed
	 */
	class BadPatchList : public std::runtime_error {
	public:
		inline BadPatchList(const std::string& s)
		: std::runtime_error(s)
		{ }
	};
	
	/*
	 * raised when the remote patch list either could not be saved to local temp
	 * file or that temp file could not be opened
	 */
	class BadFileStream : public std::runtime_error {
	public:
		inline BadFileStream(const std::string& s)
		: std::runtime_error(s)
		{ }
	};

	class BadConversion : public std::runtime_error {
	public:
		inline BadConversion(const std::string& s)
		: std::runtime_error(s)
		{ }
	};
	
}

#endif
