cmake_minimum_required(VERSION 3.10)
project(cellautomata)

find_package(X11 REQUIRED)
find_package(Eigen3 3.3 REQUIRED)



add_executable(cellautomata 
    src/main.cpp
    src/cell_automata.cpp
)

target_link_libraries(cellautomata

    ${X11_LIBRARIES}
    Eigen3::Eigen
   
)


