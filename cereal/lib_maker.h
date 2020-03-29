#ifndef CEREAL_LIB_MAKER_H
#define CEREAL_LIB_MAKER_H

#if defined(_WIN32) || defined(_WIN64)
# define strdup _strdup
# ifdef BUILDING_DLL
#  define DLL_PUBLIC __declspec(dllexport)
# elif DEV
# define DLL_PUBLIC
# else
#  define DLL_PUBLIC __declspec(dllimport)
# endif
#else
# define DLL_PUBLIC
#endif

#endif