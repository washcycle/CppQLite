#ifndef CPPSQLITEGLOBALS_H
#define CPPSQLITEGLOBALS_H

// Check if building on windows
// set the CPPSQLITE_API macro to __declspec if windows, __attribute__ if not windows
#ifdef CPPSQLITE_API_EXPORTING
	#if defined _WIN32 || defined __CYGWIN__
		#define CPPSQLITE_API __declspec(dllexport)
	#else
		#define CPPSQLITE_API __attribute__ ((dllexport))
	#endif
#else
	#define CPPSQLITE_API
#endif

// global namespace for the project
namespace sqlite{}

#endif /*CPPSQLITEGLOBALS_H*/
