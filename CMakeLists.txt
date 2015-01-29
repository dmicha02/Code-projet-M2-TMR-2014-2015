cmake_minimum_required(VERSION 2.8)

project(Photonique)

set(OpenCV_DIR "C:/opencv3/build")
find_package(OpenCV REQUIRED)

include_directories(
	./
	./Matching
	${OpenCV_INCLUDE_DIRS}
)

add_executable(Matching
	Matching/ECC.cpp
	Matching/EstimateRigidT.cpp
	Matching/TemplateMatching.cpp
	Matching/stitching.cpp
	Matching/ECC.h
	Matching/EstimateRigidT.h
	Matching/TemplateMatching.h
	Matching/stitching.h
	correction.cpp
	correction.h
	io.cpp
	io.h
	main.cpp
	matcher.cpp
	loadtextfile.cpp
	data2img.cpp
	matcher.h
	optimization.h
	timer.h
	loadtextfile.h
	data2img.h
)
		
target_link_libraries(Matching
	${OpenCV_LIBS}
)

	
