CXX = g++
CXXFLAGS = -std=c++17 -fPIC -Wall
INCLUDE = -Iinclude
LIBDIR = libs

.PHONY: all macos linux clean docker_build docker_run

# по умолчанию соберём для macOS (если ты на macOS)
all: macos_all

# ----------------------------
# macOS targets (.dylib)
# ----------------------------
macos_all: $(LIBDIR)/libcalc1.dylib $(LIBDIR)/libcalc2.dylib program1_macos program2_macos

$(LIBDIR)/libcalc1.dylib:
	mkdir -p $(LIBDIR)
	$(CXX) $(CXXFLAGS) src/derivative1.cpp src/e1.cpp -dynamiclib -o $(LIBDIR)/libcalc1.dylib

$(LIBDIR)/libcalc2.dylib:
	mkdir -p $(LIBDIR)
	$(CXX) $(CXXFLAGS) src/derivative2.cpp src/e2.cpp -dynamiclib -o $(LIBDIR)/libcalc2.dylib

program1_macos:
	$(CXX) src/main_linking.cpp -L$(LIBDIR) -lcalc1 $(INCLUDE) -o program1

program2_macos:
	$(CXX) src/main_runtime.cpp $(INCLUDE) -ldl -o program2

# ----------------------------
# linux targets (.so) — useful inside Docker
# ----------------------------
linux: linux_all

linux_all: $(LIBDIR)/libcalc1.so $(LIBDIR)/libcalc2.so program1_linux program2_linux

$(LIBDIR)/libcalc1.so:
	mkdir -p $(LIBDIR)
	$(CXX) $(CXXFLAGS) -shared src/derivative1.cpp src/e1.cpp -o $(LIBDIR)/libcalc1.so

$(LIBDIR)/libcalc2.so:
	mkdir -p $(LIBDIR)
	$(CXX) $(CXXFLAGS) -shared src/derivative2.cpp src/e2.cpp -o $(LIBDIR)/libcalc2.so

program1_linux:
	$(CXX) src/main_linking.cpp -L$(LIBDIR) -lcalc1 $(INCLUDE) -o program1_linux -Wl,-rpath,$(PWD)/$(LIBDIR)

program2_linux:
	$(CXX) src/main_runtime.cpp $(INCLUDE) -ldl -o program2_linux

# ----------------------------
clean:
	rm -rf $(LIBDIR) program1 program2 program1_linux program2_linux

# ----------------------------
# Docker helpers
# ----------------------------
docker_build:
	docker build -t lab_trace .

docker_run:
	docker run --rm -it -v $(PWD):/work -w /work lab_trace /bin/bash
