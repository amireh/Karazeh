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

#include <exception>
#include <stdlib.h>
#include <stdexcept>

namespace Pixy {

  /*! \brief
   * raised when our app version could not be located in the remote patch list
   */
	class BadVersion : public std::runtime_error {
	public:
		inline BadVersion(const std::string& s)
		: std::runtime_error(s)
		{ }
	};

  /*! \brief
   * raised when the downloader is unable to fetch the patch list from the patch
   * server
   */
	class BadPatchURL : public std::runtime_error {
	public:
		inline BadPatchURL(const std::string& s)
		: std::runtime_error(s)
		{ }
	};

  /*! \brief
   * raised when the Downloader has a problem fetching a file
   */
	class BadRequest : public std::runtime_error {
	public:
		inline BadRequest(const std::string& s)
		: std::runtime_error(s)
		{ }
	};

	/*! \brief
	 * raised when the received patch list from the server is malformed
	 */
	class BadPatchList : public std::runtime_error {
	public:
		inline BadPatchList(const std::string& s)
		: std::runtime_error(s)
		{ }
	};

	/*! \brief
	 * raised when the remote patch list either could not be saved to local temp
	 * file or that temp file could not be opened
	 */
	class BadFileStream : public std::runtime_error {
	public:
		inline BadFileStream(const std::string& s)
		: std::runtime_error(s)
		{ }
	};

	/*! \brief
	 * raised if Utility::convertTo<T> faces an issue
	 */
	class BadConversion : public std::runtime_error {
	public:
		inline BadConversion(const std::string& s)
		: std::runtime_error(s)
		{ }
	};

	/*! \brief
	 * raised if there was an error extracting or decompressing a BZ2 tar archive
	 */
	class BadArchive : public std::runtime_error {
	public:
		inline BadArchive(const std::string& s)
		: std::runtime_error(s)
		{ }
	};
	
	struct PatchEntry;
	class PatchException : public std::runtime_error {
	public:
	  inline PatchException(const std::string& s, PatchEntry* e)
	  : std::runtime_error(s)
	  { Entry = e; };
	  PatchEntry* Entry;
	};

	/*! \brief
	 * raised when a CREATE op is told to create an already existing file
	 */
	class FileAlreadyCreated : public PatchException {
	public:
		inline FileAlreadyCreated(const std::string& s, PatchEntry* e)
		: PatchException(s, e)
		{ }
	};

	/*! \brief
	 * raised when a DELETE op is told to delete a non-existent file
	 */
	class FileDoesNotExist : public PatchException {
	public:
		inline FileDoesNotExist(const std::string& s, PatchEntry* e )
		: PatchException(s, e)
		{ }
	};

	/*! \brief
	 * raised when a MODIFY op is told to patch an up-to-date file
	 */
	class FileAlreadyModified : public PatchException {
	public:
		inline FileAlreadyModified(const std::string& s, PatchEntry* e)
		: PatchException(s, e)
		{ }
	};
}

#endif
