#!/bin/bash
# Simutrans Windows Cross-Compile Build Script
# Docker内でMinGWを使用してWindows用バイナリをビルド

set -e

echo "=================================="
echo "Simutrans MinGW Cross-Compile"
echo "=================================="
echo ""

BUILD_TYPE=${1:-make}

case "$BUILD_TYPE" in
    make)
        echo "Building with Make (MinGW cross-compile)..."

        # Cross pkg-config / include / lib path setup
        export PKG_CONFIG_LIBDIR=/usr/x86_64-w64-mingw32/lib/pkgconfig
        export PKG_CONFIG_PATH=/usr/x86_64-w64-mingw32/lib/pkgconfig
        export PATH="/usr/x86_64-w64-mingw32/bin:$PATH"
        # Ensure the correct resource compiler and C++ compiler are used by make rules
        export WINDRES=x86_64-w64-mingw32-windres
        export HOSTCXX=x86_64-w64-mingw32-g++
        export CFLAGS="-I/usr/x86_64-w64-mingw32/include -I/usr/x86_64-w64-mingw32/include/freetype2 ${CFLAGS}"
        export CXXFLAGS="-I/usr/x86_64-w64-mingw32/include -I/usr/x86_64-w64-mingw32/include/freetype2 ${CXXFLAGS}"
        export LDFLAGS="-L/usr/x86_64-w64-mingw32/lib ${LDFLAGS}"

        # configureが存在しない場合は生成
        if [ ! -f ./configure ]; then
            echo "Generating configure script..."
            autoconf
        fi

        # 常に新規にconfig.defaultを生成してからMinGW向けに上書き
        if [ -f ./config.default ]; then
            echo "Removing stale config.default..."
            rm -f ./config.default || true
        fi
        echo "Configuring for MinGW..."
        ./configure --host=x86_64-w64-mingw32

        # MinGW用に各種設定を上書き
        if [ -f ./config.default ]; then
            sed -i 's/^OSTYPE = .*/OSTYPE := mingw/' ./config.default
            sed -i 's/^OSTYPE := .*/OSTYPE := mingw/' ./config.default
            # バックエンドはSDL2を使用
            sed -i 's/^BACKEND = .*/BACKEND := sdl2/' ./config.default || true
            sed -i 's/^BACKEND := .*/BACKEND := sdl2/' ./config.default || true
            # 不要な機能を無効化（MinGW最小構成）
            sed -i 's/^USE_FONTCONFIG := .*/USE_FONTCONFIG := 0/' ./config.default || true
            sed -i 's/^USE_FLUIDSYNTH_MIDI := .*/USE_FLUIDSYNTH_MIDI := 0/' ./config.default || true
            sed -i 's/^USE_ZSTD := .*/USE_ZSTD := 0/' ./config.default || true
            echo "Applied MinGW settings to config.default"
        fi

        echo "Building Simutrans for Windows..."
        make -j$(nproc) OSTYPE=mingw BACKEND=sdl2

        echo ""
        echo "Build complete!"
        if [ -f ./sim.exe ]; then
            echo "Windows executable: ./sim.exe"
            ls -lh ./sim.exe
        elif [ -f ./build/default/sim.exe ]; then
            echo "Windows executable: ./build/default/sim.exe"
            ls -lh ./build/default/sim.exe
        else
            echo "Warning: sim.exe not found in expected locations"
        fi
        ;;

        stage)
                echo "Staging Windows runtime DLLs..."
                set -e
                OUTDIR=dist/win64
                mkdir -p "$OUTDIR"

                # Prefer top-level sim.exe
                if [ -f ./sim.exe ]; then
                        cp -f ./sim.exe "$OUTDIR/"
                        EXE=./sim.exe
                elif [ -f ./build/default/sim.exe ]; then
                        cp -f ./build/default/sim.exe "$OUTDIR/"
                        EXE=./build/default/sim.exe
                else
                        echo "sim.exe not found. Run '$0 make' first." >&2
                        exit 1
                fi

                echo "Analyzing DLL imports from $EXE ..."
                DLLS=$(x86_64-w64-mingw32-objdump -p "$EXE" | awk '/DLL Name:/ {print $3}' | tr -d '\r')
                echo "$DLLS" | sed 's/^/  - /'

                echo "Copying non-system DLLs..."
                for d in $DLLS; do
                    case "$(echo "$d" | tr 'A-Z' 'a-z')" in
                        advapi32.dll|kernel32.dll|shell32.dll|user32.dll|winmm.dll|ws2_32.dll|msvcrt.dll)
                            continue ;;
                    esac
                    SRC="/usr/x86_64-w64-mingw32/bin/$d"
                    if [ ! -f "$SRC" ]; then
                        SRC=$(find /usr/x86_64-w64-mingw32/bin /usr/x86_64-w64-mingw32/lib /usr/lib/gcc/x86_64-w64-mingw32 -maxdepth 3 -type f -name "$d" 2>/dev/null | head -n1 || true)
                    fi
                    if [ -n "$SRC" ] && [ -f "$SRC" ]; then
                        cp -f "$SRC" "$OUTDIR/"
                    else
                        echo "Warning: $d not found in common MinGW paths" >&2
                    fi
                done

                echo "Staged files:"
                ls -lh "$OUTDIR"
                ;;

    cmake)
        echo "Building with CMake (MinGW cross-compile)..."

        mkdir -p build-mingw
        cd build-mingw

        echo "Configuring CMake for Windows cross-compile..."
        cmake \
            -DCMAKE_SYSTEM_NAME=Windows \
            -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
            -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ \
            -DCMAKE_RC_COMPILER=x86_64-w64-mingw32-windres \
            -DCMAKE_FIND_ROOT_PATH=/usr/x86_64-w64-mingw32 \
            -DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER \
            -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY \
            -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY \
            -DCMAKE_BUILD_TYPE=Release \
            -DSIMUTRANS_BACKEND=gdi \
            ..

        echo "Building..."
        cmake --build . -j$(nproc)

        echo ""
        echo "Build complete!"
        if [ -f ./simutrans/sim.exe ]; then
            echo "Windows executable: ./build-mingw/simutrans/sim.exe"
            ls -lh ./simutrans/sim.exe
        fi
        cd ..
        ;;

    clean)
        echo "Cleaning up..."
        make clean 2>/dev/null || true
        rm -rf build-mingw build/default/sim.exe sim.exe || true
        rm -f config.default || true
        echo "Cleanup complete!"
        ;;

    *)
        echo "Usage: $0 {make|stage|cmake|clean}"
        echo ""
        echo "  make   - Build with Make (default)"
        echo "  stage  - Collect required DLLs into dist/win64"
        echo "  cmake  - Build with CMake"
        echo "  clean  - Clean build artifacts"
        exit 1
        ;;
esac

echo ""
echo "Done!"
