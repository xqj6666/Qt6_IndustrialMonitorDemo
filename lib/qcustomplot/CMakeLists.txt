# ============================================================
# QCustomPlot 静态库
# ============================================================

# SHARED = 动态库，STATIC = 静态库
add_library(QCustomPlot STATIC
    qcustomplot.h
    qcustomplot.cpp
)

# 开启大目标文件支持，解决 MinGW 的 "too many sections" 问题
# PUBLIC 表示：编译这个库时生效，链接这个库的项目也生效
target_compile_options(QCustomPlot PRIVATE "-Wa,-mbig-obj")

# 这个库自己需要的 Qt 模块
# PRIVATE 表示：只影响这个库自己
target_link_libraries(QCustomPlot PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::PrintSupport
)

# 设置头文件搜索路径
# PUBLIC 表示：include 路径会传递给链接这个库的项目
target_include_directories(QCustomPlot PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)