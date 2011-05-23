#ifndef H_Utility_H
#define H_Utility_H

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
	
class Utility {

  public:
  
  /* splits a string s using the delimiter delim */
  inline static std::vector<std::string> split(const std::string &s, char delim) {
      std::vector<std::string> elems;
      std::stringstream ss(s);
      std::string item;
      while(std::getline(ss, item, delim)) {
          elems.push_back(item);
      }
      return elems;
  }

	template<typename T>
	inline std::string stringify(const T& x)
	{
		std::ostringstream o;
		if (!(o << x))
			throw BadConversion(std::string("stringify(")
								+ typeid(x).name() + ")");
		return o.str();
	}
	
	// helper; converts an integer-based type to a string
	template<typename T>
	inline void convert(const std::string& inString, T& inValue,
						bool failIfLeftoverChars = true)
	{
		std::istringstream _buffer(inString);
		char c;
		if (!(_buffer >> inValue) || (failIfLeftoverChars && _buffer.get(c)))
			throw BadConversion(inString);
	}
	
	template<typename T>
	inline T convertTo(const std::string& inString,
					   bool failIfLeftoverChars = true)
	{
		T _value;
		convert(inString, _value, failIfLeftoverChars);
		return _value;
	}
	
};

};

#endif
