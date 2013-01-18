solution "Raytracer"
	configurations	{ "Debug", "Release" }
		if os.get() == "windows" then
			platforms { "x32", "x64"}
		elseif os.get() == "macosx" then
			platforms { "Universal" }
		end

project "Raytracer"
	kind		"ConsoleApp"
	language	"C++"

files
{
	"src/*.h",
	"src/*.cpp"
}

defines
{
    "FREEGLUT_STATIC"
}

pchheader "src/PCH.h"
pchsource "src/PCH.cpp"

configuration "windows"
    buildoptions { "-Zm256" }
    includedirs{
        "$(BOOSTDIR)",
        "$(GLMDIR)",
        "$(GLUTDIR)/include"
    }

configuration {"windows","x32"}
    libdirs{
        "$(GLUTDIR)/lib/x86"
    }

configuration {"windows","x64"}
    libdirs{
        "$(GLUTDIR)/lib/x64"
    }

configuration "macosx"
    includedirs{
        "../boost",
        "../glm"
    }
    links{
        "OpenGL.framework",
        "GLUT.framework"
    }



