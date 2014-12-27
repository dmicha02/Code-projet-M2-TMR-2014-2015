cmake_minimum_required(VERSION 2.8)

project(test)

set(OpenCV_DIR "C:/opencv3/build")
find_package(OpenCV REQUIRED)

include_directories(
	./
	./Matching
	${OpenCV_INCLUDE_DIRS}
)

add_executable(testLib
	auxfunc.cpp
	forwadditive.cpp
	invcomp.cpp
	main.cpp
	stdafx.cpp
	auxfunc.h
	stdafx.h
	forwadditive.h
	invcomp.h
)
		
target_link_libraries(testLib
	${OpenCV_LIBS}
)

	
