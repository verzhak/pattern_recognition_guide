
cmake_minimum_required(VERSION 2.8)

find_package(OpenCV REQUIRED)

if(NOT CMAKE_BUILD_TYPE)

	#
	# Цель по умолчанию - Debug
	# Для оптимизированной цели: cmake -DCMAKE_BUILD_TYPE=Release ..
	#

	set(CMAKE_BUILD_TYPE Debug)

endif()

#
# Переменная DATA_DIR задает путь к каталогу с данными:
#
# cmake -DDATA_DIR=ПУТЬ_К_КАТАЛОГУ_С_ДАННЫМИ ..
#
if(NOT DATA_DIR)

	set(DATA_DIR "/home/amv/education/evm/met/pattern_recognition/src/data")

endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pipe -std=c++11" CACHE string "" FORCE)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/lib ${Boost_INCLUDE_DIRS})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/lib/all.hpp.in all.hpp)

set(ip_brainloller_sources image_processing/brainloller/main.cpp)
set(ip_morphology_sources image_processing/morphology/main.cpp)
set(pr_faces_sources pattern_recognition/faces/main.cpp)
set(pr_unsupervised_sources pattern_recognition/unsupervised/main.cpp pattern_recognition/unsupervised/eval.cpp pattern_recognition/unsupervised/gen.cpp)
set(pr_supervised_sources pattern_recognition/supervised/main.cpp pattern_recognition/supervised/captcha.cpp pattern_recognition/supervised/classifier.cpp)
set(pr_moving_detection_sources pattern_recognition/moving_detection/main.cpp)

add_executable(ip_brainloller ${ip_brainloller_sources})
add_executable(ip_morphology ${ip_morphology_sources})
add_executable(pr_faces ${pr_faces_sources})
add_executable(pr_unsupervised ${pr_unsupervised_sources})
add_executable(pr_supervised ${pr_supervised_sources})
add_executable(pr_moving_detection ${pr_moving_detection_sources})

target_link_libraries(ip_brainloller ${OpenCV_LIBS})
target_link_libraries(ip_morphology ${OpenCV_LIBS})
target_link_libraries(pr_faces ${OpenCV_LIBS})
target_link_libraries(pr_unsupervised ${OpenCV_LIBS})
target_link_libraries(pr_supervised ${OpenCV_LIBS} fmll_ann_pca fmll_ann_mlp)
target_link_libraries(pr_moving_detection ${OpenCV_LIBS})

