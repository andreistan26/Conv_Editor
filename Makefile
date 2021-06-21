CC = gcc
CXX = g++ 

EXE = ex
Vendor = Vendor
IMGUI_DIR = imgui
FILE_DIALOG_DIR = ImGui-Addons
SOURCES = new_main.cpp
SOURCES += $(Vendor)/$(IMGUI_DIR)/imgui.cpp $(Vendor)/$(IMGUI_DIR)/imgui_demo.cpp $(Vendor)$(IMGUI_DIR)/imgui_draw.cpp $(Vendor)/$(IMGUI_DIR)/imgui_tables.cpp $(Vendor)$(IMGUI_DIR)/imgui_widgets.cpp
SOURCES += $(Vendor)/$(IMGUI_DIR)/backends/imgui_impl_sdl.cpp $(Vendor)$(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp $(Vendor)$(FILE_DIALOG_DIR)/FileBrowser/ImGuiFileBrowser.cpp Layer.cpp image_loader.cpp
OBJS = $(addprefix build/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))
UNAME_S := $(shell uname -s)

CXXFLAGS = -I$(Vendor)/$(IMGUI_DIR) -I$(Vendor)/$(IMGUI_DIR)/backends -I$(Vendor)/Glad -I$(Vendor)/$(FILE_DIALOG_DIR)/FileBrowser -I$(Vendor)/Stb 
CXXFLAGS += -g -Wall -Wformat
LIBS =

SOURCES += glad.c
CXXFLAGS += -I Glad/ -DIMGUI_IMPL_OPENGL_LOADER_GLAD -lSDL2_image

ifeq ($(UNAME_S), Linux)
	ECHO_MESSAGE = "Linux"
	LIBS += -lGL -ldl `sdl2-config --libs`
	LIBS += `pkg-config --cflags --libs opencv4`
	CXXFLAGS += `sdl2-config --cflags`
	CFLAGS = $(CXXFLAGS)
endif

build/%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(LIBS)

build/%.o:$(Vendor)/$(FILE_DIALOG_DIR)/FileBrowser/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

build/%.o:$(Vendor)/$(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

build/%.o:$(Vendor)/$(IMGUI_DIR)/backends/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

build/%.o:../libs/gl3w/GL/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

build/%.o:$(Vendor)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(EXE)
	@echo Build complete for $(ECHO_MESSAGE)

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS) 

clean:
	rm -f $(EXE) $(OBJS)
