cmake_minimum_required(VERSION 2.8.10)

# Setup basic variables.
project(qonverter)
set(APP_NAME "Qonverter")
set(APP_LOW_NAME "qonverter")
set(APP_VERSION "1.0.7-alpha")
set(APP_AUTHOR "Martin Rotter")
set(APP_URL "http://code.google.com/p/qonverter")

message(STATUS "[qonverter] Welcome to Qonverter compilation process.")
message(STATUS "[qonverter] Compilation process begins right now.")

# Setup name for executable file.
if(APPLE)
    set(EXE_NAME "Qonverter")
else()
    set(EXE_NAME "qonverter")
endif()

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Obtain revision number if available.
if(EXISTS "${PROJECT_SOURCE_DIR}/.git")
    find_package(Git)
    if(GIT_FOUND)
	message(STATUS "[qonverter] Git is installed. Obtaining revision number.")
	execute_process(
	    COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
	    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	    OUTPUT_VARIABLE _git_revision
	)
	string(REGEX REPLACE "\n" "" _git_revision "${_git_revision}")
	set(APP_REVISION ${_git_revision})
    else(GIT_FOUND)
        set(APP_REVISION UNKNOWN-REVISION)
    endif(GIT_FOUND)
else(EXISTS "${PROJECT_SOURCE_DIR}/.git")
    set(APP_REVISION STABLE)
endif(EXISTS "${PROJECT_SOURCE_DIR}/.git")

# Configure internal C++ defines.
configure_file (
    ${PROJECT_SOURCE_DIR}/src/defs.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/src/defs.h
)

# Configure executable "properties" for Windows.
configure_file (
    ${PROJECT_SOURCE_DIR}/resources/qonverter_win.rc.in
    ${CMAKE_CURRENT_BINARY_DIR}/resources/qonverter_win.rc
)

# Configure desktop entry for Linux.
if(UNIX)
message(STATUS "[qonverter] Generating desktop entry.")
configure_file (
    ${PROJECT_SOURCE_DIR}/resources/desktop/qonverter.desktop.in
    ${CMAKE_CURRENT_BINARY_DIR}/resources/desktop/qonverter.desktop
)
endif()

# Sets correct program for embedding icon into executable with MinGW compiler.
add_definitions(-DWINDRES="${WINDRES}")

# Define some useful DEBUG for, ehrm, debug build.
if(CMAKE_BUILD_TYPE STREQUAL "release" OR CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RELEASE")
    message(STATUS "[qonverter] A release build (non-debug). Debugging outputs are silently ignored.")
    #add_definitions(-QT_NO_DEBUG_OUTPUT)
    set(CMAKE_BUILD_TYPE release)
else()
    message(STATUS "[qonverter] A debug build. -DDEBUG is defined")
    add_definitions(-DDEBUG)
    set(CMAKE_BUILD_TYPE debug)
# Enable compiler warnings.
# See http://stackoverflow.com/questions/2368811/how-to-set-warning-level-in-cmake
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-pedantic -Wall -Wextra)
elseif(CMAKE_BUILD_TOOL MATCHES "(msdev|devenv|nmake)")
    add_definitions(/W2)
endif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
endif()

# Check for C++ 11 features availability.
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    execute_process(
	COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION
    )
    if(NOT (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7))
        message(FATAL_ERROR "Your C++ compiler does not support C++ 11.")
    else()
	add_definitions(-std=c++11)
    endif()
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
elseif(${MSVC_VERSION} VERSION_LESS 1600)
    message(FATAL_ERROR "Your C++ compiler does not support C++ 11.")
endif()

# Unicode settings.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUNICODE")
add_definitions(-DUNICODE -D_UNICODE)
if(WIN32)
    # UNICODE support with Visual C++ and MinGW.
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_UNICODE")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_UNICODE")
endif()

# Find all needed Qt modules.
if(UNIX)
    find_package(Qt5DBus)
endif()
find_package(Qt5Sql)
find_package(Qt5Widgets)
find_package(Qt5Xml)
find_package(Qt5Network)
find_package(Qt5LinguistTools)

