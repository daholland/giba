add_library(gibalib
        gibalib.cpp
        gibalib.h
        )

target_include_directories(gibalib PUBLIC gibalib/)


target_link_libraries(gibalib sqlite_orm::sqlite_orm)