# Compile application icon if compiling with MinGW.
if(MINGW)
    set(WINDRES windres.exe)
    message(STATUS "[qonverter] MinGW compilation is selected. Icon file binary will be builded.")
    message(STATUS "[qonverter] Used windres tool is: " ${WINDRES})
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/qonverter.o
	COMMAND ${WINDRES}  -I${CMAKE_CURRENT_BINARY_DIR}
			    -i${CMAKE_CURRENT_BINARY_DIR}/resources/qonverter_win.rc
			    -o ${CMAKE_CURRENT_BINARY_DIR}/qonverter.o
    )
    set(APP_SOURCES ${APP_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/qonverter.o)
# MSVC takes care of this automatically.
else()
    set(APP_SOURCES ${APP_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/resources/qonverter_win.rc)
endif()

# Add source files.
set(APP_SOURCES
    ${APP_SOURCES}
    src/main.cpp
    src/muparserx/mpError.cpp
    src/muparserx/mpIValue.cpp
    src/muparserx/mpPackageUnit.cpp
    src/muparserx/mpFuncCmplx.cpp
    src/muparserx/mpOprtBinAssign.cpp
    src/muparserx/mpParser.cpp
    src/muparserx/mpFuncCommon.cpp
    src/muparserx/mpOprtBinCommon.cpp
    src/muparserx/mpOprtPostfixCommon.cpp
    src/muparserx/mpParserBase.cpp
    src/muparserx/mpFuncMatrix.cpp
    src/muparserx/mpOprtCmplx.cpp
    src/muparserx/mpRPN.cpp
    src/muparserx/mpFuncNonCmplx.cpp
    src/muparserx/mpOprtIndex.cpp
    src/muparserx/mpScriptTokens.cpp
    src/muparserx/mpFuncStr.cpp
    src/muparserx/mpOprtMatrix.cpp
    src/muparserx/mpTest.cpp
    src/muparserx/mpICallback.cpp
    src/muparserx/mpOprtNonCmplx.cpp
    src/muparserx/mpTokenReader.cpp
    src/muparserx/mpIfThenElse.cpp
    src/muparserx/mpPackageCmplx.cpp
    src/muparserx/mpValReader.cpp
    src/muparserx/mpIOprt.cpp
    src/muparserx/mpPackageCommon.cpp
    src/muparserx/mpValue.cpp
    src/muparserx/mpIPackage.cpp
    src/muparserx/mpPackageMatrix.cpp
    src/muparserx/mpValueCache.cpp
    src/muparserx/mpIToken.cpp
    src/muparserx/mpPackageNonCmplx.cpp
    src/muparserx/mpVariable.cpp
    src/muparserx/mpIValReader.cpp
    src/muparserx/mpPackageStr.cpp
    src/muparserx/mpParserMessageProvider.cpp
    src/debug.cpp
    src/date.cpp
    src/settings.cpp
    src/formabout.cpp
    src/formmain.cpp
    src/formsettings.cpp
    src/database.cpp
    src/lineedit.cpp
    src/stackedwidget.cpp
    src/calculator/formcalculator.cpp
    src/calculator/calculatorinput.cpp
    src/calculator/calculatorhighlighter.cpp
    src/calculator/calculator.cpp
    src/calculator/calculatorwrapper.cpp
    src/calculator/calculatorbutton.cpp
    src/uifactory.cpp
    src/calculator/floatinglabel.cpp
    src/unitconverter/formunitconverter.cpp
    src/unitconverter/unitconverter.cpp
    src/currencyconverter/formcurrencyconverter.cpp
    src/currencyconverter/currencyconverter.cpp
    src/downloader.cpp
    src/systemtrayicon.cpp
    src/calculator/formvariables.cpp
    src/calculator/constantsmodel.cpp
    src/calculator/constantsview.cpp
    src/calculator/formeditvariable.cpp
    src/formwelcome.cpp
    src/calculator/memoryplace.cpp
    src/calculator/memorymap.cpp
    src/calculator/constantsfiltermodel.cpp
)

# Add headers.
set(APP_HEADERS
    ${APP_HEADERS}
    src/formmain.h
    src/formabout.h
    src/formsettings.h
    src/lineedit.h
    src/stackedwidget.h
    src/calculator/formcalculator.h
    src/calculator/calculatorinput.h
    src/calculator/calculatorhighlighter.h
    src/calculator/calculator.h
    src/calculator/calculatorwrapper.h
    src/calculator/calculatorbutton.h
    src/calculator/floatinglabel.h
    src/unitconverter/formunitconverter.h
    src/unitconverter/unitconverter.h
    src/currencyconverter/formcurrencyconverter.h
    src/currencyconverter/currencyconverter.h
    src/systemtrayicon.h
    src/calculator/formvariables.h
    src/calculator/constantsmodel.h
    src/calculator/constantsview.h
    src/calculator/formeditvariable.h
    src/formwelcome.h
    src/calculator/constantsfiltermodel.h
)

# Add form files.
set(APP_FORMS
    ui/formmain.ui
    ui/formabout.ui
    ui/formsettings.ui
    ui/calculator/formcalculator.ui
    ui/unitconverter/formunitconverter.ui
    ui/currencyconverter/formcurrencyconverter.ui
    ui/calculator/formvariables.ui
    ui/calculator/formeditvariable.ui
    ui/formwelcome.ui
)

# Add resources.
set(APP_RESOURCES
    resources/qonverter.qrc
)

# Add translations.
set(APP_TRANSLATIONS
    localization/qonverter_en.ts
    localization/qonverter_cs.ts
    localization/qt_cs.ts
)

set(APP_TRANSLATIONS_WO_QT
    localization/qonverter_en.ts
    localization/qonverter_cs.ts
)

# Add skins.
set(APP_SKIN_PLAIN
    resources/skins/base/plain.qss
)

set(APP_SKIN_MODERN
    resources/skins/base/modern.qss
)

set(APP_SKIN_DARK
    resources/skins/fancy/dark.qss
)

set(APP_SKIN_DARK_IMAGES
    resources/skins/fancy/images/down_arrow.png
    resources/skins/fancy/images/handle.png
    resources/skins/fancy/images/checkbox.png
)

# Add needed text files.
set(APP_MISC
    resources/text/COPYING_HTML
    resources/text/AUTHORS
    resources/text/COPYING
    resources/text/CHANGELOG
)

# Wrap files, create moc files.
qt5_wrap_cpp(APP_MOC ${APP_HEADERS})
qt5_wrap_ui(APP_UI ${APP_FORMS})
qt5_add_resources(APP_RCC ${APP_RESOURCES})

if(Qt5LinguistTools_FOUND)
    message(STATUS "[qonverter] Qt Linguist Tools found. Translations will get refreshed.")
    qt5_add_translation(APP_QM ${APP_TRANSLATIONS})
else()
    message(STATUS "[qonverter] Qt Linguist Tools NOT found. No refreshing for translations.")
endif()

# Include additional directory paths.
include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/muparserx
    ${CMAKE_CURRENT_SOURCE_DIR}/src/calculator
    ${CMAKE_CURRENT_SOURCE_DIR}/src/unitconverter
    ${CMAKE_CURRENT_SOURCE_DIR}/src/currencyconverter
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/src
    ${Qt5Xml_INCLUDE_DIRS}
)

if(WIN32)
    add_executable(${EXE_NAME} WIN32
        ${APP_SOURCES}
        ${APP_FORMS}
        ${APP_RCC}
        ${APP_QM}
    )
    target_link_libraries(${EXE_NAME} Qt5::WinMain)
else()
    add_executable(${EXE_NAME}
        ${APP_SOURCES}
        ${APP_FORMS}
        ${APP_RCC}
        ${APP_QM}
    )
endif()

# Use modules from Qt.
qt5_use_modules(${EXE_NAME}
    Core
    Widgets
    Sql
    Network
    Xml
)

# Installation stage.
#if(APPLE)
#    # Mac's bundle install.
#    message(STATUS "[qonverter] You will probably install on Mac OS.")
#    set_target_properties(${EXE_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/resources/macosx/Info.plist.in")
#    set(MACOSX_BUNDLE_ICON_FILE qonverter.icns)
#    set(MACOSX_BUNDLE_INFO_STRING "${EXE_NAME} ${APP_VERSION}")
#    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.googlecode.qonverter")
#    set(MACOSX_BUNDLE_LONG_VERSION_STRING "${APP_VERSION}")
#    set(MACOSX_BUNDLE_BUNDLE_NAME "${EXE_NAME}")
#    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${APP_VERSION}")
#    set(MACOSX_BUNDLE_BUNDLE_VERSION "${APP_VERSION}")
#    set(MACOSX_BUNDLE_COPYRIGHT "(c) Martin Rotter")
#    set_source_files_properties(${CMAKE_SOURCE_DIR}/resources/macosx/qonverter.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
#    install(TARGETS ${EXE_NAME} BUNDLE DESTINATION ${CMAKE_INSTALL_PREFIX})
#    install(FILES ${APP_QM} DESTINATION ${CMAKE_INSTALL_PREFIX}/${EXE_NAME}.app/Contents/Resources/l10n)
#    install(FILES ${APP_SKINS} DESTINATION ${CMAKE_INSTALL_PREFIX}/${EXE_NAME}.app/Contents/Resources/skins)
#    install(FILES ${APP_MISC} DESTINATION ${CMAKE_INSTALL_PREFIX}/${EXE_NAME}.app/Contents/Resources/information)
#    configure_file(resources/macosx/bundle.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/bundle.cmake @ONLY)
#    add_custom_target(bundle ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/bundle.cmake)
#elseif(WIN32)
if(WIN32)
    message(STATUS "[qonverter] You will probably install on Windows.")
    install(TARGETS ${EXE_NAME} RUNTIME DESTINATION ./)
    install(FILES ${APP_QM} DESTINATION ./l10n)
    install(FILES ${APP_SKIN_PLAIN} DESTINATION ./skins/base)
    install(FILES ${APP_SKIN_MODERN} DESTINATION ./skins/base)
    install(FILES ${APP_SKIN_DARK} DESTINATION ./skins/fancy)
    install(FILES ${APP_SKIN_DARK_IMAGES} DESTINATION ./skins/fancy/images)
    install(FILES ${APP_MISC} DESTINATION ./)
elseif(OS2)
    message(STATUS "[qonverter] You will probably install on OS/2.")
    install(TARGETS ${EXE_NAME} RUNTIME DESTINATION ./)
    install(FILES ${APP_QM} DESTINATION ./l10n)
    install(FILES ${APP_SKIN_PLAIN} DESTINATION ./skins/base)
    install(FILES ${APP_SKIN_MODERN} DESTINATION ./skins/base)
    install(FILES ${APP_SKIN_DARK} DESTINATION ./skins/fancy)
    install(FILES ${APP_SKIN_DARK_IMAGES} DESTINATION ./skins/fancy/images)
    install(FILES ${APP_MISC} DESTINATION ./)
elseif(UNIX)
    message(STATUS "[qonverter] You will probably install on Linux.")
    install(TARGETS ${EXE_NAME} RUNTIME DESTINATION bin)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/resources/desktop/qonverter.desktop DESTINATION share/applications)
    install(FILES resources/graphics/qonverter.png DESTINATION share/icons/hicolor/256x256/apps/)
    install(FILES ${APP_QM} DESTINATION share/qonverter/l10n)
    install(FILES ${APP_SKIN_PLAIN} DESTINATION share/qonverter/skins/base)
    install(FILES ${APP_SKIN_MODERN} DESTINATION share/qonverter/skins/base)
    install(FILES ${APP_SKIN_DARK} DESTINATION share/qonverter/skins/fancy)
    install(FILES ${APP_SKIN_DARK_IMAGES} DESTINATION share/qonverter/skins/fancy/images)
    install(FILES ${APP_MISC} DESTINATION share/qonverter/information)
endif()

# Custom target for packaging.
set(CPACK_PACKAGE_NAME ${APP_LOW_NAME})
set(CPACK_PACKAGE_VERSION ${APP_VERSION})
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-src")
set(CPACK_IGNORE_FILES "/CVS/;/\\\\.svn/;/\\\\.git/;\\\\.swp$;/CMakeLists.txt.user;\\\\.#;/#;\\\\.tar.gz$;/CMakeFiles/;CMakeCache.txt;\\\\.qm$;/build/;\\\\.diff$;.DS_Store'")
set(CPACK_SOURCE_IGNORE_FILES ${CPACK_IGNORE_FILES})

# Load packaging facilities.
include(CPack)

# make dist implementation.
add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)

# make lupdate and make lrelease implementation.
add_custom_target(lupdate
    ${Qt5Core_QMAKE_EXECUTABLE} -project -o ${CMAKE_CURRENT_BINARY_DIR}/qonverter.pro
        COMMAND ${Qt5_LUPDATE_EXECUTABLE} -ts ${APP_TRANSLATIONS_WO_QT} -no-obsolete -pro ${CMAKE_CURRENT_BINARY_DIR}/qonverter.pro
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_custom_target(lrelease
    ${Qt5Core_QMAKE_EXECUTABLE} -project -o ${CMAKE_CURRENT_BINARY_DIR}/qonverter.pro
        COMMAND ${Qt5_LRELEASE_EXECUTABLE} -compress ${CMAKE_CURRENT_BINARY_DIR}/qonverter.pro
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
